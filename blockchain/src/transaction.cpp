#include "transaction.h"
#include <openssl/bio.h>
#include <openssl/types.h>
#include <openssl/pem.h>
#include <iomanip>
#include <sstream>

Transaction::Transaction(std::string from, std::string to, double amt, std::string data): sender(from), recipient(to), amount(amt), payload(data), verified(false) { }

void Transaction::sign(std::string priv_key) {
    std::string transaction_data = this->to_string();
    unsigned char tx_hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, transaction_data.c_str(), transaction_data.size());
    SHA256_Final(tx_hash, &sha256);

    EVP_PKEY *pkey;
    BIO *bio = BIO_new_mem_buf(priv_key.data(), priv_key.size());
    pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
    BIO_free(bio);

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    EVP_SignInit(mdctx, EVP_sha256());
    unsigned char sig[256];
    unsigned int sig_len;
    EVP_SignUpdate(mdctx, tx_hash, SHA256_DIGEST_LENGTH);
    EVP_SignFinal(mdctx, sig, &sig_len, pkey);
    
    std::stringstream ss;
    for (unsigned int i = 0; i < sig_len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int) sig[i];
    }
    this->signature = ss.str();

    EVP_MD_CTX_free(mdctx);
}

std::string Transaction::to_string() const {
    std::stringstream ss;
    ss << "From: " << this->sender
       << ", To: " << this->recipient
       << ", Amount: " << this->amount
       << ", Timestamp: " << timestamp
       << ", Data: " << payload;
    return ss.str();
}

