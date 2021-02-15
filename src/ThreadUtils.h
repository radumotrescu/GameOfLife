#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    Semaphore(int count_ = 0)
        : count(count_)
    {
    }

    inline void notify() {
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        //notify the waiting thread
        cv.notify_one();
    }

    inline void notify(int n) {
        for (int i = 0; i < n; i++)
            notify();
    }

    inline void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        while (count == 0) {
            //wait on the mutex until notify is called
            cv.wait(lock);
        }
        count--;
    }
private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};

class Barrier
{
public:
    Barrier(int _n) :n(_n)
    {};
    void phase1()
    {
        mutex.wait();
        count++;

        if (count == n)
        {
            turnstile1.notify(n);
        }
        mutex.notify();

        turnstile1.wait();
    }
    void phase2()
    {
        mutex.wait();
        count--;

        if (count == 0)
        {
            turnstile2.notify(n);
        }
        mutex.notify();
        turnstile2.wait();
    }

    void wait()
    {
        phase1();
        phase2();
    }
private:
    int n = 0;
    int count = 0;
    Semaphore mutex = Semaphore(1);
    Semaphore turnstile1 = Semaphore(0);
    Semaphore turnstile2 = Semaphore(0);
};
