#include "node.h"
#include "crypto_utils.h"
#include <cmath>
#include <iostream>
#include <pthread.h>
#include <queue>

Node::Node(int pid, const Blockchain &bc, int max_faulty): pi(pid), blockchain(bc), f(max_faulty), timer(STOPPED), t0(1), n((3 * max_faulty) + 1) {}

Node::Node(int pid, const Blockchain &bc, int max_faulty, int num_nodes): pi(pid), blockchain(bc), f(max_faulty), n(num_nodes), timer(STOPPED), t0(1) {}

Node::Node(int pid, const Blockchain &bc, int max_faulty, int num_nodes, int timer_constant): pi(pid), blockchain(bc), f(max_faulty), n(num_nodes), timer(STOPPED), t0(timer_constant) {}

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
    return verify_signature(hash, msg.signature, msg.sender);
}

bool Node::has_quorum(int ri, MessageType msgtyp) {
    if (msgtyp == PREPARE) {
        return valid_prepare_msgs[ri].size() >= ((n + f) / 2) + 1;
    }  
    if (msgtyp == COMMIT) {
        return valid_commit_count[ri] >= ((n + f) / 2) + 1;
    }
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
    std::cout << "Node " << pi << " just added the block to its blockchain!" << std::endl;
}

int Node::receive(const Message &msg) {
    bool valid_msg = verify_message(msg);
    switch (msg.type) {
        case ROUND_CHANGE:
            if (valid_msg) {
                this->valid_roundchange_count[msg.round]++;
            }
            if (check_skip_round() != -1) {
                int rmin = check_skip_round();
                handle_skip_round(rmin);
            }
            return 4;
        case PRE_PREPARE:
            if (valid_msg && justify_preprepare(msg)) {
                Message prepare(PREPARE, msg.consensus_number, msg.round, msg.value, this->pi);
                this->sign_message(prepare);
                this->set_timer(RUNNING, msg.round);
                this->broadcast(prepare);
                this->round_stage[msg.round] = 1;
                return 1;
            }
            return 0;

        case PREPARE:
            if (valid_msg)
                this->valid_prepare_msgs[msg.round].push_back(msg);
            if (this->round_stage[msg.round] == 1 && has_quorum(msg.round, PREPARE)) {
                this->pr = msg.round;
                this->pv = msg.value;
                Message commit(COMMIT, msg.consensus_number, msg.round, msg.value, this->pi);
                this->round_stage[msg.round] = 2;
                this->sign_message(commit);
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
    return std::chrono::steady_clock::now() >= this->expiration_time;
}

void Node::handle_timeout() {
    this->r++;
    set_timer(RUNNING, this->r);
}

void Node::run() {
    while (true) {
        bool expired = this->check_expired();
        if (expired) {
            std::cout << "node " << pi << " timed out" << std::endl;
            return;
        }
        pthread_mutex_lock(&queue_mutex);
        while (message_queue.empty()) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        Message msg = message_queue.front();
        message_queue.pop();
        pthread_mutex_unlock(&queue_mutex);

        int result = receive(msg);
        if (result == 3) {
            return;
        }
    }
}
