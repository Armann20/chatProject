#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;
using std::string;
using std::string_view;

constexpr ip::port_type PortNumber{1234};
constexpr string_view Delimiter{"\n"};

string read_(tcp::socket &socket)
{
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, Delimiter);
    string data = boost::asio::buffer_cast<const char *>(buf.data());
    return data;
}
void send_(tcp::socket &socket, const string &message)
{
    const string msg = message + std::string(Delimiter);
    boost::asio::write(socket, boost::asio::buffer(message));
}

int main()
{
    boost::asio::io_service io_service;
    // listen for new connection
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), PortNumber));
    // socket creation
    tcp::socket socket_(io_service);
    // waiting for connection
    acceptor_.accept(socket_);
    // read operation
    string message = read_(socket_);
    cout << message << endl;
    // write operation
    send_(socket_, "Hello From Server!");
    cout << "Servent sent Hello message to Client!" << endl;
    return 0;
}
