 #include "net/common_headers.h"
 #include "net/connection.h"
 
 #include <boost/asio.hpp>

 using namespace boost::asio;
class Server
{
public:
    Server(int port)
    : _acceptor(_context, ip::tcp::endpoint(ip::tcp::v4(), port))
    {     
    }
    virtual ~Server()
    {}
public:
    void WaitForConnection()
    {
        _acceptor.async_accept(
        [this](std::error_code ec, ip::tcp::socket socket)
        {
            if(!ec)
            {
                std::cout << "New connection: " << socket.remote_endpoint() << std::endl;
                
            }
            else
            {
                std::cout << "Connection error: " << ec.message() << std::endl;
            }
            WaitForConnection();
        });
        
    }
    bool Start()
    {
        try
        {
            WaitForConnection();
            _context.run();

        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return false;
        }
        
        std::cout << "[Server] started!" << std::endl;
        return true;
    }
    void Stop()
    {
        _context.stop();
    }
private:
    io_context _context;
    ip::tcp::acceptor _acceptor;
};
 int main()
 {
    Server server(1234);
    server.Start();
    // while(true)
    // {
    //     server.Update();
    // }
 }