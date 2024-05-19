#include "node.h"
#include "crypto_utils.h"
#include <iostream>
#include <pthread.h>

Node::Node(int pid, const Blockchain &bc, int max_faulty): pi(pid), blockchain(bc), f(max_faulty) {}

Node::Node(int pid, const Blockchain &bc, int max_faulty, int num_nodes): pi(pid), blockchain(bc), f(max_faulty), n(num_nodes) {}

Node::~Node() {
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_cond);
}

void Node::broadcast(const Message &msg) const {
    for (Node *node : network) {
        if (node->pi != this->pi) {
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
    } else if (msgtyp == COMMIT) {
        return valid_commit_count[ri] >= ((n + f) / 2) + 1;
    }
    return false;
}

bool Node::justify_preprepare(const Message &msg) const {
    if (msg.ri == 1) {
        return true;
    }
    return false;
}

void Node::decide(const Block &block) {
    this->blockchain.add_block(block);
    std::cout << "Node " << pi << " just added the block to its blockchain!" << std::endl;
}

int Node::receive(const Message &msg) {
    bool valid_msg = verify_message(msg);
    switch (msg.type) {
        case PRE_PREPARE:
            if (valid_msg && justify_preprepare(msg)) {
                Message prepare(PREPARE, msg.lambda_i, msg.ri, msg.value, this->pi);
                this->sign_message(prepare);
                this->broadcast(prepare);
                std::cout << pi << " just broadcasted its prepare message" << std::endl;
                this->round_stage[msg.ri] = 1;
                return 1;
            }
            return 0;
        case PREPARE:
            if (valid_msg) {
                this->valid_prepare_msgs[msg.ri].push_back(msg);
                std::cout << pi << " has valid prepare msg count of " << valid_prepare_msgs[msg.ri].size() << " with the addition of node" << msg.sender << "'s prepare msg" << std::endl;
            }
            if (this->round_stage[msg.ri] == 1 && has_quorum(msg.ri, PREPARE)) {
                pr_i = msg.ri;
                pv_i = msg.value;
                Message commit(COMMIT, msg.lambda_i, msg.ri, msg.value, this->pi);
                this->round_stage[msg.ri] = 2;
                this->sign_message(commit);
                this->broadcast(commit);
                return 2;
            } 
            return 1;
        case COMMIT:
            if (this->round_stage[msg.ri] == 2 && has_quorum(msg.ri, COMMIT)) {
                this->round_stage[msg.ri] = 3;
                decide(msg.value);
                return 3;
            }
            break;
    }
    return 4;
}

void Node::run() {
    while (true) {
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
