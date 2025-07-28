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
    /**
     * Message enum
     */
    MessageType type;
    int consensus_number;
    int round;
    Block value;
    int prepared_round;
    Block prepared_value;
    /**
     * Process ID of outgoing node
     */
    int sender;
    /**
     * UTF8 encoded signature
     */
    std::string signature;

    Message(MessageType typ, int lambda, int r, Block val, int pi);
    Message(MessageType typ, int lambda, int r, int pr, Block pv, int pi);
    std::string to_string() const;
    void sign(std::string hash, EVP_PKEY *priv_key);
private:
};

#endif
