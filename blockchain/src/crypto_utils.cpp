#include "crypto_utils.h"
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/types.h>
#include <sstream>
#include <iomanip>
#include <vector>

std::string calculate_sha256(const std::string &data) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (ctx == nullptr)
        return "";

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    if (EVP_DigestUpdate(ctx, data.c_str(), data.size()) != 1) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;

    if (EVP_DigestFinal_ex(ctx, hash, &len) != 1) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    EVP_MD_CTX_free(ctx);

    std::stringstream hexstr;
    for (unsigned int i = 0; i < len; i++)
        hexstr << std::hex << std::setw(2) << std::setfill('0') << (int) hash[i];
    return hexstr.str();
}

std::string sign(const std::string &hash, EVP_PKEY *priv_key) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (ctx == nullptr)
        return "";

    if (EVP_DigestSignInit(ctx, nullptr, EVP_sha256(), nullptr, priv_key) <= 0) {
        EVP_MD_CTX_free(ctx);
        return "";
    }
    
    size_t siglen = 0;
    const unsigned char *hash_cstr = reinterpret_cast<const unsigned char *>(hash.c_str());
    if (EVP_DigestSign(ctx, nullptr, &siglen, hash_cstr, hash.size()) <= 0) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    std::vector<unsigned char> sig(siglen);
    if (EVP_DigestSign(ctx, sig.data(), &siglen, hash_cstr, hash.size()) <= 0) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    EVP_MD_CTX_free(ctx);
    return std::string(reinterpret_cast<char *>(sig.data()), siglen);
}

std::pair<EVP_PKEY *, EVP_PKEY *> generate_RSA_keypair() {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (ctx == nullptr)
        return {nullptr, nullptr};
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return {nullptr, nullptr};
    }
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return {nullptr, nullptr};
    }

    EVP_PKEY *pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return {nullptr, nullptr};
    }

    EVP_PKEY_CTX_free(ctx);

    BIO *pubbio = BIO_new(BIO_s_mem());
    if (!PEM_write_bio_PUBKEY(pubbio, pkey)) {
        EVP_PKEY_free(pkey);
        BIO_free(pubbio);
        return {nullptr, nullptr};
    }
    EVP_PKEY *pubkey = PEM_read_bio_PUBKEY(pubbio, nullptr, nullptr, nullptr);
    if (pubkey == nullptr) {
        EVP_PKEY_free(pkey);
        BIO_free(pubbio);
        return {nullptr, nullptr};
    }
    BIO_free(pubbio);
    return {pubkey, pkey};
}

bool verify(const std::string &hash, const std::string &signature, EVP_PKEY *pub_key) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (ctx == nullptr)
        return false;

    if (EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, pub_key) <= 0) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    if (EVP_DigestVerifyUpdate(ctx, hash.c_str(), hash.size()) <= 0) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    const unsigned char *sig_cstr = reinterpret_cast<const unsigned char *>(signature.c_str());
    bool result = EVP_DigestVerifyFinal(ctx, sig_cstr, signature.size()) == 1;
    EVP_MD_CTX_free(ctx);
    return result;
}
