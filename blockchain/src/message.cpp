#include "message.h"
#include "crypto_utils.h"
#include "blockchain.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <sstream>

// Doesn't sign the message; must do so explicitly
Message::Message(MessageType typ, int lambda, int rn, Block val, int pi):
    type(typ), consensus_number(lambda),
    round(rn), value(val), sender(pi), signature(""), prepared_round(-1), prepared_value(Block()) {}

Message::Message(MessageType typ, int lambda, int r, int pr, Block pv, int pi):
    type(typ), consensus_number(lambda),
    round(r), value(Block()), prepared_round(pr),
    prepared_value(pv), sender(pi), signature("") {}

// Message serialization to basic string
std::string Message::to_string() const {
    std::stringstream ss;
    ss << type << "|" << consensus_number << "|" << round << "|" << value.to_string() << "|" << prepared_round << "|" << prepared_value.to_string() << "|" << sender;
    return ss.str();
}

// Signs the message with private key
void Message::sign(std::string hash, EVP_PKEY *priv_key) {
    this->signature = sign_hash(hash, priv_key);
}

