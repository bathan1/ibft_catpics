#include "blockchain.h"
#include "message.h"

enum TimerState {
    RUNNING,
    STOPPED,
    EXPIRED
};

class Node {
public:
    int pi;
    int f;
    int n;
    unsigned int lambda_i;
    int ri, pr_i;
    Block pv_i;
    Block inputvalue_i;
    TimerState timer;
    std::chrono::steady_clock::time_point expiration_time;
    std::unordered_map<int, int> prepare_count;
    std::unordered_map<int, int> commit_count;
    EVP_PKEY *private_key;
    EVP_PKEY *public_key;
    std::unordered_map<int, RSA *> public_keys;
    int leaderpi;
    std::function<bool(const Block&)> beta;
    void decide(const Block &block);
    Blockchain blockchain;

    Node(std::string initial_data);
    bool verify_signature(const std::string &msg, const std::string &signature, int sender_id);
    int leader(int lambda, int r_i);
    void start(int lambda, Block value);
    void broadcast(Message msg);
    void receive(Message msg);
    void set_timer();
    bool has_quorum(int ri, MessageType msgtyp);
    bool justify_round_change();
    bool justify_preprepare(Message &msg);
    bool validate_message(Message &msg);
    std::chrono::steady_clock::time_point t(int ri);
};
