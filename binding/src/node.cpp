#include "node.h"
#include "crypto_utils.h"
#include <cmath>
#include <iostream>
#include <pthread.h>
#include <queue>

Node::Node(int pid, const Blockchain &bc, int num_nodes, Log *log):
    log(log),
    blockchain(bc),
    pi(pid), 
    n(num_nodes),
    f((num_nodes - 1) / 3),
    lambda(-1),
    r(-1),
    pr(-1),
    pv(Block()),
    faulty(false),
    input_value(Block()),
    t0(1),
    timer(STOPPED) {}

Node::~Node() {
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_cond);
}

void Node::broadcast(const Message &msg) const {
    for (Node *node : network) {
        if (msg.type == PRE_PREPARE) {
            pthread_mutex_lock(&node->queue_mutex);
            node->message_queue.push(msg);
            pthread_cond_signal(&node->queue_cond);
            pthread_mutex_unlock(&node->queue_mutex);
        } else if (node->pi != this->pi) {
            pthread_mutex_lock(&node->queue_mutex);
            node->message_queue.push(msg);
            pthread_cond_signal(&node->queue_cond);
            pthread_mutex_unlock(&node->queue_mutex);
        }
    }
}

std::string Node::compute_hash(const std::string &data) const {
    return calculate_sha256(data);
}

bool Node::verify_signature(const std::string &hash, const std::string &signature, int sender_id) const {
    EVP_PKEY *sender_pubkey = public_keys.at(sender_id);
    return verify(hash, signature, sender_pubkey);
}

void Node::sign_message(Message &msg) const {
    std::string hash = compute_hash(msg.to_string());
    msg.sign(hash, private_key);
}

bool Node::verify_message(const Message &msg) const {
    std::string hash = compute_hash(msg.to_string());
    bool valid_sender = verify_signature(hash, msg.signature, msg.sender);
    if (!valid_sender)
        return false;
    for (const auto &tx : msg.value.transactions) {
        bool valid = verify_transaction(tx);
        if (!valid) {
            std::cout << "Node" << " " << this->pi << " failed to validate a transaction" << std::endl;
            return false;
        }
    }
    return true;
}

bool Node::verify_transaction(const Transaction &tx) const {
    std::string hash = calculate_sha256(tx.to_string());
    int sender = tx.sender;
    EVP_PKEY *pubkey = public_keys.at(sender);
    if (!pubkey)
        return false;
    return verify(hash, tx.signature, pubkey);
}

bool Node::has_quorum(int ri, MessageType msgtyp) {
    if (msgtyp == PREPARE)
        return valid_prepare_msgs[ri].size() >= 2 * f + 1;
    if (msgtyp == COMMIT)
        return valid_commit_count[ri] >= 2 * f + 1;
    return false;
}

bool Node::justify_preprepare(const Message &msg) const {
    if (msg.round == 1) {
        return true;
    }
    return false;
}

int Node::check_skip_round() const {
    std::priority_queue<int> pq;
    for (auto it = valid_roundchange_count.cbegin(); it != valid_roundchange_count.cend(); it++) {
        if (it->first > this->r) {
            pq.push(it->first);
        }
    }
    if (pq.size() >= f + 1) {
        return pq.top();
    }
    return -1;
}

void Node::handle_skip_round(int r_min) {
    this->r = r_min;
    this->set_timer(RUNNING, this->r);
    Message rc(ROUND_CHANGE, this->lambda, this->r, this->pr, this->pv, this->pi);
    this->sign_message(rc);
    this->broadcast(rc);
}

void Node::decide(const Block &block) {
    this->blockchain.add_block(block);
}

void Node::log_entry(const Message &msg) {
    std::string message_type;
    switch (msg.type) {
        case ROUND_CHANGE:
            message_type = "ROUND_CHANGE";
            break;
        case PRE_PREPARE:
            message_type = "PRE_PREPARE";
            break;
        case PREPARE:
            message_type = "PREPARE";
            break;
        case COMMIT:
            message_type = "COMMIT";
            break;
    }
    LogEntry entry = {
        message_type,
        msg.round,
        msg.sender,
        this->pi
    };
    pthread_mutex_lock(&log->mutex);
    log->entries.push_back(entry);
    pthread_mutex_unlock(&log->mutex);
}

int Node::receive(const Message &msg) {
    log_entry(msg);
    bool valid_msg = verify_message(msg);
    switch (msg.type) {
        case ROUND_CHANGE:
            if (valid_msg)
                this->valid_roundchange_count[msg.round]++;
            if (check_skip_round() != -1) {
                int rmin = check_skip_round();
                handle_skip_round(rmin);
            }
            return 4;
        case PRE_PREPARE:
            if (valid_msg && justify_preprepare(msg)) {
                Message prepare(PREPARE, msg.consensus_number, msg.round, msg.value, this->pi);
                this->round_stage[msg.round] = 1;
                this->valid_prepare_msgs[msg.round].push_back(prepare);
                if (!faulty) {
                    this->sign_message(prepare);
                }
                this->set_timer(RUNNING, msg.round);
                this->broadcast(prepare);
                return 1;
            }
            return 0;
        case PREPARE:
            if (valid_msg) {
                this->valid_prepare_msgs[msg.round].push_back(msg);
            }
            if (this->round_stage[msg.round] == 1 && has_quorum(msg.round, PREPARE)) {
                this->pr = msg.round;
                this->pv = msg.value;
                Message commit(COMMIT, msg.consensus_number, msg.round, msg.value, this->pi);
                this->round_stage[msg.round] = 2;
                this->valid_commit_count[msg.round]++;
                if (!faulty) {
                    this->sign_message(commit);
                }
                this->broadcast(commit);
                return 2;
            } 
            return 1;
        case COMMIT:
            if (valid_msg)
                this->valid_commit_count[msg.round]++;
            if (this->round_stage[msg.round] == 2 && has_quorum(msg.round, COMMIT)) {
                this->round_stage[msg.round] = 3;
                this->set_timer(STOPPED, 0);
                decide(msg.value);
                return 3;
            }
            return 2;
    }
    return 4;
}

void Node::set_timer(TimerState state, int ri) {
    this->timer = state;
    if (state == RUNNING)
        set_expiration(ri);
}

void Node::set_expiration(int ri) {
    std::chrono::seconds timeout = t(ri);
    auto now = std::chrono::steady_clock::now();
    this->expiration_time = now + timeout;
}

std::chrono::seconds Node::t(int ri) const {
    return std::chrono::seconds(static_cast<int>(t0 * std::pow(2, ri)));
}

bool Node::check_expired() {
    if (timer == STOPPED)
        return false;
    return std::chrono::steady_clock::now() >= this->expiration_time;
}

void Node::handle_timeout() {
    r++;
    set_timer(RUNNING, this->r);
    Message roundchange(ROUND_CHANGE, this->lambda, this->r, this->pr, this->pv, this->pi);
    sign_message(roundchange);
    broadcast(roundchange); 
}

int Node::leader() {
    return (lambda + r) % n;
}

void Node::listen() {
    if (leader() == pi) {
        Message preprepare(PRE_PREPARE, lambda, r, input_value, pi);
        sign_message(preprepare);
        broadcast(preprepare);
        round_stage[1] = 1;
        set_timer(RUNNING, r);
    }

    while (true) {
        bool expired = this->check_expired();
        if (expired) {
            handle_timeout();
        }
        pthread_mutex_lock(&queue_mutex);
        while (message_queue.empty()) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        Message msg = message_queue.front();
        message_queue.pop();
        pthread_mutex_unlock(&queue_mutex);

        int result = receive(msg);
        // Support just 1 round right now
        if (result >= 3) {
            return;
        }
    }
}
