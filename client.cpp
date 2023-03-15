 #include "net/common_headers.h"
 #include "net/connection.h"
 #include "net/thread_safe_queue.h"
 #include <boost/asio.hpp>

 using namespace boost::asio;

 class Client
{
public:
	Client()
    : _socket(_context)
    {
    }

    virtual ~Client()
    {
        Disconnect();
    }
public:

    bool Connect(const std::string& host, const int port) {
        try
        {
            ip::tcp::resolver resolver(_context);
            auto endpoint = resolver.resolve(host, std::to_string(port));
            _connection = std::make_unique<Connection>(_context, ip::tcp::socket(_context));
            _connection -> ConnectToServer(endpoint);
            _context.run();
        }   
        catch(std::exception& e)
        {
            std::cerr << "Client Exception: " << e.what() << "\n";
        }
        return true;
    }
    void Disconnect()
    {
        if(IsConnected())
        {
            _context.stop();
        }
    }
    bool IsConnected()
    {
        if(_socket.is_open())
            return true;
        return false;
    }

private:

    io_context _context;

    ip::tcp::socket _socket;
    std::unique_ptr<Connection> _connection;
};
 int main()
 {
    Client client;
    client.Connect("localhost", 1234);
}
