#include "common_headers.h"
#include "client.h"
using boost::asio::ip::tcp;
using namespace boost::asio;

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 4)
        {
            std::cerr << "Usage: client <nickname> <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::query query(argv[2], argv[3]);
        tcp::resolver::iterator iterator = resolver.resolve(query);
        std::array<char, MAX_NICKNAME_SIZE> nickname;
        strcpy(nickname.data(), argv[1]);

        Client client(nickname, io_context, iterator);

        std::thread mainThread([&io_context](){io_context.run();});

        std::array<char, MAX_PACK_SIZE> msg;

        while (true)
        {
            memset(msg.data(), '\0', msg.size());
            if (!std::cin.getline(msg.data(), MAX_PACK_SIZE - PADDING - MAX_NICKNAME_SIZE))
            {
                std::cin.clear();
            }
            client.write(msg);
        }

        client.close();
        mainThread.join();
    } catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}


Client::Client(const std::array<char, MAX_NICKNAME_SIZE>& nickname,
            boost::asio::io_context& io_context,
            tcp::resolver::iterator endpoint_iterator) :
            io_context_(io_context), socket_(io_context)
{
    strcpy(nickname_.data(), nickname.data());
    memset(read_msg_.data(), '\0', MAX_PACK_SIZE);
    boost::asio::async_connect(socket_, endpoint_iterator, [this](const boost::system::error_code& error, const ip::tcp::resolver::iterator& endpoint_iterator){
        onConnect(error);
    });
    
}

void Client::write(const std::array<char, MAX_PACK_SIZE>& msg)
{
    io_context_.post([msg, this](){writeImpl(msg);});
}

void Client::close()
{
    io_context_.post([this](){ closeImpl();});
}
void Client::onConnect(const boost::system::error_code& error)
{
    if (!error)
    {
        boost::asio::async_write(socket_,
                                    boost::asio::buffer(nickname_, nickname_.size()),
                                    [this](const boost::system::error_code& error, std::size_t bytes){ readHandler(error);});
    }
}
void Client::readHandler(const boost::system::error_code& error)
{
    std::cout << read_msg_.data() << std::endl;
    if (!error)
    {
        boost::asio::async_read(socket_,
                                boost::asio::buffer(read_msg_, read_msg_.size()),
                                [this](const boost::system::error_code& error, std::size_t bytes){ readHandler(error);});
    } else
    {
        closeImpl();
    }
}

void Client::writeImpl(std::array<char, MAX_PACK_SIZE> msg)
{
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
        boost::asio::async_write(socket_,
                                    boost::asio::buffer(write_msgs_.front(), write_msgs_.front().size()),
                                    [this](const boost::system::error_code& error, std::size_t bytes){ writeHandler(error);});
    }
}

void Client::writeHandler(const boost::system::error_code& error)
{
    if (!error)
    {
        write_msgs_.pop_front();
        if (!write_msgs_.empty())
        {
            boost::asio::async_write(socket_,
                                        boost::asio::buffer(write_msgs_.front(), write_msgs_.front().size()),
                                        [this](const boost::system::error_code& error, std::size_t bytes){ writeHandler(error);});
        }
    } else
    {
        closeImpl();
    }
}

void Client::closeImpl()
{
    socket_.close();
}
