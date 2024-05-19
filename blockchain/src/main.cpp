#include "crypto_utils.h"
#include "blockchain.h"
#include "message.h"
#include <iostream>
#include <openssl/evp.h>
#include <string>
#include <unordered_map>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage:" << " " << argv[0] << " " << "<number_of_nodes>" << std::endl;
        return 1;
    }
    int n = std::atoi(argv[1]);
    std::unordered_map<int, EVP_PKEY *> public_keys;
    std::unordered_map<int, EVP_PKEY *> private_keys;
    for (int i = 0; i < n; i++) {
        auto [pubkey, privkey] = generate_RSA_keypair();
        public_keys[i] = pubkey;
        private_keys[i] = privkey;
    }

    // Simulating node 0 being the leader and proposing a block with a certain hash
    const int signer_index = 0;
    EVP_PKEY *node0_privkey = private_keys[0];

    Block random_block(0, {}, "NULL");
    std::string block_serialized = random_block.to_string();
    std::string node0_block_hash = calculate_sha256(block_serialized);
    random_block.assign_hash(node0_block_hash);
    Message msg(PRE_PREPARE, 1, 1, random_block, 0);
    std::string msg_serialized = msg.to_string();
    std::string node0_msg_hash = calculate_sha256(msg_serialized);
    msg.sign(node0_msg_hash, node0_privkey);

    int num_verified = 0;
    for (int i = 1; i < n; i++) {
        // Simulating a node serializing the message to compute its hash
        std::string serialized = msg.to_string();
        std::string computed_msg_hash = calculate_sha256(serialized);

        // Each node looks up the sender's pseudonym
        int sender = msg.sender;
        // then finds the corresponding public key 
        EVP_PKEY *sender_pubkey = public_keys[sender];

        bool result = verify_signature(computed_msg_hash, msg.signature, sender_pubkey);
        if (result) {
            std::cout << "Node " << i << " successfully verified the block!" << std::endl;
            num_verified++;
        } else {
            std::cout << "Node " << i << " could not verify the block" << std::endl;
        }
    }
    std::cout << "num_verified=" << num_verified << std::endl;

    // cleanup
    for (auto &pair : public_keys)
        EVP_PKEY_free(pair.second);
    for (auto &pair : private_keys)
        EVP_PKEY_free(pair.second);
    return 0;
}
