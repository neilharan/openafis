#ifndef THREADPOOL_H
#define THREADPOOL_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <pthread.h>
#endif


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

class ThreadPool
{
public:
    enum class Priority { Low, Normal, High, Critical };

    ThreadPool(const size_t threadsCount = std::thread::hardware_concurrency(), const Priority priority = Priority::Normal)
    {
        m_threads.reserve(threadsCount);

        for (size_t i {}; i < threadsCount; ++i) {
            m_threads.emplace_back([=]() {
                setPriority(priority);

                for (;;) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(m_mutex);
                        m_condition.wait(lock, [=] { return m_terminating || !m_tasks.empty(); });
                        if (m_terminating && m_tasks.empty()) {
                            return;
                        }
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    ~ThreadPool()
    {
        {
            std::lock_guard lock(m_mutex);
            m_terminating = true;
        }
        m_condition.notify_all();
        for (auto& t : m_threads) {
            t.join();
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <class F> auto enqueue(F&& f)
    {
        using R = std::invoke_result_t<F>;
        auto task = std::make_shared<std::packaged_task<R()>>(std::forward<F>(f));
        auto r = task->get_future();

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_tasks.emplace([task]() { (*task)(); });
        }
        m_condition.notify_one();
        return r;
    }

    [[nodiscard]] size_t size() const { return m_threads.size(); }

private:
    static void setPriority(const Priority priority)
    {
#ifdef _WINDOWS_
        const auto p = [=]() {
            switch (priority) {
                case Priority::Low: {
                    return THREAD_PRIORITY_LOWEST;
                }
                default:
                case Priority::Normal: {
                    return THREAD_PRIORITY_NORMAL;
                }
                case Priority::High: {
                    return THREAD_PRIORITY_HIGHEST;
                }
                case Priority::Critical: {
                    return THREAD_PRIORITY_TIME_CRITICAL;
                }
            }
        };

        if (!SetThreadPriority(GetCurrentThread(), p())) {
            std::cerr << __FUNCTION__ << ": failed [SetThreadPriority(): failed; GetLastError() = " << GetLastError() << "]" << std::endl;
        }
#endif

#ifdef _PTHREAD_H
        const auto p = [=]() {
            switch (priority) {
                case Priority::Low: {
                    return -19;
                }
                default:
                case Priority::Normal: {
                    return 0;
                }
                case Priority::High: {
                    return -10;
                }
                case Priority::Critical: {
                    return -20;
                }
            }
        };

        const auto self = pthread_self();
        int policy;
        sched_param sp;
        pthread_getschedparam(self, &policy, &sp);
        sp.sched_priority = p();
        if (pthread_setschedparam(self, SCHED_OTHER, &sp)) {
            std::cerr << __FUNCTION__ << ": failed [pthread_setschedparam(): failed; errno = " << std::strerror(errno) << "]" << std::endl;
        }
#endif
    }

    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_terminating {};
};

}

#endif // THREADPOOL_H
