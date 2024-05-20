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
    Node(int pid, const Blockchain &bc, int num_nodes);
    ~Node();

    Blockchain blockchain;
    std::vector<Node *> network;
    EVP_PKEY *private_key;
    std::unordered_map<int, EVP_PKEY *> public_keys;
    std::queue<Message> message_queue;
    mutable pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

    int pi;
    int n;
    int f;
    int lambda;
    int r; 
    int pr;
    Block pv;
    bool faulty;
    Block input_value;
    int t0;
    TimerState timer;

    void broadcast(const Message &msg) const;
    int receive(const Message &msg);
    void run();
    void sign_message(Message &msg) const;
    bool verify_message(const Message &msg) const;
    void decide(const Block &block);
    int leader(int lambda, int r_i);
    void start(int lambda, Block value);
    void set_timer(TimerState state, int ri);
    bool check_expired();
    bool justify_round_change();
    bool validate_message(const Message &msg);
    bool verify_transaction(const Transaction &tx) const;
    std::unordered_map<int, int> round_stage;

private:
    int leader();
    void handle_timeout();
    void set_expiration(int ri);
    std::chrono::seconds t(int ri) const;
    std::chrono::steady_clock::time_point expiration_time;
    std::unordered_map<int, std::vector<Message>> valid_prepare_msgs;
    std::unordered_map<int, int> valid_commit_count;
    std::unordered_map<int, std::vector<Message>> valid_roundchange_msgs;
    std::unordered_map<int, int> valid_roundchange_count;
    std::string compute_hash(const std::string &data) const;
    bool has_quorum(int ri, MessageType msgtyp);
    int check_skip_round() const;
    void handle_skip_round(int r_min);
    bool verify_signature(const std::string &hash, const std::string &signature, int sender_id) const;
    bool justify_preprepare(const Message &msg) const;
};
