#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_ITERATIONS 5000000
#define NUM_THREADS 2

// 共享变量，用于线程间通信
volatile int flag[NUM_THREADS];
pthread_barrier_t barrier;

// 获取当前时间（微秒）
static inline uint64_t get_time_us() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

// 线程函数
void *thread_func(void *arg) {
  int thread_id = *(int *)arg;
  int other_id = 1 - thread_id;

  // 等待所有线程准备就绪
  pthread_barrier_wait(&barrier);

  for (int i = 0; i < NUM_ITERATIONS; i++) {
    // 等待另一个线程设置标志
    while (flag[thread_id] == 0) {
      sched_yield(); // 主动让出CPU
    }

    // 清除自己的标志，设置对方标志
    flag[thread_id] = 0;
    flag[other_id] = 1;
  }

  return NULL;
}

void test_thread_switch() {
  pthread_t threads[NUM_THREADS];
  int thread_ids[NUM_THREADS] = {0, 1};

  // 初始化屏障
  pthread_barrier_init(&barrier, NULL, NUM_THREADS + 1); // +1 for main thread

  // 初始化标志
  flag[0] = 1; // 线程0先运行
  flag[1] = 0; // 线程1等待

  printf("开始线程切换测试...\n");
  printf("每个线程执行 %d 次切换\n", NUM_ITERATIONS);

  // 创建线程
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
  }

  // 等待所有线程准备就绪
  pthread_barrier_wait(&barrier);

  uint64_t start_time = get_time_us();

  // 等待所有线程完成
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  uint64_t end_time = get_time_us();

  // 计算平均切换时间
  uint64_t total_time = end_time - start_time;
  // 每次循环包含2次切换（A->B 和 B->A）
  int total_switches = NUM_ITERATIONS * 2;
  double avg_switch_time =
      (double)total_time * 1000 / total_switches; // 转换为纳秒

  printf("线程切换测试结果：\n");
  printf("总时间: %lu 微秒\n", total_time);
  printf("总切换次数: %d\n", total_switches);
  printf("平均每次切换时间: %.2f 纳秒\n", avg_switch_time);
  printf("平均每秒切换次数: %.2f\n",
         (double)total_switches / total_time * 1000000);

  pthread_barrier_destroy(&barrier);
}

#define SHM_SIZE 1024
// 进程切换测试
void test_process_switch() {
  printf("开始进程切换测试...\n");
  printf("每个进程执行 %d 次切换\n", NUM_ITERATIONS);

  // 创建共享内存用于进程间通信
  int shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
  if (shmid < 0) {
    perror("shmget failed");
    exit(1);
  }

  // 在共享内存中创建标志变量
  volatile int *flag = (volatile int *)shmat(shmid, NULL, 0);
  if (flag == (void *)-1) {
    perror("shmat failed");
    exit(1);
  }

  // 创建管道用于同步
  int pipefd[2];
  if (pipe(pipefd) < 0) {
    perror("pipe failed");
    exit(1);
  }

  pid_t pid = fork();

  if (pid < 0) {
    perror("fork failed");
    exit(1);
  }

  if (pid == 0) {     // 子进程
    close(pipefd[0]); // 关闭读端

    // 等待父进程信号
    char sync;
    read(pipefd[0], &sync, 1);

    // 子进程：等待标志为1，然后设置为0
    for (int i = 0; i < NUM_ITERATIONS; i++) {
      while (flag[0] == 0) {
        // 主动让出CPU
        sched_yield();
      }
      flag[0] = 0;
    }

    // 通知父进程完成
    write(pipefd[1], "D", 1);
    close(pipefd[1]);
    shmdt((void *)flag);
    exit(0);
  } else {            // 父进程
    close(pipefd[1]); // 关闭写端

    // 初始化标志
    flag[0] = 1;

    // 通知子进程开始
    write(pipefd[0], "S", 1);

    uint64_t start_time = get_time_us();

    // 父进程：设置标志为1，然后等待为0
    for (int i = 0; i < NUM_ITERATIONS; i++) {
      flag[0] = 1;
      while (flag[0] == 1) {
        sched_yield();
      }
    }

    // 等待子进程完成
    char done;
    read(pipefd[0], &done, 1);

    uint64_t end_time = get_time_us();

    // 等待子进程结束
    wait(NULL);

    // 计算平均切换时间
    uint64_t total_time = end_time - start_time;
    // 每次循环包含2次切换（父->子 和 子->父）
    int total_switches = NUM_ITERATIONS * 2;
    double avg_switch_time =
        (double)total_time * 1000 / total_switches; // 转换为纳秒

    printf("进程切换测试结果：\n");
    printf("总时间: %lu 微秒\n", total_time);
    printf("总切换次数: %d\n", total_switches);
    printf("平均每次切换时间: %.2f 纳秒\n", avg_switch_time);
    printf("平均每秒切换次数: %.2f\n",
           (double)total_switches / total_time * 1000000);

    // 清理
    close(pipefd[0]);
    shmdt((void *)flag);
    shmctl(shmid, IPC_RMID, NULL);
  }
}

// main.c - 包含上面的函数并运行测试
int main() {
  printf("=== 线程切换 vs 进程切换性能测试 ===\n\n");

  // 设置CPU亲和性（可选，使测试更稳定）
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset); // 绑定到CPU0
  sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);

  // 测试线程切换
  test_thread_switch();

  printf("\n-----------------------------------\n\n");

  // 测试进程切换
  test_process_switch();

  printf("\n=== 测试完成 ===\n");
  return 0;
}