#include <iostream>
#include <stdint.h>
#include <vector>
#include <memory>
#include <ucontext.h>
#include <functional>

class Fiber;
void func();
void loop();

ucontext_t main_contex;

Fiber* cur_fiber;
int id = 0;

class Fiber {
public:
  typedef std::shared_ptr<Fiber> ptr;

public:
  Fiber() {
    getcontext(&main_contex);

    void* main_ss_sp = malloc(sizeof(char) * 1024 * 1024);
    main_contex.uc_link = nullptr;
    main_contex.uc_stack.ss_sp = main_ss_sp;
    main_contex.uc_stack.ss_size = 1024 * 1024;

    makecontext(&main_contex, loop, 0);
  }
  Fiber(std::function<void()> cb, uint64_t stack_size)
    : m_cb(cb)
    , m_stack_size(stack_size)
  {
    m_id = ++id;
    m_name = "fiber-" + std::to_string(m_id);

    getcontext(&this->m_ctx);
    
    void* ss_sp = malloc(sizeof(char) * 1024 * 1024);
    // this->m_ctx.uc_link = &main_contex;
    this->m_ctx.uc_link = nullptr;
    this->m_ctx.uc_stack.ss_sp = ss_sp;
    this->m_ctx.uc_stack.ss_size = 1024 * 1024;

    makecontext(&this->m_ctx, func, 0);
  }
  ~Fiber() {
    free(this->m_ctx.uc_stack.ss_sp);
  }
  ucontext_t* getThis() {
    return &m_ctx;
  }
  uint64_t getId() {
    return m_id;
  }
  const std::string& getName() {
    return m_name;
  }
  uint64_t getStackSize() {
    return m_stack_size;
  }

private:
  uint64_t m_id;
  std::string m_name;
  ucontext_t m_ctx;
  // void *m_stack;
  std::function<void()> m_cb;
  uint64_t m_stack_size;
};

void func() {
  std::cout << " id = " << cur_fiber->getId() << std::endl \
            << " name = " << cur_fiber->getName() << std::endl \
            << " stack_size = " << cur_fiber->getStackSize() << std::endl;
            
  swapcontext(cur_fiber->getThis(), &main_contex);
  std::cout << " Back from main" << std::endl;
  swapcontext(cur_fiber->getThis(), &main_contex);
}

std::vector<Fiber::ptr> fibers;

void loop() {
  for (auto fiber: fibers) {
    cur_fiber = fiber.get();
    swapcontext(&main_contex, fiber->getThis());
    std::cout << " Back from func" << std::endl;
    swapcontext(&main_contex, fiber->getThis());
    std::cout << " Back from func the second time." << std::endl;
    std::cout << "-------------------------------------------\n";
  }
  printf("Finish loop");
}

Fiber main_fiber;
void* ptr = &main_fiber;

int main() {
  
  for (int i = 0; i < 5; i++) {
    Fiber::ptr fiber = std::make_shared<Fiber>(func, 1024*1024);
    fibers.push_back(fiber);
  }
  setcontext(&main_contex);
}