#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <stdint.h>
#include <string.h>

#define NUM_CREATIONS 1000  // 每种测试的创建次数
#define MAX_THREADS 10000   // 最大线程数（防止系统资源耗尽）
#define MAX_PROCESSES 1000  // 最大进程数

// 获取当前时间（微秒）
static inline uint64_t get_time_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

// 获取进程内存使用（RSS，单位KB）
static long get_rss_kb() {
    FILE* file = fopen("/proc/self/statm", "r");
    if (!file) return 0;
    
    long size, resident, share, text, lib, data, dt;
    fscanf(file, "%ld %ld %ld %ld %ld %ld %ld", 
           &size, &resident, &share, &text, &lib, &data, &dt);
    fclose(file);
    
    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    return resident * page_size_kb;  // RSS in KB
}

// ==================== 线程创建测试 ====================

void* thread_dummy_func(void* arg) {
    // 线程立即退出
    return NULL;
}

void test_thread_creation_cost() {
    printf("测试线程创建开销...\n");
    printf("创建 %d 个线程\n", NUM_CREATIONS);
    
    pthread_t threads[MAX_THREADS];
    uint64_t total_time = 0;
    long total_memory = 0;
    
    // 获取初始内存使用
    long initial_memory = get_rss_kb();
    
    for (int i = 0; i < NUM_CREATIONS; i++) {
        uint64_t start_time = get_time_us();
        
        // 创建线程
        int ret = pthread_create(&threads[i], NULL, thread_dummy_func, NULL);
        
        uint64_t end_time = get_time_us();
        
        if (ret != 0) {
            printf("创建第 %d 个线程失败\n", i);
            break;
        }
        
        total_time += (end_time - start_time);
        
        // 每创建100个线程检查一次内存
        if (i % 100 == 0) {
            long current_memory = get_rss_kb();
            total_memory += (current_memory - initial_memory);
            initial_memory = current_memory;
        }
        
        // 每创建500个线程等待一下，防止资源耗尽
        if (i % 500 == 0 && i > 0) {
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
        }
    }
    
    // 等待所有线程结束
    for (int i = 0; i < NUM_CREATIONS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 计算平均开销
    double avg_time_us = (double)total_time / NUM_CREATIONS;
    double avg_memory_kb = (double)total_memory / (NUM_CREATIONS / 100);
    
    printf("线程创建测试结果：\n");
    printf("总创建时间: %lu 微秒\n", total_time);
    printf("平均每个线程创建时间: %.2f 微秒 (%.3f 毫秒)\n", 
           avg_time_us, avg_time_us / 1000);
    printf("估计每个线程内存开销: %.2f KB\n", avg_memory_kb);
    printf("-----------------------------------\n\n");
}

// ==================== 进程创建测试 ====================

void test_process_creation_cost() {
    printf("测试进程创建开销...\n");
    printf("创建 %d 个进程\n", NUM_CREATIONS);
    
    uint64_t total_time = 0;
    long total_memory = 0;
    
    // 获取初始内存使用
    long initial_memory = get_rss_kb();
    
    for (int i = 0; i < NUM_CREATIONS; i++) {
        uint64_t start_time = get_time_us();
        
        pid_t pid = fork();
        
        if (pid < 0) {
            printf("创建第 %d 个进程失败\n", i);
            break;
        }
        
        if (pid == 0) {  // 子进程
            // 子进程立即退出
            _exit(0);
        } else {  // 父进程
            uint64_t end_time = get_time_us();
            total_time += (end_time - start_time);
            
            // 等待子进程结束，避免僵尸进程
            waitpid(pid, NULL, 0);
            
            // 每创建10个进程检查一次内存
            if (i % 10 == 0) {
                long current_memory = get_rss_kb();
                total_memory += (current_memory - initial_memory);
                initial_memory = current_memory;
            }
        }
    }
    
    // 计算平均开销
    double avg_time_us = (double)total_time / NUM_CREATIONS;
    double avg_memory_kb = (double)total_memory / (NUM_CREATIONS / 10);
    
    printf("进程创建测试结果：\n");
    printf("总创建时间: %lu 微秒\n", total_time);
    printf("平均每个进程创建时间: %.2f 微秒 (%.3f 毫秒)\n", 
           avg_time_us, avg_time_us / 1000);
    printf("估计每个进程内存开销: %.2f KB\n", avg_memory_kb);
    printf("-----------------------------------\n\n");
}

// ==================== 批量创建测试 ====================

// 测试批量创建线程的开销
void test_batch_thread_creation() {
    printf("测试批量线程创建（创建后立即等待）...\n");
    
    int batch_sizes[] = {1, 10, 100, 1000};
    int num_batches = sizeof(batch_sizes) / sizeof(batch_sizes[0]);
    
    for (int b = 0; b < num_batches; b++) {
        int batch_size = batch_sizes[b];
        if (batch_size > MAX_THREADS) batch_size = MAX_THREADS;
        
        pthread_t threads[batch_size];
        
        uint64_t start_time = get_time_us();
        
        // 批量创建线程
        for (int i = 0; i < batch_size; i++) {
            pthread_create(&threads[i], NULL, thread_dummy_func, NULL);
        }
        
        // 等待所有线程
        for (int i = 0; i < batch_size; i++) {
            pthread_join(threads[i], NULL);
        }
        
        uint64_t end_time = get_time_us();
        uint64_t total_time = end_time - start_time;
        
        printf("批量创建 %4d 个线程: 总时间 %6lu 微秒, 平均 %.2f 微秒/线程\n",
               batch_size, total_time, (double)total_time / batch_size);
    }
    printf("-----------------------------------\n\n");
}

// 测试批量创建进程的开销
void test_batch_process_creation() {
    printf("测试批量进程创建...\n");
    
    int batch_sizes[] = {1, 5, 10, 50};
    int num_batches = sizeof(batch_sizes) / sizeof(batch_sizes[0]);
    
    for (int b = 0; b < num_batches; b++) {
        int batch_size = batch_sizes[b];
        if (batch_size > MAX_PROCESSES) batch_size = MAX_PROCESSES;
        
        uint64_t start_time = get_time_us();
        
        // 批量创建进程
        for (int i = 0; i < batch_size; i++) {
            pid_t pid = fork();
            
            if (pid == 0) {  // 子进程
                _exit(0);
            } else if (pid > 0) {  // 父进程
                // 继续创建下一个
                continue;
            } else {
                printf("创建进程失败\n");
                break;
            }
        }
        
        // 等待所有子进程
        for (int i = 0; i < batch_size; i++) {
            wait(NULL);
        }
        
        uint64_t end_time = get_time_us();
        uint64_t total_time = end_time - start_time;
        
        printf("批量创建 %4d 个进程: 总时间 %6lu 微秒, 平均 %.2f 微秒/进程\n",
               batch_size, total_time, (double)total_time / batch_size);
    }
    printf("-----------------------------------\n\n");
}

// ==================== 内存详细测试 ====================

void test_memory_usage_detailed() {
    printf("测试内存使用详情...\n\n");
    
    // 测试前状态
    printf("测试前内存状态:\n");
    system("free -h");
    printf("\n");
    
    // 创建10个线程并测量内存
    printf("创建10个线程后的内存变化:\n");
    long before_threads = get_rss_kb();
    
    pthread_t threads[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, thread_dummy_func, NULL);
    }
    
    // 给系统一点时间
    usleep(10000);
    
    long after_threads = get_rss_kb();
    
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("线程栈内存开销: %ld KB (约 %.1f KB/线程)\n", 
           after_threads - before_threads, 
           (double)(after_threads - before_threads) / 10);
    
    // 创建5个进程并测量内存
    printf("\n创建5个进程后的内存变化:\n");
    long before_procs = get_rss_kb();
    
    for (int i = 0; i < 5; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            usleep(10000);  // 子进程等待一下
            _exit(0);
        }
        waitpid(pid, NULL, 0);
    }
    
    long after_procs = get_rss_kb();
    
    printf("进程内存开销: %ld KB (约 %.1f KB/进程)\n",
           after_procs - before_procs,
           (double)(after_procs - before_procs) / 5);
    
    printf("-----------------------------------\n\n");
}

// ==================== 综合性能测试 ====================

void test_creation_performance() {
    printf("=== 进程 vs 线程创建性能综合测试 ===\n\n");
    
    // 警告信息
    printf("注意：\n");
    printf("1. 测试可能需要几秒钟时间\n");
    printf("2. 结果会因系统负载和配置而异\n");
    printf("3. 进程创建测试较慢，请耐心等待\n\n");
    
    // 设置CPU亲和性，使测试更稳定
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);  // 绑定到CPU0
    sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
    
    // 运行各种测试
    test_thread_creation_cost();
    
    test_process_creation_cost();
    
    test_batch_thread_creation();
    
    test_batch_process_creation();
    
    test_memory_usage_detailed();
    
    // 总结
    printf("=== 测试总结 ===\n");
    printf("1. 线程创建速度通常比进程快5-50倍\n");
    printf("2. 线程内存开销通常比进程小10-100倍\n");
    printf("3. 实际开销取决于：\n");
    printf("   - 系统负载\n");
    printf("   - 内核版本和配置\n");
    printf("   - 硬件性能\n");
    printf("   - 默认栈大小设置\n");
}

// ==================== 带参数的测试 ====================

void test_with_stack_size() {
    printf("\n=== 测试不同栈大小的影响 ===\n\n");
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    size_t stack_sizes[] = {8192, 16384, 32768, 65536, 131072, 1048576};  // 8KB到1MB
    int num_sizes = sizeof(stack_sizes) / sizeof(stack_sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        pthread_attr_setstacksize(&attr, stack_sizes[s]);
        
        pthread_t thread;
        uint64_t start_time = get_time_us();
        
        int ret = pthread_create(&thread, &attr, thread_dummy_func, NULL);
        
        uint64_t end_time = get_time_us();
        
        if (ret == 0) {
            pthread_join(thread, NULL);
            printf("栈大小 %6zu bytes: 创建时间 %lu 微秒\n", 
                   stack_sizes[s], end_time - start_time);
        }
    }
    
    pthread_attr_destroy(&attr);
}

int main(int argc, char* argv[]) {
    int run_all = 1;
    int run_stack_test = 0;
    
    // 解析命令行参数
    if (argc > 1) {
        if (strcmp(argv[1], "--stack") == 0) {
            run_all = 0;
            run_stack_test = 1;
        } else if (strcmp(argv[1], "--help") == 0) {
            printf("用法:\n");
            printf("  %s            运行所有测试\n", argv[0]);
            printf("  %s --stack    只运行栈大小测试\n", argv[0]);
            printf("  %s --help     显示帮助\n", argv[0]);
            return 0;
        }
    }
    
    if (run_all) {
        test_creation_performance();
    }
    
    if (run_stack_test) {
        test_with_stack_size();
    }
    
    return 0;
}