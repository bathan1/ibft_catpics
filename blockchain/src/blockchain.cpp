#include "blockchain.h"
#include <cmath>
#include <iomanip>
#include <openssl/obj_mac.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <sstream>

Block::Block(): index(-1), previous_hash(""), hash("") {}

Block::Block(int idx, std::vector<Transaction> txs, std::string prev_hash): index(idx), transactions(txs), previous_hash(prev_hash) {
    timestamp = time(nullptr);
    hash = calculate_hash();
}

std::string Block::to_string() const {
    return std::to_string(index) + "|" + std::to_string(timestamp) + "|" + previous_hash;
}

std::string Block::calculate_hash() {
    std::stringstream ss;
    ss << to_string();

    for (const auto &tx : transactions) {
        ss << tx.to_string();
    }

    const std::string data = ss.str();
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);

    std::stringstream hexstr;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        hexstr << std::hex << std::setw(2) << std::setfill('0') << (int) hash[i];
    }

    return hexstr.str();
}

Blockchain::Blockchain(std::string initial_data) {
    Block genesis(0, {}, NULL);
}

void Blockchain::add_block(Block b) {
    chain.push_back(b);
}
