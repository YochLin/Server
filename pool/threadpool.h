#ifndef  _THREADPOOL_H_
#define  _THREADPOOL_H_

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>
#include <assert.h>

struct Pool {
    bool isClosed;
    std::mutex mtx;
    std::condition_variable cond;
    std::queue<std::function<void()>> tasks;
};
class ThreadPool
{
public:
    /*
     * 初始化線程池
     */
    explicit ThreadPool(size_t thread_num = 8) : pool_(std::make_shared<Pool>()){
        for(size_t i = 0; i < thread_num; i++) {
            std::thread([pool = pool_]{
                std::unique_lock<std::mutex> locker(pool->mtx);
                while(1) {
                    if(!pool->tasks.empty()) {
                        auto task = std::move(pool->tasks.front());
                        pool->tasks.pop();
                        locker.unlock();
                        task();
                        locker.lock();
                    }
                    else if(pool->isClosed) break;
                    else pool->cond.wait(locker);
                }
            }).detach();
        }
    }
    ThreadPool() = default;

    ~ThreadPool() {
        if(static_cast<bool>(pool_)) {
            {
                std::unique_lock<std::mutex> locker(pool_->mtx);
                pool_->isClosed = true;
            }
            pool_->cond.notify_all();
        }
    }

    /*
     *  將任務加入隊列中，並呼叫線程去處理
     */
    template<class T>
    void AddTask(T task) {
        {
            std::unique_lock<std::mutex> locker(pool_->mtx);
            pool_->tasks.emplace(std::forward<T>(task));
        }
        pool_->cond.notify_one();
    }

private:
    std::shared_ptr<Pool> pool_;
};

#endif