#include "common_headers.h"
using namespace boost::asio;

class Connection
{
public:
    Connection(io_context& context, ip::tcp::socket socket)
    : _context(context), _socket(std::move(socket))
    {}
    virtual ~Connection() = default;

public:
    bool ConnectToServer(const ip::tcp::resolver::results_type& endpoint)
    {
        async_connect(_socket, endpoint, 
        [this](std::error_code ec, ip::tcp::endpoint ep)
        {
            if(!ec)
            {

            }
            
        });
        return false;
    }
private:
    io_context& _context;
    ip::tcp::socket _socket;
};
