#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>

//See https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

class threadPool{
    public:
        void start();
        void queueJob(const std::function<void()>& job);
        void stop();
        bool busy();
        int getQueueSize();

    private:
        bool shouldTerminate = false;

        void threadLoop();
        std::mutex queueMutex;
        std::condition_variable mutexCondition;
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> jobs;

};

void threadPool::start(){
    const int numThreads = std::thread::hardware_concurrency();
    threads.resize(numThreads);
    for(int i = 0; i < numThreads; i++){
        threads.at(i) = std::thread(&threadPool::threadLoop, this);
    }
}

void threadPool::threadLoop() {
    while(true){
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            mutexCondition.wait(lock, [this] {
                return !jobs.empty() || shouldTerminate;
            });
            if(shouldTerminate){
                return;
            }
            job = jobs.front();
            jobs.pop();
        }
        job();
    }
}

void threadPool::queueJob(const std::function<void()>& job){
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        jobs.push(job);
    }
    mutexCondition.notify_one();
}

bool threadPool::busy(){
    bool poolBusy;
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        poolBusy = !jobs.empty();
    }
    return poolBusy;
}

void threadPool::stop(){
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        shouldTerminate = true;
    }
    mutexCondition.notify_all();
    for(std::thread& activeThread : threads){
        activeThread.join();
    }
    threads.clear();
}

int threadPool::getQueueSize(){
    return jobs.size();
}



#endif