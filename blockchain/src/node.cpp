#include "node.h"
#include "crypto_utils.h"
#include <iostream>
#include <pthread.h>

Node::Node(int pid, const Blockchain &bc): pi(pid), blockchain(bc) {}

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

bool Node::verify_message(const Message &msg) const {
    std::string hash = compute_hash(msg.to_string());
    return verify_signature(hash, msg.signature, msg.sender);
}

void Node::receive(const Message &msg) {
    switch (msg.type) {
        case PREPARE:
            if (verify_message(msg)) {
                std::cout << "Node " << pi << " just verified node " << msg.sender << "'s message!" << std::endl;
                pr_i = msg.ri;
                pv_i = msg.value;
            } else {
                std::cout << "Node " << pi << " could not verify node " << msg.sender << "'s message." << std::endl;
            }
    }
}

void Node::run() {
    bool received = false;
    while (!received) {
        pthread_mutex_lock(&queue_mutex);
        while (message_queue.empty()) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }

        received = true;
        Message msg = message_queue.front();
        message_queue.pop();
        pthread_mutex_unlock(&queue_mutex);

        receive(msg);
    }
}
