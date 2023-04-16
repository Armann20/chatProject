#include <iostream>
#include <sqlite3.h>

class ChatDatabase {
public:
    ChatDatabase(const std::string& db_file_path) : db_(nullptr), err_msg_(nullptr) {
        int rc = sqlite3_open(db_file_path.c_str(), &db_);
        if (rc != SQLITE_OK) {
            std::cerr << "Error opening database: " << sqlite3_errmsg(db_) << std::endl;
            sqlite3_close(db_);
            db_ = nullptr;
        }
    }

    ~ChatDatabase() {
        if (db_) {
            sqlite3_close(db_);
        }
        if (err_msg_) {
            sqlite3_free(err_msg_);
        }
    }

    bool createTable() {
        const char* create_query = "CREATE TABLE IF NOT EXISTS chat_messages("
                                   "message_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "message_text TEXT NOT NULL,"
                                   "sender_name TEXT NOT NULL,"
                                   "receiver_name TEXT NOT NULL,"
                                   "sent_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL)";
        int rc = sqlite3_exec(db_, create_query, nullptr, nullptr, &err_msg_);
        if (rc != SQLITE_OK) {
            std::cerr << "Error creating table: " << err_msg_ << std::endl;
            return false;
        }
        return true;
    }

    bool insertMessage(const std::string& message_text, const std::string& sender_name, const std::string& receiver_name) {
        std::string insert_query = "INSERT INTO chat_messages(message_text, sender_name, receiver_name) "
                                   "VALUES('" + message_text + "', '" + sender_name + "', '" + receiver_name + "')";
        int rc = sqlite3_exec(db_, insert_query.c_str(), nullptr, nullptr, &err_msg_);
        if (rc != SQLITE_OK) {
            std::cerr << "Error inserting message: " << err_msg_ << std::endl;
            return false;
        }
        return true;
    }

    bool viewMessages() {
        const char* view_query = "SELECT message_text, sender_name, receiver_name, sent_date FROM chat_messages";
        int rc = sqlite3_exec(db_, view_query, [](void* data, int argc, char** argv, char** col_name) -> int {
            std::cout << argv[0] << " from " << argv[1] << " to " << argv[2] << " at " << argv[3] << std::endl;
            return 0;
        }, nullptr, &err_msg_);
        if (rc != SQLITE_OK) {
            std::cerr << "Error querying messages: " << err_msg_ << std::endl;
            return false;
        }
        return true;
    }

private:
    sqlite3* db_;
    char* err_msg_;
};