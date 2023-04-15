#include <iostream>
#include <sqlite3.h>

int main() {
    sqlite3* db;
    char* err_msg = nullptr;

    // open or create a database
    int rc = sqlite3_open("chat_messages.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    // create a table for chat messages
    const char* create_query = "CREATE TABLE IF NOT EXISTS chat_messages("
                               "message_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "message_text TEXT NOT NULL,"
                               "sender_name TEXT NOT NULL,"
                               "receiver_name TEXT NOT NULL,"
                               "sent_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL)";
    rc = sqlite3_exec(db, create_query, nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "Error creating table: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    // insert a chat message
    const char* insert_query = "INSERT INTO chat_messages(message_text, sender_name, receiver_name) "
                               "VALUES('Hello, how are you?', 'Alice', 'Bob')";
    rc = sqlite3_exec(db, insert_query, nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "Error inserting message: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    // query all chat messages
    const char* view_query = "SELECT message_text, sender_name, receiver_name, sent_date FROM chat_messages";
    rc = sqlite3_exec(db, view_query, [](void* data, int argc, char** argv, char** col_name) -> int {
        std::cout << argv[0] << " from " << argv[1] << " to " << argv[2] << " at " << argv[3] << std::endl;
        return 0;
    }, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "Error querying messages: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    // close the database connection
    sqlite3_close(db);
    return 0;
}