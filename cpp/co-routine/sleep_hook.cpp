// sleep_hook.cpp
#include <cstring>
#include <iostream>
#include <chrono>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <time.h>

#include "uthread.h"

extern schedule_t schedule;

// 创建定时器
int create_timer(unsigned int seconds) {
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timer_fd == -1) {
        std::cerr << "Failed to create timerfd" << std::endl;
        return -1;
    }
    
    // 设置定时器
    struct itimerspec its;
    memset(&its, 0, sizeof(its));
    its.it_value.tv_sec = seconds;
    its.it_value.tv_nsec = 0;
    
    if (timerfd_settime(timer_fd, 0, &its, NULL) == -1) {
        std::cerr << "Failed to set timer" << std::endl;
        close(timer_fd);
        return -1;
    }
    
    return timer_fd;
}


int regist(schedule_t& schedule, struct epoll_event ev) {
    schedule.fd_to_threadid[ev.data.fd] = schedule.running_thread;
    return epoll_ctl(schedule.epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev);
}

int sleep_impl(unsigned int seconds) {
    // 4. 记录开始时间
    std::cout << "[Sleep Hook] Start sleeping for " << seconds << " seconds" << std::endl;
    auto start_time = std::chrono::steady_clock::now();

    typedef unsigned int (*real_sleep_t)(unsigned int);
    static real_sleep_t real_sleep = nullptr;
    if (!real_sleep) {
        real_sleep = (real_sleep_t)dlsym(RTLD_NEXT, "sleep");
    }

    // 1. 创建定时器
    int timer_fd = create_timer(seconds);
    if (timer_fd == -1) {
        return real_sleep(seconds);  // 失败时回退到系统 sleep
    }
    
    // 3. 将定时器添加到 epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;  // 监听可读事件
    ev.data.fd = timer_fd;
    
    if (regist(schedule, ev) == -1) {
        std::cerr << "Failed to add timer to epoll" << std::endl;
        close(timer_fd);
        return real_sleep(seconds);
    }

    uthread_yield(schedule);

    return 0;
}

// g++ -shared -fPIC -o libsleep_hook.so sleep_hook.cpp -ldl
extern "C" unsigned int sleep(unsigned int seconds) {
    return sleep_impl(seconds);
}