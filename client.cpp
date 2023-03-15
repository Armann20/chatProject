#include "net/common_headers.h"
#include "net/connection.h"

using namespace boost::asio;

class Client
{
public:
	Client()
    : _socket(_context)
    {
    }

    virtual ~Client() = default;
public:

    void Connect(const std::string& host, const int port) {
        ip::tcp::resolver resolver(_context);
        auto endpoint = resolver.resolve(host, std::to_string(port));
        auto handler = [this](std::error_code ec, ip::tcp::endpoint& ep) // <-- define lambda function
        {
            if (!ec) {

            }
        };
        async_connect(_socket, endpoint,handler);
    }
    void Disconnect()
    {}
    bool IsConnected()
    {
        return false;
    }

private:

    io_context _context;
    std::thread _thread;
    ip::tcp::socket _socket;

};
int main()
{
    Client client;
    client.Connect("localhost", 8001);
}
