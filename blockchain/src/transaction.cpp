#include "transaction.h"
#include "crypto_utils.h"
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <iomanip>
#include <sstream>

Transaction::Transaction(int from, int to, double amt, std::string data): sender(from), recipient(to), amount(amt), payload(data), verified(false) { 
}

void Transaction::sign(EVP_PKEY *priv_key) {
    std::string serialized = to_string();
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

