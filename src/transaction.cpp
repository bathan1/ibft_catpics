#include "transaction.h"
#include "crypto_utils.h"
#include <sstream>

Transaction::Transaction(int from, int to, double amt, std::string data): sender(from), recipient(to), amount(amt), payload(data), timestamp(time(nullptr)), verified(false) { }

void Transaction::sign(EVP_PKEY *priv_key) {
    std::string serialized = to_string();
    std::string hash = calculate_sha256(serialized);
    this->signature = sign_hash(hash, priv_key);
}

std::string Transaction::to_string() const {
    std::stringstream ss;
    ss << "From: " << sender
       << ", To: " << recipient
       << ", Amount: " << amount
       << ", Timestamp: " << timestamp
       << ", Data: " << payload;
    return ss.str();
}

