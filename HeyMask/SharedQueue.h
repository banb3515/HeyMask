//
//  SharedQueue.h
//  여러 스레드에서 공유하여 사용 가능한 대기열입니다.
//
//  Created by BanB on 2021/07/02.
//

#ifndef SharedQueue_h
#define SharedQueue_h

#include <queue>
#include <mutex>
#include <map>
#include <condition_variable>

using namespace std;

// 여러 스레드에서 공유하여 사용 가능한 대기열입니다.
template <typename T>
class SharedQueue
{
public:
    SharedQueue();
    ~SharedQueue();

    /*
    * 대기열의 맨 앞의 객체를 가져옵니다.
    * @return 대기열 맨 앞의 객체입니다.
    */
    T& Front();

    // 대기열의 맨 앞의 객체를 삭제합니다.
    void PopFront();

    /*
    * 대기열에 객체를 추가합니다.
    * @param item: 대기열에 추가할 객체입니다. (const)
    */
    void Push(const T& item);

    /*
    * 대기열에 객체를 추가합니다.
    * @param item: 대기열애 추가할 객체입니다.
    */
    void Push(T&& item);

    /*
    * 대기열의 사이즈(객체 개수)를 반환합니다.
    * @return 대기열의 사이즈입니다.
    */
    int Size();

    /*
    * 대기열이 비어있는지 확인합니다.
    * @return 대기열이 비어있으면 True, 비어있지 않으면 False룰 반환합니다.
    */
    bool Empty();

    // 대기열을 초기화합니다.
    void Clear();

private:
    deque<T> queue; // 대기열
    mutex mtx; // Mutex를 이용하여 서로 다른 스레드에서 참조하지 못하도록 하는 변수
    condition_variable cond; // 특정 조건이 만족될 때까지 스레드를 멈추도록 하는 변수
};

template <typename T>
SharedQueue<T>::SharedQueue() {}

template <typename T>
SharedQueue<T>::~SharedQueue() {}

template <typename T>
T& SharedQueue<T>::Front()
{
    unique_lock<mutex> mlock(mtx); // 다른 스레드에서 사용하지 못하도록 Mutex 잠금
    cond.wait(mlock, [this] { return !queue.empty(); });
    return queue.front(); // 대기열 맨 앞 객체 반환
}

template <typename T>
void SharedQueue<T>::PopFront()
{
    unique_lock<mutex> mlock(mtx); // 다른 스레드에서 사용하지 못하도록 Mutex 잠금
    cond.wait(mlock, [this] { return !queue.empty(); });
    queue.pop_front(); // 대기열 맨 앞 객체 삭제
}

template <typename T>
void SharedQueue<T>::Push(const T& item)
{
    unique_lock<mutex> mlock(mtx); // 다른 스레드에서 사용하지 못하도록 Mutex 잠금
    queue.push_back(item);
    cond.notify_one();
}

template <typename T>
void SharedQueue<T>::Push(T&& item)
{
    unique_lock<mutex> mlock(mtx); // 다른 스레드에서 사용하지 못하도록 Mutex 잠금
    queue.push_back(std::move(item));
    cond.notify_one();
}

template <typename T>
int SharedQueue<T>::Size()
{
    unique_lock<mutex> mlock(mtx); // 다른 스레드에서 사용하지 못하도록 Mutex 잠금
    return (int)queue.size(); // 대기열 사이즈 반환
}

template <typename T>
bool SharedQueue<T>::Empty()
{
    unique_lock<mutex> mlock(mtx); // 다른 스레드에서 사용하지 못하도록 Mutex 잠금
    return queue.empty(); // 대기열이 비어있는지 반환
}

template <typename T>
void SharedQueue<T>::Clear()
{
    unique_lock<mutex> mlock(mtx); // 다른 스레드에서 사용하지 못하도록 Mutex 잠금
    queue.clear(); // 대기열 초기화
}

#endif /* SharedQueue_h */
