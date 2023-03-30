#ifndef ROOM_PARTICIPANTS_H
#define ROOM_PARTICIPANTS_H
#include "common_headers.h"
#include "timestamp.h"
using boost::asio::ip::tcp;
class participant
{
public:
    virtual ~participant() = default;
    virtual void onMessage(std::array<char, MAX_IP_PACK_SIZE> & msg) = 0;
};
class chatRoom {
public:
    void enter(std::shared_ptr<participant> participant, const std::string & nickname)
    {
        participants_.insert(participant);
        name_table_[participant] = nickname;
        std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
                      boost::bind(&participant::onMessage, participant, _1));
    }

    void leave(std::shared_ptr<participant> participant)
    {
        participants_.erase(participant);
        name_table_.erase(participant);
    }

    void broadcast(std::array<char, MAX_IP_PACK_SIZE>& msg, std::shared_ptr<participant> participant)
    {
        std::string timestamp = getTimestamp();
        std::string nickname = getNickname(participant);
        std::array<char, MAX_IP_PACK_SIZE> formatted_msg;

        strcpy(formatted_msg.data(), timestamp.c_str());
        strcat(formatted_msg.data(), nickname.c_str());
        strcat(formatted_msg.data(), msg.data());

        recent_msgs_.push_back(formatted_msg);
        while (recent_msgs_.size() > max_recent_msgs)
        {
            recent_msgs_.pop_front();
        }

        std::for_each(participants_.begin(), participants_.end(),
                      boost::bind(&participant::onMessage, _1, std::ref(formatted_msg)));
    }

    std::string getNickname(std::shared_ptr<participant> participant)
    {
        return name_table_[participant];
    }

private:
    enum { max_recent_msgs = 100 };
    std::unordered_set<std::shared_ptr<participant>> participants_;
    std::unordered_map<std::shared_ptr<participant>, std::string> name_table_;
    std::deque<std::array<char, MAX_IP_PACK_SIZE>> recent_msgs_;
};
class personInRoom: public participant,
                    public std::enable_shared_from_this<personInRoom>
{
public:
    personInRoom(boost::asio::io_context& io_context,
                 boost::asio::io_context::strand& strand, chatRoom& room)
                 : socket_(io_context), strand_(strand), room_(room)
    {
    }

    tcp::socket& socket() { return socket_; }

    void start()
    {
        boost::asio::async_read(socket_,
                                boost::asio::buffer(nickname_, nickname_.size()),
                                strand_.wrap(boost::bind(&personInRoom::nicknameHandler, shared_from_this(), _1)));
    }

    void onMessage(std::array<char, MAX_IP_PACK_SIZE>& msg)
    {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            boost::asio::async_write(socket_,
                                     boost::asio::buffer(write_msgs_.front(), write_msgs_.front().size()),
                                     strand_.wrap(boost::bind(&personInRoom::writeHandler, shared_from_this(), _1)));
        }
    }

private:
    void nicknameHandler(const boost::system::error_code& error)
    {
        if (strlen(nickname_.data()) <= MAX_NICKNAME_SIZE - 2)
        {
            strcat(nickname_.data(), ": ");
        }
        else
        {
            //cut off nickname if too long
            nickname_[MAX_NICKNAME_SIZE - 2] = ':';
            nickname_[MAX_NICKNAME_SIZE - 1] = ' ';
        }

        room_.enter(shared_from_this(), std::string(nickname_.data()));

        boost::asio::async_read(socket_,
                                boost::asio::buffer(read_msg_, read_msg_.size()),
                                strand_.wrap(boost::bind(&personInRoom::readHandler, shared_from_this(), _1)));
    }

    void readHandler(const boost::system::error_code& error)
    {
        if (!error)
        {
            room_.broadcast(read_msg_, shared_from_this());

            boost::asio::async_read(socket_,
                                    boost::asio::buffer(read_msg_, read_msg_.size()),
                                    strand_.wrap(boost::bind(&personInRoom::readHandler, shared_from_this(), _1)));
        }
        else
        {
            room_.leave(shared_from_this());
        }
    }

    void writeHandler(const boost::system::error_code& error)
    {
        if (!error)
        {
            write_msgs_.pop_front();

            if (!write_msgs_.empty())
            {
                boost::asio::async_write(socket_,
                                         boost::asio::buffer(write_msgs_.front(), write_msgs_.front().size()),
                                         strand_.wrap(boost::bind(&personInRoom::writeHandler, shared_from_this(), _1)));
            }
        }
        else
        {
            room_.leave(shared_from_this());
        }
    }

    tcp::socket socket_;
    boost::asio::io_context::strand& strand_;
    chatRoom& room_;
    std::array<char, MAX_NICKNAME_SIZE> nickname_;
    std::array<char, MAX_IP_PACK_SIZE> read_msg_;
    std::deque<std::array<char, MAX_IP_PACK_SIZE> > write_msgs_;
};


#endif