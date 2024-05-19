#include <openssl/types.h>
#include <string>
#include <vector>

class Transaction {
public:
    int sender;
    int recipient;
    double amount;
    long timestamp;
    std::string hash;
    std::string signature;
    std::string payload;
    bool verified;

    Transaction(int from, int to, double amt, std::string data);
    void sign(EVP_PKEY *priv_key);
    std::string to_string() const;
};
