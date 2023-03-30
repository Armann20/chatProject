#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H
#include "common_headers.h"
namespace {
class workerThread
{
public:
    static void run(std::shared_ptr<boost::asio::io_context> io_context)
    {
        {
            std::lock_guard < std::mutex > lock(m);
            std::cout << "[" << std::this_thread::get_id() << "] Thread starts" << std::endl;
        }

        io_context->run();

        {
            std::lock_guard < std::mutex > lock(m);
            std::cout << "[" << std::this_thread::get_id() << "] Thread ends" << std::endl;
        }

    }
private:
    static std::mutex m;
};
std::mutex workerThread::m;
}
#endif