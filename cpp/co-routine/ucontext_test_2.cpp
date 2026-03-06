#include <ucontext.h>
#include <vector>

#define DEFAULT_STACK_SZIE (1024 * 128)
#define MAX_UTHREAD_SIZE 1024

enum ThreadState { FREE, RUNNABLE, RUNNING, SUSPEND };

struct schedule_t;

typedef void (*Fun)(void *arg);

struct uthread_t {
  ucontext_t ctx;
  Fun func;
  void *arg;
  enum ThreadState state;
  char stack[DEFAULT_STACK_SZIE];
};



struct schedule_t {
  ucontext_t main;
  int running_thread;
  uthread_t *threads;
  int max_index; // 曾经使用到的最大的index + 1

  schedule_t() : running_thread(-1), max_index(0) {
    threads = new uthread_t[MAX_UTHREAD_SIZE];
    for (int i = 0; i < MAX_UTHREAD_SIZE; i++) {
      threads[i].state = FREE;
    }
  }

  int get_next_ctx() const {
    int next = -1;
    for (int i = 1; i < max_index; i++) {
      if (threads[i].state == RUNNABLE) {
        next = i;
        break;
      }
    }
    return next;
  }

  ~schedule_t() { delete[] threads; }
};

void uthread_resume(schedule_t &schedule, int id) {
  if (id < 0 || id >= schedule.max_index) {
    return;
  }

  uthread_t *t = &(schedule.threads[id]);

  if (t->state == SUSPEND) {
    swapcontext(&(schedule.main), &(t->ctx));
  }
}

void uthread_yield(schedule_t &schedule) {
  if (schedule.running_thread != -1) {
    uthread_t *t = &(schedule.threads[schedule.running_thread]);
    t->state = SUSPEND;
    schedule.running_thread = -1;

    swapcontext(&(t->ctx), &(schedule.main));
  }
}

void uthread_body(schedule_t *ps) {
  int id = ps->running_thread;

  if (id != -1) {
    uthread_t *t = &(ps->threads[id]);

    t->func(t->arg);

    t->state = FREE;

    ps->running_thread = -1;
  }
}

int uthread_create(schedule_t &schedule, Fun func, void *arg) {
  int id = 0;

  for (id = 0; id < schedule.max_index; ++id) {
    if (schedule.threads[id].state == FREE) {
      break;
    }
  }

  if (id == schedule.max_index) {
    schedule.max_index++;
  }

  uthread_t *t = &(schedule.threads[id]);

  t->state = RUNNABLE;
  t->func = func;
  t->arg = arg;

  getcontext(&(t->ctx));

  t->ctx.uc_stack.ss_sp = t->stack;
  t->ctx.uc_stack.ss_size = DEFAULT_STACK_SZIE;
  t->ctx.uc_stack.ss_flags = 0;
  t->ctx.uc_link = &(schedule.main);
  schedule.running_thread = id;

  makecontext(&(t->ctx), (void (*)(void))(uthread_body), 1, &schedule);

  return id;
}

int schedule_finished(const schedule_t &schedule) {
  if (schedule.running_thread != -1) {
    return 0;
  } else {
    for (int i = 0; i < schedule.max_index; ++i) {
      if (schedule.threads[i].state != FREE) {
        return 0;
      }
    }
  }

  return 1;
}

void schedule_ctl(const schedule_t &self) {
  while (!schedule_finished(self)) {
    int id = self.get_next_ctx();
    swapcontext(&(t->ctx), &(schedule.main));
  }
}

ucontext_t main_ctx, schedule_ctx, func1_ctx, func2_ctx;



int main() {}