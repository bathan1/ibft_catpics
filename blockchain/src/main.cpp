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
    node->lambda = args->lambda;
    node->r = 1;
    node->pr = -1;
    node->pv = Block();
    node->input_value = *(args->value);

    if (node->pi == 0) {
        Message preprepare(PRE_PREPARE, node->lambda, node->r, node->input_value, 0);
        node->sign_message(preprepare);
        node->broadcast(preprepare);
        node->round_stage[1] = 1;
        std::cout << "leader just broadcasted the preprepare message" << std::endl;
    }
    node->set_timer(RUNNING, node->r);
    node->run();
    return nullptr;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Usage:" << " " << argv[0] << " " << "<number_of_nodes>" << " " << "<number_of_faulty>" << std::endl;
        return 1;
    }
    // ibft can tolerate up to 3f + 1 faulty nodes
    int n = std::atoi(argv[1]);
    int f = (n - 1) / 3;
    int actual_f = std::atoi(argv[2]);

    std::srand(std::time(nullptr));
    std::cout << "n=" << n << "\n";
    std::cout << "f=" << f << "\n";
    std::cout << "actual number of faulty " << actual_f << "\n";
    std::cout << "quorum=" << 2 * f + 1 << "\n";
    if (actual_f > f) {
        std::cout << "Max number of faulty is " << f << " please enter a valid number of faulty nodes\n";
        return 0;
    }

    // Initialize the blockchain
    Blockchain bc("meow");

    std::unordered_map<int, EVP_PKEY *> public_keys;
    std::vector<Node *> network;
    std::vector<pthread_t> threads(n);
    std::vector<WorkerArgs *> worker_args(n);

    // Initialize the n nodes
    for (int i = 0; i < n; i++) {
        Node *node = new Node(i, bc, f, n, false);
        network.push_back(node);
        auto [pubkey, privkey] = generate_RSA_keypair();
        public_keys[i] = pubkey;
        node->private_key = privkey;
    }

    int num_faulty = 0; 
    // Initialize faulty nodes
    for (int i = 0; i < actual_f; i++) {
        int random = rand() % network.size();
        while (network[random]->faulty)
            random = rand() % network.size();
        network[random]->faulty = true;
        std::cout << "Initialized node " << random << " as faulty!\n";
    }
    
    // Then add the network to each node
    for (int i = 0; i < n; i++) {
        network[i]->network = network;
        network[i]->public_keys = public_keys;
    }

    Node *leader = network[0];
    Transaction t1(0, 0, 0, "meoww");
    t1.sign(leader->private_key);
    Block proposed(bc.chain.size(), {t1}, bc.chain[0].hash);

    for (int i = 0; i < n; i++) {
        WorkerArgs *args = new WorkerArgs();
        *(args) = { network[i], bc.chain.size(), &proposed };
        worker_args.push_back(args);
        pthread_create(&threads[i], nullptr, worker, args);
    }
    
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], nullptr);
    }

    int numstopped = 0;
    for (int i = 0; i < n; i++) {
        if (network[i]->timer == STOPPED) {
            numstopped++;
        }
    }
    std::cout << "num_stopped=" << numstopped << std::endl;

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
