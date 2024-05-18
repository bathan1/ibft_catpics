#include "crypto_utils.h"
#include <iostream>
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

    const int signer_index = 0;
    std::string data = "lol";
    std::string hash = calculate_sha256(data);
    std::cout << "hash=" << hash << std::endl;
    std::string sig = sign(hash, private_keys[signer_index]);
    std::cout << "sig with hash=" << sig << std::endl;

    int num_verified = 0;
    for (int i = 1; i < n; i++) {
        bool result;
        if (i == 3) {
            // Bad verification
            result = verify(hash, sig, public_keys[3]); 
        } else {
            result = verify(hash, sig, public_keys[0]); 
        }
        if (result) {
            std::cout << "Node " << i << " successfully verified the signature!" << std::endl;
            num_verified++;
        } else {
            std::cout << "Node " << i << " was not able to verify the signature!" << std::endl;
        }
    }

    std::cout << "num_verified=" << num_verified << std::endl;
    return 0;
}
