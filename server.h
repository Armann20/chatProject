#ifndef SERVER_H
#define SERVER_H

#include "common_headers.h"
#include "timestamp.h"
#include "room_participants.h"

using boost::asio::ip::tcp;
class Server
{
public:
    Server(boost::asio::io_context& io_context,
           boost::asio::io_context::strand& strand,
           const tcp::endpoint& endpoint);
    void run();
    void onAccept(std::shared_ptr<personInRoom> new_participant, const boost::system::error_code& error);


private:
    boost::asio::io_context& io_context_;
    boost::asio::io_context::strand& strand_;
    tcp::acceptor acceptor_;
    chatRoom room_;
};

#endif