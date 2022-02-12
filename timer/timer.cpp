#include "timer.h"

void Timer::Add(int32_t fd, int timeout, const TimeoutCallBack& cb)
{
    size_t i;
    if(ref_.count(fd) == 0) { // 判斷此 fd 是否已經加入
        i = heap_.size();
        ref_[fd] = i;
        heap_.push_back({
            fd,
            Clock::now() + MS(timeout),
            cb
        });
        Siftup(i);
    }
    else {
        i = ref_[fd];
        heap_[i].expires = Clock::now() + MS(timeout);
        heap_[i].cb = cb;
        if(!Siftdown(i, heap_.size())) 
            Siftup(i);
    }
}

void Timer::Siftup(size_t i)
{
    int j = (i - 1) / 2;
    while(j >= 0) {
        if(heap_[j] < heap_[i])
            break;
        SwapNode(i, j);
        i = j;
        j = (i - 1) / 2;
    }
}

void Timer::SwapNode(size_t i , size_t j)
{
    std::swap(heap_[i], heap_[j]);
    ref_[heap_[i].id] = i;
    ref_[heap_[j].id] = j;
}

bool Timer::Siftdown(size_t index, size_t n)
{
    size_t i = index;
    size_t j = i * 2 + 1;
    while(j < n) {
        if(j + 1 < n && heap_[j + 1] < heap_[j])
            j++;
        if(heap_[i] < heap_[j])
            break;
        SwapNode(i, j);
        i = j;
        j = i * 2 - 1;
    }
    return i > index;
}

void Timer::Adjust(int32_t fd, int timeout)
{
    heap_[ref_[fd]].expires = Clock::now() + MS(timeout);
    Siftdown(ref_[fd], heap_.size());
}

void Timer::Del(size_t index)
{
    // 將要刪除的節點換到隊尾，並調整整體 heap 排序
    size_t i = index;
    size_t n = heap_.size() - 1;
    if(i < n) {
        SwapNode(i, n);
        if(!Siftdown(i, n))
            Siftup(i);
    }

    // 刪除隊尾節點
    ref_.erase(heap_.back().id);
    heap_.pop_back();
}

void Timer::DoWork(int32_t fd)
{
    int i = ref_[fd];
    TimerNode node = heap_[i];
    node.cb();
    Del(i);
}

void Timer::Tick()
{
    while(!heap_.empty()) {
        TimerNode node = heap_.front();
        if(std::chrono::duration_cast<MS>(node.expires - Clock::now()).count() > 0)
            break;
        node.cb();
        Pop();
    }
}

void Timer::Pop()
{
    Del(0);
}

void Timer::Clear()
{
    ref_.clear();
    heap_.clear();
}

int Timer::GetNextTick()
{
    Tick();
    int res = -1;
    if(!heap_.empty()) {
        res = std::chrono::duration_cast<MS>(heap_.front().expires - Clock::now()).count();
        if(res < 0)
            res = 0;
    }
    return res;
}