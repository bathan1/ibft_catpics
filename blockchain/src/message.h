#ifndef MESSAGE_H
#define MESSAGE_H

#include <openssl/types.h>
#include <string>

class Block;

enum MessageType {
    ROUND_CHANGE,
    PRE_PREPARE,
    PREPARE,
    COMMIT
};

class Message {
public:
    MessageType type;
    int lambda_i;
    int ri;
    Block &value;
    int pr_j;
    int pv_j;
    int sender; // The process id of the node that sent the message
    std::string signature; // The signature of that sender using their private key

    Message(MessageType typ, int cn, int rn, Block &val, int pi);
    std::string to_string() const;
    void sign(std::string hash, EVP_PKEY *priv_key);
private:
};

#endif
