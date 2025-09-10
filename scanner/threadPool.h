#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <stdexcept>

namespace Scanner {
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

public:
    template<typename F>
    void Enqueue(F&& task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            if (stop_) {
                throw std::runtime_error("Cannot enqueue on stopped ThreadPool");
            }
            tasks_.emplace(std::forward<F>(task));
        }
        condition_.notify_one();
    }
    
public:
    void Wait();
    void Stop();

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::condition_variable finished_;
    std::atomic<bool> stop_;
    std::atomic<size_t> activeTasks_;
};
} // namespace Scanner