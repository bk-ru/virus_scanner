#include "threadPool.h"

namespace Scanner {

ThreadPool::ThreadPool(size_t numThreads) 
    : stop_(false), activeTasks_(0) {
    if (numThreads == 0)
        throw std::invalid_argument("ThreadPool must have at least 1 thread");
    for (size_t i = 0; i < numThreads; ++i) {
        workers_.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    condition_.wait(lock, [this] { 
                        return stop_ || !tasks_.empty(); 
                    });
                    
                    if (stop_ && tasks_.empty())
                        return;
                    
                    if (!tasks_.empty()) {
                        task = std::move(tasks_.front());
                        tasks_.pop();
                        activeTasks_++;
                    }
                }
                
                if (task) {
                    task();
                    activeTasks_--;
                    finished_.notify_one();
                }
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    Stop();
}

void ThreadPool::Wait() {
    std::unique_lock<std::mutex> lock(queueMutex_);
    finished_.wait(lock, [this] { 
        return tasks_.empty() && activeTasks_ == 0; 
    });
}

void ThreadPool::Stop() {
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        stop_ = true;
    }
    condition_.notify_all();
    
    for (auto& worker : workers_)
        if (worker.joinable())
            worker.join();
}

} // namespace Scanner