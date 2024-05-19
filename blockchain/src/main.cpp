#include "crypto_utils.h"
#include "blockchain.h"
#include "node.h"
#include <cstdlib>
#include <iostream>
#include <openssl/evp.h>
#include <pthread.h>
#include <string>
#include <unordered_map>

void *worker(void *arg) {
    Node *node = static_cast<Node *>(arg);
    node->run();
    return nullptr;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage:" << " " << argv[0] << " " << "<number_of_nodes>" << std::endl;
        return 1;
    }
    // ibft can tolerate up to 3f + 1 faulty nodes
    int n = std::atoi(argv[1]);
    int f = (n - 1) / 3;

    // Initialize the blockchain
    Blockchain bc("meow");

    std::unordered_map<int, EVP_PKEY *> public_keys;
    std::vector<Node *> network;
    std::vector<pthread_t> threads(n);

    // Initialize the n nodes
    for (int i = 0; i < n; i++) {
        Node *node = new Node(i, bc, f);
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

    for (int i = 0; i < n; i++) {
        pthread_create(&threads[i], nullptr, worker, network[i]);
    }
    
    Node *leader = network[0];
    Block proposed(bc.chain.size(), {}, bc.chain[0].hash);
    Message preprepare(PRE_PREPARE, 1, 1, proposed, 0);
    leader->sign_message(preprepare);
    leader->broadcast(preprepare);
    std::cout << "Leader just broadcasted the preprepare message!" << std::endl;

    for (int i = 0; i < n; i++) {
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
