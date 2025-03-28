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

namespace rge
{
    class ThreadPool
    {
    public:
        explicit ThreadPool(const size_t numThreads = 0);
        ~ThreadPool();

        ThreadPool(const ThreadPool& other) = delete;
        ThreadPool& operator = (const ThreadPool&) = delete;

        NODISCARD inline size_t GetSize() const { return m_WorkerThreads.size(); }
        NODISCARD inline size_t GetQueueSize() const
        {
            std::unique_lock lock(m_QueueMutex);
            return m_Tasks.size();
        }
        NODISCARD inline size_t GetActiveTasksCount() const { return m_ActiveTasks; }

        /**
         * Waits for all tasks to complete
         */
        void WaitForAll();

        /**
        * @brief Submits a task to be executed by the thread pool.
        * @param func The function to execute.
        * @param args The arguments to pass to the function.
        * @return A future that will contain the result of the function call.
        */
        template<class F, class... Args>
        auto Enqueue(F&& func, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
        {
            using return_type = std::invoke_result_t<F, Args...>;

            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(func), std::forward<Args>(args)...)
            );

            std::future<return_type> result = task->get_future();

            {
                std::unique_lock lock(m_QueueMutex);
                m_Tasks.emplace([task]() { (*task)(); });
            }

            m_QueueCondition.notify_one();

            return result;
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
