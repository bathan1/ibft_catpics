#include <string>
#include <vector>

class Transaction {
public:
    std::string sender;
    std::string recipient;
    double amount;
    long timestamp;
    std::string signature;
    std::string payload;
    bool verified;

    Transaction(std::string from, std::string to, double amt, std::string data);
    void sign(std::string priv_key);
    std::string to_string() const;
};
