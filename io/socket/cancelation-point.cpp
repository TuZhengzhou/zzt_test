#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t   cond = PTHREAD_COND_INITIALIZER;

// void* thread0(void* arg)
// {
//   pthread_mutex_lock(&mutex);
//   pthread_cond_wait(&cond, &mutex);
//   pthread_mutex_unlock(&mutex);
//   pthread_exit(NULL);
// }
void cleanup(void *arg)
{
  pthread_mutex_unlock(&mutex);
}

// 取消点是“在推迟取消模式(deferred 模式)下取消能生效的位置”。

// POSIX 中的两种取消模式	            描述
// Deferred cancellation（默认）	    线程只有在“取消点”才会被取消
// Asynchronous cancellation	       线程在任何时刻都可能被取消（危险！）

// 取消点本质上是：在 deferred 模式下检查取消请求并触发取消
// glibc 为了实现阻塞类取消点（如 pthread_cond_wait），
// 确实会在内部短暂切换到异步取消模式，但这只是实现细节，不是取消点的本质特征。

// 目标线程到达取消点
// → glibc 检查到 pending cancel
// → glibc 开始取消流程
//     • 执行 cleanup handlers（LIFO 顺序）
//     • 恢复 mutex 状态
//     • 调用 pthread_exit(PTHREAD_CANCELED)
// → 线程终止


void* thread0(void* arg)
{
  // 在同一个线程里，所有通过 __pthread_cleanup_push 注册、但尚未被对应 __pthread_cleanup_pop 弹出的 cleanup handler，在线程被取消时都会被依次调用，而且是典型的栈结构：后注册的先调用（LIFO）。
  pthread_cleanup_push(cleanup, NULL);  // thread cleanup handler
  printf("thread0::before wait\n");
  fsync(2);
  pthread_mutex_lock(&mutex);
  pthread_cond_wait(&cond, &mutex);
  pthread_mutex_unlock(&mutex);
  pthread_cleanup_pop(0); // 正常返回时会执行到这里，传入 0 表示弹出但不执行

  printf("thread0::after wait. This thread have been cancel, will not reach here.\n");
  fsync(2);
  pthread_exit(NULL);
}

void* thread1(void* arg)
{
  sleep(6);
  pthread_mutex_lock(&mutex);
  pthread_mutex_unlock(&mutex);
  pthread_cond_broadcast(&cond);
  pthread_exit(NULL);
}
int main()
{
  pthread_t tid[2];
  if (pthread_create(&tid[0], NULL, &thread0, NULL) != 0) {
    exit(1);
  }
  if (pthread_create(&tid[1], NULL, &thread1, NULL) != 0) {
    exit(1);
  }
  sleep(3);
  pthread_cancel(tid[0]);

  pthread_join(tid[0], NULL);
  pthread_join(tid[1], NULL);

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
  return 0;
}