#ifndef THREADPOOL_MUTEXQUEUE_H
#define THREADPOOL_MUTEXQUEUE_H

#include <queue>
#include <mutex>

template<typename T>
class MutexQueue {
private:
    std::mutex mutex_var;
    std::queue<T> queue;

public:
    void push(T &&item){
        std::lock_guard<std::mutex> lock(mutex_var);
        queue.emplace(std::forward<T>(item));
    }

    bool pop(T& item){
        std::lock_guard<std::mutex> lock(mutex_var);
        if(!queue.empty()){
            item = queue.front();
            queue.pop();
            return true;
        }
        else{
            return false;
        }
    }

    std::size_t size(){
        std::lock_guard<std::mutex> lock(mutex_var);
        return queue.size();
    }

    bool empty(){
        std::lock_guard<std::mutex> lock(mutex_var);
        return queue.empty();
    }
};


#endif //THREADPOOL_MUTEXQUEUE_H
