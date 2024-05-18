#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <functional>
#include <openssl/types.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include "transaction.h"

class Block {
public:
    int index;
    long timestamp;
    std::vector<Transaction> transactions;
    std::string previous_hash;
    std::string hash;

    std::string to_string() const;
    Block();
    Block(int idx, std::vector<Transaction> txs, std::string prev_hash);
    std::string calculate_hash();

};

class Blockchain {
public:
    std::vector<Block> chain;
    std::vector<Transaction> current_transactions;

    Blockchain(std::string initial_data);
    void add_block(Block b);
};

#endif
