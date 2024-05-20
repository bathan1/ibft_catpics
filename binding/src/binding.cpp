#include "crypto_utils.h"
#include "napi.h"

#include "blockchain.h"
#include "node.h"
#include <iostream>
#include <pthread.h>
#include <unordered_map>

struct WorkerArgs {
    Node *node;
    size_t lambda;
    Block *value;
};

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

    for (int i = 0; i < n; i++) {
        Node *node = new Node(i, bc, n);
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
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "startSimulation"), Napi::Function::New(env, start_simulation));
    return exports;
}

NODE_API_MODULE(ibft, Init)
