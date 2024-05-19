#include "crypto_utils.h"
#include "blockchain.h"
#include "node.h"
#include <iostream>
#include <openssl/evp.h>
#include <pthread.h>
#include <string>
#include <unordered_map>

void *worker(void *arg) {
    Node *node = static_cast<Node *>(arg);
    if (node->pi != 0) {
        node->run();
    }
    return nullptr;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage:" << " " << argv[0] << " " << "<number_of_nodes>" << std::endl;
        return 1;
    }
    int n = std::atoi(argv[1]);

    // Initialize the blockchain
    Blockchain bc("meow");

    std::unordered_map<int, EVP_PKEY *> public_keys;
    std::vector<Node *> network;
    std::vector<pthread_t> threads(n);
    // Initialize the n nodes
    for (int i = 0; i < n; i++) {
        Node *node = new Node(i, bc);
        network.push_back(node);
        auto [pubkey, privkey] = generate_RSA_keypair();
        public_keys[i] = pubkey;
        node->private_key = privkey;
    }
    // Then add the network to each node
    for (int i = 0; i < n; i++) {
        network[i]->network = network;
        network[i]->public_keys = public_keys;
    }

    for (int i = 1; i < n; i++) {
        pthread_create(&threads[i], nullptr, worker, network[i]);
    }

    Block random_block(bc.chain.size(), {}, bc.chain[0].hash);
    Message msg(PREPARE, 1, 1, random_block, 0);
    std::string serialized = msg.to_string();
    std::string hash = calculate_sha256(serialized);
    msg.sign(hash, network[0]->private_key);
    network[0]->broadcast(msg);

    for (int i = 1; i < n; i++) {
        pthread_join(threads[i], nullptr);
    }

    // cleanup
    for (auto &pair : public_keys)
        EVP_PKEY_free(pair.second);
    for (int i = 0; i < network.size(); i++) {
        EVP_PKEY_free(network[i]->private_key);
        delete network[i];
    }
    std::cout << "here!" << std::endl;
    return 0;
}
