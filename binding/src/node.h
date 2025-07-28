#include "blockchain.h"
#include "message.h"
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <pthread.h>

enum TimerState {
    RUNNING,
    STOPPED,
    EXPIRED
};

struct LogEntry {
    std::string message_type;
    int round;
    int sender_id;
    int receiver_id;
};

class Log {
public:
    Log() {
        if (pthread_mutex_init(&mutex, nullptr) != 0) {
            throw std::runtime_error("Failed to initialize mutex for the log");
        }
    }
    ~Log() {
        pthread_mutex_destroy(&mutex);
    }
    std::vector<LogEntry> entries;
    pthread_mutex_t mutex;
};

class Node {
public:
    Node(int pid, const Blockchain &bc, int num_nodes, Log *log);
    ~Node();

    Log *log;
    Blockchain blockchain;
    EVP_PKEY *private_key;
    std::vector<Node *> network;
    std::unordered_map<int, EVP_PKEY *> public_keys;
    std::queue<Message> message_queue;
    std::unordered_map<int, int> round_stage;

    mutable pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    mutable pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

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

    int leader();
    void broadcast(const Message &msg) const;
    int receive(const Message &msg);
    void listen();
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
    void log_entry(const Message &msg);

private:
    std::chrono::steady_clock::time_point expiration_time;
    std::unordered_map<int, std::vector<Message>> valid_prepare_msgs;
    std::unordered_map<int, int> valid_commit_count;
    std::unordered_map<int, std::vector<Message>> valid_roundchange_msgs;
    std::unordered_map<int, int> valid_roundchange_count;

    void handle_timeout();
    void set_expiration(int ri);
    std::chrono::seconds t(int ri) const;
    std::string compute_hash(const std::string &data) const;
    bool has_quorum(int ri, MessageType msgtyp);
    int check_skip_round() const;
    void handle_skip_round(int r_min);
    bool verify_signature(const std::string &hash, const std::string &signature, int sender_id) const;
    bool justify_preprepare(const Message &msg) const;
};
