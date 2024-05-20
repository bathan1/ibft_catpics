#include "crypto_utils.h"
#include "napi.h"

#include "blockchain.h"
#include "node.h"
#include <iostream>
#include <openssl/evp.h>
#include <pthread.h>
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
    node->input_value = *(args->value);
    node->run();
    return nullptr;
}

void start_simulation(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() < 3) {
        Napi::TypeError::New(env, "Three arguments expected").ThrowAsJavaScriptException();
        return;
    }
    if (!info[0].IsNumber() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Number arguments for arg1 and arg2 expected").ThrowAsJavaScriptException();
        return;
    }

    if (!info[2].IsString()) {
        Napi::TypeError::New(env, "String argument for arg3 expected").ThrowAsJavaScriptException();
        return;
    }

    std::string initial_data = info[2].As<Napi::String>();
    Blockchain bc(initial_data);

    int n = info[0].As<Napi::Number>().Int64Value();
    int f = (n - 1) / 3;
    int actual_faulty = info[1].As<Napi::Number>().Int64Value();
    if (actual_faulty > f) {
        Napi::Error::New(env, "Can't assign actual_faulty > f").ThrowAsJavaScriptException();
        return;
    }
    std::unordered_map<int, EVP_PKEY *> public_keys;
    std::vector<Node *> network;
    std::vector<pthread_t> threads(n);
    std::vector<WorkerArgs *> worker_args(n);

    Log log;
    for (int i = 0; i < n; i++) {
        Node *node = new Node(i, bc, n, &log);
        network.push_back(node);
        auto [pubkey, privkey] = generate_RSA_keypair();
        public_keys[i] = pubkey;
        node->private_key = privkey;
    }

    for (int i = 0; i < actual_faulty; i++) {
        int random = rand() % network.size();
        while (network[random]->faulty)
            random = rand() % network.size();
        network[random]->faulty = true;
    }

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
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "startSimulation"), Napi::Function::New(env, start_simulation));
    return exports;
}

NODE_API_MODULE(ibft, Init)
