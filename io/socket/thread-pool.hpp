#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP
#include <condition_variable>
#include <cassert>
#include <cstdio>
#include <list>
#include <mutex>
#include <thread>

class ThreadPool {
private:
    struct Worker {
        // pthread_t   thread;
        std::thread thread;
        ThreadPool* pool;
        bool        is_working;
        bool        terminate;
    };
    struct Task {
        void (*func)(void* args_formal);
        void* args_real;
    };
public:
    ThreadPool(size_t num_worker, size_t num_max_jobs);
    ~ThreadPool();

    void PushJob(void (*func)(void* args_formal), void* args_real);

private:
    static void* StartingJob(void* args);
    void ThreadLoop(ThreadPool::Worker* worker);

private:
    Worker*         _workers;
    std::list<Task> _jobs;
    size_t          _num_worker;
    size_t          _num_max_jobs;
    std::mutex              _job_mutex;
    std::condition_variable _job_cv;
};

ThreadPool::ThreadPool(size_t num_worker, size_t num_max_jobs) {
    _workers    = new Worker[num_worker];
    if (_workers == nullptr) {
        perror("creat workers failed\n");
    }
    for (size_t i = 0; i < num_worker; i++) {
        _workers[i].terminate = false;
        _workers[i].is_working = false;
        _workers[i].pool = this;
        // if (pthread_create(&(_workers[i].thread), nullptr, StartingJob, &_workers[i])) {
        //     delete [] _workers;
        //     perror("create pthread failed\n");
        // }
        _workers[i].thread = std::thread(StartingJob, &_workers[i]);
    }
    _num_worker = num_worker;
    _num_max_jobs   = num_max_jobs;
}

ThreadPool::~ThreadPool() {
    // 检查是否所有任务都执行完毕
    while (true) {
        std::unique_lock<std::mutex> wait(_job_mutex);
        if (this->_jobs.size() == 0) {
            wait.unlock();
            break;
        }
        wait.unlock();
    }
    
    std::unique_lock<std::mutex> lock(_job_mutex);  // 让线程池中的线程不要再获取新的任务
    for (size_t i = 0; i < _num_worker; i++) {
        _workers[i].terminate = true;
    }
    lock.unlock();
    _job_cv.notify_all();
    for (size_t i = 0; i < _num_worker; i++) {
        // pthread_join(_workers[i].thread, nullptr);
        _workers[i].thread.join();
    }
    delete[] _workers;
}

void* ThreadPool::StartingJob(void* args) {
    ThreadPool::Worker* worker = static_cast<ThreadPool::Worker*>(args);

    worker->pool->ThreadLoop(worker);
    
    return nullptr;
}

void ThreadPool::ThreadLoop(ThreadPool::Worker* worker) {
    while(true) {
        std::unique_lock<std::mutex> lock(_job_mutex);
        _job_cv.wait(lock, [this, worker](){
            return this->_jobs.size() > 0 || worker->terminate == true;
        });
        if (worker->terminate == true) {
            lock.unlock();
            break;
        }
        assert(this->_jobs.size() > 0);
        auto task = this->_jobs.front();
        this->_jobs.pop_front();
        lock.unlock();

        task.func(task.args_real);
    }
    // pthread_exit(nullptr);
}

void ThreadPool::PushJob(void (*func)(void *), void *args_real) {
    Task task;
    task.func       = func;
    task.args_real  = args_real;
    std::unique_lock<std::mutex> lock(_job_mutex);
    this->_jobs.push_back(task);
    lock.unlock();
    _job_cv.notify_one();
}

#endif