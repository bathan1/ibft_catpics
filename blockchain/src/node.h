#include "blockchain.h"
#include "message.h"
#include <queue>
#include <unordered_map>
#include <vector>

enum TimerState {
    RUNNING,
    STOPPED,
    EXPIRED
};

class Node {
public:
    int pi;
    Blockchain blockchain;
    std::vector<Node *> network;
    EVP_PKEY *private_key;
    std::unordered_map<int, EVP_PKEY *> public_keys;
    std::queue<Message> message_queue;
    mutable pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

    Node(int pid, const Blockchain &bc);
    void broadcast(const Message &msg) const;
    void receive(const Message &msg);
    void run(); 

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
    int leaderpi;
    std::function<bool(const Block&)> beta;
    void decide(const Block &block);

    std::string compute_hash(const std::string &data) const;
    bool verify_signature(const std::string &hash, const std::string &signature, int sender_id);
    int leader(int lambda, int r_i);
    void start(int lambda, Block value);
    void set_timer();
    bool has_quorum(int ri, MessageType msgtyp);
    bool justify_round_change();
    bool justify_preprepare(Message &msg);
    bool validate_message(Message &msg);
    std::chrono::steady_clock::time_point t(int ri);
};
