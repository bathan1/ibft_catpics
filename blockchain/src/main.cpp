#include "crypto_utils.h"
#include "blockchain.h"
#include "node.h"
#include <cstdlib>
#include <iostream>
#include <openssl/evp.h>
#include <pthread.h>
#include <string>
#include <unordered_map>

struct WorkerArgs {
    Node *node;
    size_t lambda;
    Block *value;
};

void *worker(void *arg) {
    WorkerArgs *args = static_cast<WorkerArgs *>(arg);
    Node *node = args->node;
    node->lambda_i = args->lambda;
    node->ri = 1;
    node->pr_i = -1;
    node->pv_i = Block();
    node->inputvalue_i = *(args->value);

    if (node->pi == 0) {
        Message preprepare(PRE_PREPARE, 1, 1, node->inputvalue_i, 0);
        node->sign_message(preprepare);
        node->broadcast(preprepare);
        node->round_stage[1] = 1;
        std::cout << "leader just broadcasted the preprepare message" << std::endl;
    }

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

    std::cout << "n=" << n << "\n";
    std::cout << "f=" << f << "\n";
    std::cout << "quorum=" << ((n + f) / 2) + 1 << "\n";

    // Initialize the blockchain
    Blockchain bc("meow");

    std::unordered_map<int, EVP_PKEY *> public_keys;
    std::vector<Node *> network;
    std::vector<pthread_t> threads(n);
    std::vector<WorkerArgs *> worker_args(n);

    // Initialize the n nodes
    for (int i = 0; i < n; i++) {
        Node *node = new Node(i, bc, f);
        node->n = n;
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

    Node *leader = network[0];
    Block proposed(bc.chain.size(), {}, bc.chain[0].hash);

    for (int i = 0; i < n; i++) {
        WorkerArgs *args = new WorkerArgs();
        *(args) = { network[i], bc.chain.size(), &proposed };
        worker_args.push_back(args);
        pthread_create(&threads[i], nullptr, worker, args);
    }
    
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], nullptr);
    }

    // cleanup
    for (int i = 0; i < worker_args.size(); i++)
        delete worker_args[i];
    for (auto &pair : public_keys)
        EVP_PKEY_free(pair.second);
    for (int i = 0; i < network.size(); i++) {
        EVP_PKEY_free(network[i]->private_key);
        delete network[i];
    }
    std::cout << "here!" << std::endl;
    return 0;
}
