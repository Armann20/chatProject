#include <fstream>
#include <sstream>
#include <vector>

class ChatLog {
public:
    // Constructor to create/open the chat log file
    ChatLog(const std::string& filename) {
        outfile_.open(filename, std::ios::app);
    }

    // Destructor to close the chat log file
    ~ChatLog() {
        outfile_.close();
    }

    // Method to add a message to the chat log
    void addMessage(const std::string& message, const std::string& timestamp, const std::string& sender, const std::string& recipient) {
        // Construct a CSV record with the message and attributes
        std::ostringstream record;
        record << "\"" << timestamp << "\",\"" << sender << "\",\"" << recipient << "\",\"" << message << "\"";

        // Write the record to the file
        outfile_ << record.str() << std::endl;

        // Add the record to the in-memory message vector
        messages_.emplace_back(timestamp, sender, recipient, message);
    }

    // Method to get the in-memory message vector
    const std::vector<std::tuple<std::string, std::string, std::string, std::string>>& getMessages() const {
        return messages_;
    }

private:
    std::ofstream outfile_;
    std::string filename_;
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> messages_;
};