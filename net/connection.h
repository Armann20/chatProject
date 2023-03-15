#include "common_headers.h"
using namespace boost::asio;
class Connection
{
public:
void ConnectToServer(const ip::tcp::resolver::results_type& endpoint)
    {
        async_connect(_socket, endpoint, [this](std::error_code ec, size_t lenth){});
    }
private:
    ip::tcp::socket _socket;
};
