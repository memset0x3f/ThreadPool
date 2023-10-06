#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include "MutexQueue.h"
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>
#include <functional>

class ThreadPool{
private:
    class ThreadWorker{
    private:
        int id;
        ThreadPool *pool;
    public:
        ThreadWorker(ThreadPool *pool_, int id_);

        void operator()();
    };

    bool is_shutdown;

    MutexQueue<std::function<void()>> queue;

    std::mutex pool_mutex;
    std::condition_variable condition;

    std::vector<std::thread> threads;

public:
    ThreadPool(const ThreadPool &other) = delete;
    ThreadPool(ThreadPool &&other) = delete;
    ThreadPool &operator=(const ThreadPool &other) = delete;
    ThreadPool &operator=(ThreadPool &&other) = delete;

    explicit ThreadPool(int n_threads = 4);

    void shutdown();

    template<typename T, typename... Args>
    auto submit(T &&f, Args &&... args) -> std::future<decltype(std::forward<T>(f)(std::forward<Args...>(args...)))>{
        using type = decltype(std::forward<T>(f)(std::forward<Args...>(args...)));
        std::function<type()> func = std::bind(std::forward<T>(f), std::forward<Args...>(args...));
        std::shared_ptr func_ptr = std::make_shared<std::packaged_task<type()>>(func);

        std::function<void()> wrapper_func = [func_ptr](){
            (*func_ptr)();
        };
        queue.push(std::move(wrapper_func));

        condition.notify_one();     // Wake up one thread if its waiting

        return func_ptr->get_future();
    }
};


#endif //THREADPOOL_THREADPOOL_H
