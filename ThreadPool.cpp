#include "ThreadPool.h"

ThreadPool::ThreadWorker::ThreadWorker(ThreadPool *pool_, const int id_):pool(pool_), id(id_){}

void ThreadPool::ThreadWorker::operator()(){
    std::function<void()> func;
    while(!pool->is_shutdown){
        std::unique_lock<std::mutex> lock(pool->pool_mutex);
        bool dequeued = pool->queue.pop(func);
        if(dequeued){   // Do task
            func();
        }
        else{       // No task to do, thread will wait
            // Double check, the thread pool might be shutdown during previous operation
            // Without double check, the thread will be blocked forever
            if(pool->is_shutdown){
                break;
            }
            pool->condition.wait(lock);
        }
    }
}

ThreadPool::ThreadPool(const int n_threads):is_shutdown(false), threads(std::vector<std::thread>(n_threads)){
    for(int i = 0; i < n_threads; i++){
        threads[i] = std::thread(ThreadWorker(this, i));
    }
}

void ThreadPool::shutdown(){
    is_shutdown = true;
    condition.notify_all();

    // Wait till current threads are finished
    for(auto &thread: threads){
        if(thread.joinable()){
            thread.join();
        }
    }
}
