#include "message.h"
#include "crypto_utils.h"
#include "blockchain.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <sstream>

// Doesn't sign the message; must do so explicitly
Message::Message(MessageType typ, int cn, int rn, Block &val, int pi):
    type(typ), lambda_i(cn), 
    ri(rn), value(val), sender(pi), signature(""), pr_j(-1), pv_j(-1) {}

// Message serialization to basic string
std::string Message::to_string() const {
    std::stringstream ss;
    ss << type << "|" << lambda_i << "|" << ri << "|" << value.to_string() << "|" << pr_j << "|" << pv_j << "|" << sender;
    return ss.str();
}

// Signs the message with private key
void Message::sign(std::string hash, EVP_PKEY *priv_key) {
    this->signature = sign_hash(hash, priv_key);
}

