 #include "net/common_headers.h"
 #include "net/connection.h"
 
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
    {}
public:

    void Connect(const std::string& host, const int port) {
        ip::tcp::resolver resolver(_context);
        auto endpoint = resolver.resolve(host, std::to_string(port));
        async_connect(_socket, endpoint,
            [this](std::error_code ec, ip::tcp::endpoint ep)
            {
                if(!ec)
                {
                    while(true)
                    {
                        
                    }
                }                    
            });
    }
    void Disconnect()
    {}
    bool IsConnected()
    {
        return false;
    }

private:

    io_context _context;

    ip::tcp::socket _socket;

};
 int main()
 {
    Client client;
    client.Connect("localhost", 1234);
}
