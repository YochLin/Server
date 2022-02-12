#ifndef _TIMER_H_
#define _TIMER_H_

#include <queue>
#include <unordered_map>
#include <time.h>
#include <algorithm>
#include <arpa/inet.h>
#include <functional>
#include <assert.h>
#include <chrono>

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef Clock::time_point TimeStamp;

struct TimerNode {
    int id;
    TimeStamp expires;
    TimeoutCallBack cb;
    bool operator<(const TimerNode &t) {
        return expires < t.expires;
    }
};

class Timer {
public:

    Timer() {
        heap_.reserve(64);
    }

    ~Timer() {
        Clear();
    }

    void Adjust(int32_t fd, int newExpires);                                // 調整計時器位置
    void Add(int32_t fd, int timeout, const TimeoutCallBack& cb);           // 加入新的事件計時器 / 增加現有事件時間 
    void DoWork(int32_t fd);                                                // 刪除節點並觸發回調函數
    void Clear();

    void Tick();                                                            // 清除超時節點
    void Pop();                                                             // 移除最頂節點
    int GetNextTick();

private:
    
    void Del(size_t i);                                                     // 刪除節點
    void Siftup(size_t i);                                                  // 往上移動節點
    bool Siftdown(size_t index, size_t n);                                  // 往下移動節點
    void SwapNode(size_t i, size_t j);                                      // 交換節點位置

    std::vector<TimerNode> heap_;                                           // 存放用戶時間
    std::unordered_map<int, size_t> ref_;                                   // 紀錄不同用戶的時間
};


#endif