#include "blockchain.h"
#include <cmath>
#include <openssl/obj_mac.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <sstream>

Block::Block(): index(-1), previous_hash(""), hash("") {}

Block::Block(int idx, std::vector<Transaction> txs, std::string prev_hash): index(idx), transactions(txs), previous_hash(prev_hash) {
    timestamp = time(nullptr);
}

std::string Block::to_string() const {
    std::stringstream ss;
    ss << std::to_string(index) << "|" << std::to_string(timestamp) << "|" + previous_hash + "|";
    for (const auto &tx : transactions)
        ss << tx.to_string();
    return ss.str();
}

void Block::assign_hash(std::string hash) {
    this->hash = hash;
}

Blockchain::Blockchain(std::string initial_data) {
    Block genesis(0, {}, NULL);
}

void Blockchain::add_block(Block b) {
    chain.push_back(b);
}
