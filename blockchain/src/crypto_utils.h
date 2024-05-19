#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <openssl/types.h>
#include <string>
#include <utility>

std::pair<EVP_PKEY *, EVP_PKEY *>generate_RSA_keypair();
std::string calculate_sha256(const std::string &data);
std::string sign_hash(const std::string &hash, EVP_PKEY *priv_key);
bool verify(const std::string &hash, const std::string &signature, EVP_PKEY *pub_key);

#endif
