#include "message.h"
#include "crypto_utils.h"
#include "blockchain.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <sstream>

Message::Message(MessageType typ, 
        int cn, int rn, Block &val, 
        int pi, RSA *priv_key): 
        type(typ), lambda_i(cn), 
        ri(rn), value(val), sender(pi) {
    
}

std::string Message::to_string() const {
    std::stringstream ss;
    ss << type << "|" << lambda_i << "|" << ri << "|" << value.to_string() << "|" << pr_j << "|" << pv_j << "|" << sender;
    return ss.str();
}

std::string Message::sign() {
    const std::string hash = calculate_sha256(to_string());
}

