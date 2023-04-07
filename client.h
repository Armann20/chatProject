#include "common_headers.h"
using boost::asio::ip::tcp;

class Client
{
public:
    Client(const std::array<char, MAX_NICKNAME_SIZE>& nickname,
            boost::asio::io_context& io_context,
            tcp::resolver::iterator endpoint_iterator);
    void write(const std::array<char, MAX_PACK_SIZE>& msg);
    void close();

private:
    void onConnect(const boost::system::error_code& error);
    void readHandler(const boost::system::error_code& error);
    void writeImpl(std::array<char, MAX_PACK_SIZE> msg);
    void writeHandler(const boost::system::error_code& error);
    void closeImpl();

private:
    boost::asio::io_context& io_context_;
    tcp::socket socket_;
    std::array<char, MAX_PACK_SIZE> read_msg_;
    std::deque<std::array<char, MAX_PACK_SIZE>> write_msgs_;
    std::array<char, MAX_NICKNAME_SIZE> nickname_;
};