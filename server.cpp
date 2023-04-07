#include "common_headers.h"
#include "timestamp.h"
#include "server.h"
#include "worker_thread.h"
#include "room_participants.h"

using boost::asio::ip::tcp;
using namespace boost::asio;
Server::Server(boost::asio::io_context& io_context,
           boost::asio::io_context::strand& strand,
           const tcp::endpoint& endpoint)
: io_context_(io_context), strand_(strand), acceptor_(io_context, endpoint)
{
    run();
}

void Server::run()
{
    std::shared_ptr<personInRoom> new_participant(new personInRoom(io_context_, strand_, room_));
    acceptor_.async_accept(new_participant->socket(), strand_.wrap([this, new_participant](const boost::system::error_code& error){
        onAccept(new_participant, error);
    }));
}

void Server::onAccept(std::shared_ptr<personInRoom> new_participant, const boost::system::error_code& error)
{
    if (!error)
    {
        new_participant->start();
    }

    run();
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: chat_server <port> [<port> ...]\n";
            return 1;
        }

        std::shared_ptr<boost::asio::io_context> io_context(new boost::asio::io_context);
        boost::shared_ptr<boost::asio::io_context::work> work(new boost::asio::io_context::work(*io_context));
        boost::shared_ptr<boost::asio::io_context::strand> strand(new boost::asio::io_context::strand(*io_context));

        std::cout << "[" << std::this_thread::get_id() << "]" << "server starts" << std::endl;

        std::list < std::shared_ptr < Server >> servers;
        for (int i = 1; i < argc; ++i)
        {
            tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
            std::shared_ptr<Server> a_server(new Server(*io_context, *strand, endpoint));
            servers.push_back(a_server);
        }

        boost::thread_group workers;
        for (int i = 0; i < 1; ++i)
        {
            boost::thread * t = new boost::thread{ &workerThread::run, io_context};
            workers.add_thread(t);
        }

        workers.join_all();
    } catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
