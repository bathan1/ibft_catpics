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
    node->listen();
    return nullptr;
}

Napi::Array start_simulation(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() < 3) {
        Napi::TypeError::New(env, "Three arguments expected").ThrowAsJavaScriptException();
        return {};
    }
    if (!info[0].IsNumber() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Number arguments for arg1 and arg2 expected").ThrowAsJavaScriptException();
        return {};
    }

    if (!info[2].IsString()) {
        Napi::TypeError::New(env, "String argument for arg3 expected").ThrowAsJavaScriptException();
        return {};
    }

    int n = info[0].As<Napi::Number>().Int64Value();
    int f = (n - 1) / 3;
    int actual_faulty = info[1].As<Napi::Number>().Int64Value();
    if (actual_faulty > f) {
        Napi::Error::New(env, "Can't assign actual_faulty > f").ThrowAsJavaScriptException();
        return {};
    }
    if (n > 10) {
        Napi::Error::New(env, "This simulation only supports 10 threads :(").ThrowAsJavaScriptException();
        return {};
    }
    std::cout << "n = " << n << std::endl;
    std::cout << "f = " << f << std::endl;
    std::cout << "actual_faulty = " << actual_faulty << std::endl;

    std::unordered_map<int, EVP_PKEY *> public_keys;
    std::vector<Node *> network;
    std::vector<pthread_t> threads(n);
    std::vector<WorkerArgs *> worker_args(n);

    // Setup the blockchain
    Blockchain bc("meow");
    Log log;
    for (int i = 0; i < n; i++) {
        Node *node = new Node(i, bc, n, &log);
        network.push_back(node);
        auto [pubkey, privkey] = generate_RSA_keypair();
        public_keys[i] = pubkey;
        node->private_key = privkey;
    }

    std::vector<Block> existing;
    // Pick 3 random nodes to make the transactions
    for (int i = 0; i < 3; i++) {
        int random = rand() % network.size();
        Transaction tx(random, random, 0, "meow!");
        tx.sign(network[i]->private_key);
        Block b(bc.chain.size(), {tx}, bc.chain[bc.chain.size() - 1].hash);
        std::string block_hash = calculate_sha256(b.to_string());
        b.assign_hash(block_hash);
        existing.push_back(b);
    }
    for (const auto &block : existing) {
        bc.add_block(block);
    }
    for (size_t i = 0; i < network.size(); i++) {
        network[i]->blockchain = bc;
    }

    std::cout << "Last hash is " << bc.chain[bc.chain.size() - 1].hash << std::endl;

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

    Node *node = network[0];
    std::string data = info[2].As<Napi::String>();
    Transaction t1(0, 0, 0, data);
    t1.sign(node->private_key);
    Block proposed(bc.chain.size(), {t1}, bc.chain[bc.chain.size() - 1].hash);

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
    for (size_t i = 0; i < worker_args.size(); i++)
        delete worker_args[i];
    for (auto &pair : public_keys)
        EVP_PKEY_free(pair.second);
    for (size_t i = 0; i < network.size(); i++) {
        EVP_PKEY_free(network[i]->private_key);
        delete network[i];
    }

    Napi::Array log_array = Napi::Array::New(env, log.entries.size());
    for (size_t i = 0; i < log.entries.size(); i++) {
        Napi::Object log_obj = Napi::Object::New(env);
        log_obj.Set("messageType", log.entries[i].message_type);
        log_obj.Set("round", log.entries[i].round);
        log_obj.Set("senderId", log.entries[i].sender_id);
        log_obj.Set("receiverId", log.entries[i].receiver_id);
        log_array[i] = log_obj;
    }
    return log_array;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "startSimulation"), Napi::Function::New(env, start_simulation));
    return exports;
}

NODE_API_MODULE(ibft, Init)
