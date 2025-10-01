#include "Utilities/Threading/ThreadPool.hpp"
#include "Debug.hpp"

uint64_t GetThisThreadID()
{
    const auto id = std::this_thread::get_id();
    return std::hash<std::thread::id>{}(id);
}

namespace Rigel
{
    ThreadPool::ThreadPool(const size_t numThreads)
    {
        const auto _numThreads = numThreads == 0 ? std::thread::hardware_concurrency() : numThreads;

        for (size_t i = 0; i < _numThreads; ++i)
            m_WorkerThreads.emplace_back([this] { this->ThreadLoop(); });
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

    const std::vector<std::thread::id>& ThreadPool::GetThreadsIDs() const
    {
        while (true)
        {
            {
                std::unique_lock lock(m_ThreadIdMutex);
                if (m_ThreadIDs.size() == m_WorkerThreads.size())
                    break;
            }
        }
        return m_ThreadIDs;
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
        {
            std::unique_lock lock(m_ThreadIdMutex);
            m_ThreadIDs.push_back(std::this_thread::get_id());
        }

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

            try {
                task();
            }
            catch (const std::exception& e) {
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
