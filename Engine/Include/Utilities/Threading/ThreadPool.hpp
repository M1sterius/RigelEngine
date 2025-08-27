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

        /**
         * Returns unique std::thread::id of all threads in the pool
         */
        NODISCARD const std::vector<std::thread::id>& GetThreadsIDs() const;

        NODISCARD size_t GetSize() const { return m_WorkerThreads.size(); }
        NODISCARD size_t GetQueueSize() const;
        NODISCARD size_t GetActiveTasksCount() const { return m_ActiveTasks; }

        /**
         * Waits for all scheduled and active tasks to complete
         */
        void WaitForAll();

        /**
         * @brief Enqueues a task to be executed on the thread pool.
         *
         * Returns a std::future that can be used to block the calling thread until
         * the task has been executed.
         *
         * @tparam Func The type of the callable object to be executed.
         * @tparam Args The types of the arguments to be passed to the callable.
         * @param func The callable object (e.g., function, lambda, functor).
         * @param args The arguments to be passed to the callable.
         * @return A future that can be used to retrieve the result of the task once it's completed.
         */
        template<typename Func, typename... Args>
        std::future<std::invoke_result_t<Func, Args...>> Enqueue(Func&& func, Args&&... args)
        {
            using RetType = std::invoke_result_t<Func, Args...>;

            auto taskPtr = std::make_shared<std::packaged_task<RetType()>>(
                std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
            );

            {
                std::unique_lock lock(m_QueueMutex);
                m_Tasks.emplace([taskPtr]() { (*taskPtr)(); });
            }
            m_QueueCondition.notify_one();

            return taskPtr->get_future();
        }
    private:
        void ThreadLoop();

        std::vector<std::thread::id> m_ThreadIDs;
        mutable std::mutex m_ThreadIdMutex;

        std::vector<std::thread> m_WorkerThreads;
        std::queue<std::function<void()>> m_Tasks;

        mutable std::mutex m_QueueMutex;
        std::condition_variable m_QueueCondition;
        std::condition_variable m_CompletionCondition;

        std::atomic<bool> m_ShouldStop;
        std::atomic<size_t> m_ActiveTasks;
    };
}
