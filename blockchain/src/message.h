#ifndef MESSAGE_H
#define MESSAGE_H

#include "blockchain.h"
#include <openssl/types.h>
#include <string>

enum MessageType {
    ROUND_CHANGE,
    PRE_PREPARE,
    PREPARE,
    COMMIT
};

class Message {
public:
    MessageType type;
    int consensus_number;
    int round;
    Block value;
    int prepared_round;
    Block prepared_value;
    int sender; // The process id of the node that sent the message
    std::string signature; // The signature of that sender using their private key

    Message(MessageType typ, int lambda, int r, Block val, int pi);
    Message(MessageType typ, int lambda, int r, int pr, Block pv, int pi);
    std::string to_string() const;
    void sign(std::string hash, EVP_PKEY *priv_key);
private:
};

#endif
