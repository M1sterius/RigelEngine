#pragma once

#include "Core.hpp"

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <atomic>

namespace Rigel
{
    class ThreadPool
    {
    public:
        /**
         * 
         * @param numThreads How many threads the pool will have. Pass 0 to use std::thread::hardware_concurrency()
         */
        explicit ThreadPool(const size_t numThreads = 0);
        ~ThreadPool();

        ThreadPool(const ThreadPool& other) = delete;
        ThreadPool& operator = (const ThreadPool&) = delete;

        NODISCARD inline size_t GetSize() const { return m_WorkerThreads.size(); }
        NODISCARD size_t GetQueueSize() const;
        NODISCARD inline size_t GetActiveTasksCount() const { return m_ActiveTasks; }

        /**
         * Waits for all tasks to complete
         */
        void WaitForAll();

        void Enqueue(const std::function<void()>& task)
        {
            {
                std::unique_lock lock(m_QueueMutex);
                m_Tasks.push(task);
            }
            m_QueueCondition.notify_one();
        }
    private:
        void ThreadLoop();

        std::vector<std::thread> m_WorkerThreads;
        std::queue<std::function<void()>> m_Tasks;

        mutable std::mutex m_QueueMutex;
        std::condition_variable m_QueueCondition;
        std::condition_variable m_CompletionCondition;

        std::atomic<bool> m_ShouldStop;
        std::atomic<size_t> m_ActiveTasks;
    };
}
