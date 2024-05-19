#include "blockchain.h"
#include "message.h"
#include <queue>
#include <set>
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

    Node(int pid, const Blockchain &bc, int max_faulty);
    Node(int pid, const Blockchain &bc, int max_faulty, int num_nodes);
    ~Node();
    void broadcast(const Message &msg) const;
    int receive(const Message &msg);
    void run();
    void sign_message(Message &msg) const;
    bool verify_message(const Message &msg) const;

    int f;
    int n;
    unsigned int lambda_i;
    int ri, pr_i;
    Block pv_i;
    Block inputvalue_i;
    TimerState timer;
    std::chrono::steady_clock::time_point expiration_time;
    void decide(const Block &block);

    int leader(int lambda, int r_i);
    void start(int lambda, Block value);
    void set_timer();
    bool has_quorum(int ri, MessageType msgtyp);
    bool justify_round_change();
    bool validate_message(const Message &msg);
    std::chrono::steady_clock::time_point t(int ri);
    std::unordered_map<int, int> round_stage;

private:
    std::unordered_map<int, std::vector<Message>> valid_prepare_msgs;
    std::unordered_map<int, int> valid_commit_count;
    std::unordered_map<int, std::vector<Message>> valid_roundchange_msgs;
    std::string compute_hash(const std::string &data) const;
    bool verify_signature(const std::string &hash, const std::string &signature, int sender_id) const;
    bool justify_preprepare(const Message &msg) const;
};
