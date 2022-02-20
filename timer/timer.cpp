#include "timer.h"

void Timer::Add(int32_t fd, int timeout, const TimeoutCallBack& cb)
{
    size_t index;
    //  判斷 fd 是否已存在
    if(ref_.count(fd) == 0) { 
        index = heap_.size();
        ref_[fd] = index;
        heap_.push_back({
            fd,
            Clock::now() + MS(timeout),
            cb
        });
        Siftup(index);
    }
    else {
        index = ref_[fd];
        heap_[index].expires = Clock::now() + MS(timeout);
        heap_[index].cb = cb;
        Siftdown(index);
    }
}

void Timer::SwapNode(size_t i , size_t j)
{
    std::swap(heap_[i], heap_[j]);
    ref_[heap_[i].id] = j;
    ref_[heap_[j].id] = i;
}

//      0
//    1    2
//  3  4  5  6
void Timer::Siftup(size_t index)
{
    int parent_node = (index - 1) / 2;
    if(parent_node >= 0 && heap_[index] < heap_[parent_node]) {
        SwapNode(index, parent_node);
        Siftup(parent_node);
    }
}

//      0
//    1    2
//  3  4  5  6
void Timer::Siftdown(size_t index)
{
    size_t left_child = (index + 1) * 2 - 1;
    size_t right_child = (index + 1) * 2;
    int small_index;
    // printf("%s, index: %d, left_child: %d, right_child: %d\n", __func__, index, left_child, right_child);

    if(left_child < heap_.size() && heap_[left_child] < heap_[index])
        small_index = left_child;
    else
        small_index = index;
    
    if(right_child < heap_.size() && heap_[right_child] < heap_[small_index])
        small_index = right_child;
    
    if(small_index != index) {
        SwapNode(small_index, index);
        Siftdown(small_index);
    }
    // return;
}

void Timer::Adjust(int32_t fd, int timeout)
{
    heap_[ref_[fd]].expires = Clock::now() + MS(timeout);
    Siftdown(ref_[fd]);
}

void Timer::Pop()
{
    heap_[0] = heap_[heap_.size() - 1];
    heap_.erase(heap_.begin() + heap_.size() - 1);
    Siftdown(1);
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