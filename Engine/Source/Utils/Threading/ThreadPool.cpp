#include "ThreadPool.hpp"
#include "Debug.hpp"
#include "SleepUtility.hpp"

namespace rge
{
    ThreadPool::ThreadPool(const size_t numThreads)
    {
        const auto _numThreads = numThreads == 0 ? std::thread::hardware_concurrency() : numThreads;

        m_WorkerThreads.reserve(_numThreads);
        for (size_t i = 0; i < _numThreads; ++i)
            m_WorkerThreads.emplace_back(std::thread(ThreadLoop, this));
    }

    ThreadPool::~ThreadPool()
    {
        m_ShouldStop = true;
        m_QueueCondition.notify_all();

        for (auto& thread : m_WorkerThreads)
        {
            if (thread.joinable())
                thread.join();
        }
    }

    size_t ThreadPool::GetQueueSize() const
    {
        std::unique_lock lock(m_QueueMutex);
        return m_Tasks.size();
    }

    void ThreadPool::WaitForAll()
    {
        std::unique_lock lock(m_QueueMutex);

        m_CompletionCondition.wait(lock, [this]
        {
           return m_Tasks.empty() && m_ActiveTasks == 0;
        });
    }

    void ThreadPool::ThreadLoop()
    {
        while (true)
        {
            std::function<void()> task;

            {
                std::unique_lock lock(m_QueueMutex);

                m_QueueCondition.wait(lock, [this]
                {
                    return m_ShouldStop || !m_Tasks.empty();
                });

                if (m_ShouldStop)
                    return;

                task = std::move(m_Tasks.front());
                m_Tasks.pop();
            }

            ++m_ActiveTasks;

            try
            {
                task();
            }
            catch (const std::exception& e)
            {
                Debug::Error("An exception was thrown when executing a task on a thread pool! Exception: {}", e.what());
            }

            --m_ActiveTasks;

            {
                std::unique_lock lock(m_QueueMutex);
                if (m_Tasks.empty() && m_ActiveTasks == 0)
                {
                    m_CompletionCondition.notify_one();
                }
            }
        }
    }
}
