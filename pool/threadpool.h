#ifndef  _THREADPOOL_H_
#define  _THREADPOOL_H_

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>
#include <assert.h>
class ThreadPool
{
public:
    explicit ThreadPool(size_t threadCount = 8): pool_(std::make_shared<Pool>()){
        assert(threadCount > 0);
        for(size_t i = 0; i < threadCount; i++){
            std::thread([pool = pool_](){
                std::unique_lock<std::mutex> locker(pool->mtx);
                while(true){
                    if(!pool->tasks.empty()){
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


private:
    struct Pool {
        std::mutex mtx;
        std::condition_variable cond;
        bool isClosed;
        std::queue<std::function<void()>> tasks;
    };
    std::shared_ptr<Pool> pool_;
};

#endif