// mqueue_example_fixed.cpp
#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

// 清理函数：删除可能残留的消息队列
void cleanup_message_queue(const std::string& queue_name) {
    mq_unlink(queue_name.c_str());
}

class MessageQueue {
private:
    mqd_t mq_handle_;
    std::string name_;
    bool is_owner_;
    
public:
    // 打开或创建消息队列
    MessageQueue(const std::string& name, bool create = false, 
                 bool overwrite = false,  // 新增：是否覆盖已存在的队列
                 size_t max_msgs = 10, size_t max_msg_size = 1024)
        : name_(name), is_owner_(create) {
        
        int flags = O_RDWR;
        if (create) {
            flags |= O_CREAT;
            if (overwrite) {
                // 先尝试删除已存在的队列
                mq_unlink(name.c_str());
            } else {
                flags |= O_EXCL;  // 如果已存在则失败
            }
        }
        
        struct mq_attr attr = {0};
        attr.mq_flags = 0;
        attr.mq_maxmsg = static_cast<long>(max_msgs);
        attr.mq_msgsize = static_cast<long>(max_msg_size);
        attr.mq_curmsgs = 0;
        
        mode_t mode = 0666;
        mq_handle_ = mq_open(name.c_str(), flags, mode, create ? &attr : nullptr);
        
        if (mq_handle_ == (mqd_t)-1) {
            std::string error_msg = "Failed to open message queue '";
            error_msg += name;
            error_msg += "': ";
            error_msg += strerror(errno);
            throw std::runtime_error(error_msg);
        }
    }
    
    // 发送消息
    void send(const std::string& message, unsigned int priority = 0) {
        if (mq_send(mq_handle_, message.c_str(), message.size() + 1, priority) == -1) {
            throw std::system_error(errno, std::system_category(), 
                                   "Failed to send message");
        }
    }
    
    // 接收消息
    std::string receive(unsigned int* priority = nullptr) {
        struct mq_attr attr = {0};
        if (mq_getattr(mq_handle_, &attr) == -1) {
            throw std::system_error(errno, std::system_category(),
                                   "Failed to get queue attributes");
        }
        
        char* buffer = new char[attr.mq_msgsize];
        unsigned int prio = 0;
        
        ssize_t bytes = mq_receive(mq_handle_, buffer, attr.mq_msgsize, &prio);
        if (bytes == -1) {
            delete[] buffer;
            throw std::system_error(errno, std::system_category(),
                                   "Failed to receive message");
        }
        
        if (priority) {
            *priority = prio;
        }
        
        std::string result(buffer);
        delete[] buffer;
        return result;
    }
    
    // 获取队列中的消息数量
    size_t get_message_count() const {
        struct mq_attr attr = {0};
        if (mq_getattr(mq_handle_, &attr) == -1) {
            throw std::system_error(errno, std::system_category(),
                                   "Failed to get queue attributes");
        }
        return attr.mq_curmsgs;
    }
    
    // 析构函数
    ~MessageQueue() {
        if (mq_handle_ != (mqd_t)-1) {
            mq_close(mq_handle_);
            if (is_owner_) {
                mq_unlink(name_.c_str());
            }
        }
    }
    
    // 禁止拷贝
    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;
    
    // 允许移动
    MessageQueue(MessageQueue&& other) noexcept
        : mq_handle_(other.mq_handle_), 
          name_(std::move(other.name_)), 
          is_owner_(other.is_owner_) {
        other.mq_handle_ = (mqd_t)-1;
        other.is_owner_ = false;
    }
    
    MessageQueue& operator=(MessageQueue&& other) noexcept {
        if (this != &other) {
            this->~MessageQueue();
            mq_handle_ = other.mq_handle_;
            name_ = std::move(other.name_);
            is_owner_ = other.is_owner_;
            other.mq_handle_ = (mqd_t)-1;
            other.is_owner_ = false;
        }
        return *this;
    }
};

// 版本1：简单的示例，自动清理
void simple_example_auto_cleanup() {
    const std::string queue_name = "/mq_test_example";
    
    try {
        // 先清理可能存在的旧队列
        cleanup_message_queue(queue_name);
        
        std::cout << "Creating message queue..." << std::endl;
        MessageQueue mq(queue_name, true, false, 5, 256);
        
        std::cout << "Sending message..." << std::endl;
        mq.send("Hello, World!");
        
        std::cout << "Message count: " << mq.get_message_count() << std::endl;
        
        std::string received = mq.receive();
        std::cout << "Received: " << received << std::endl;
        
        std::cout << "Simple example completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// 版本2：生产者-消费者示例
void producer_consumer_safe() {
    const std::string queue_name = "/mq_pc_example";
    
    // 在fork之前先清理队列
    cleanup_message_queue(queue_name);
    
    pid_t pid = fork();
    
    if (pid == 0) {  // 子进程 - 消费者
        sleep(1);
        try {
            std::cout << "[Consumer] Starting..." << std::endl;
            
            // 打开队列（不创建，因为父进程会创建）
            MessageQueue mq(queue_name, false);
            
            unsigned int priority = 0;
            for (int i = 0; i < 5; ++i) {
                std::string msg = mq.receive(&priority);
                std::cout << "[Consumer] Received: " << msg << "[priority = " << priority << "]" << std::endl;
            }
            
            std::cout << "[Consumer] All messages received!" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "[Consumer] Error: " << e.what() << std::endl;
        }
        
    } else if (pid > 0) {  // 父进程 - 生产者
        try {
            // 等待消费者启动
            sleep(1);
            
            std::cout << "[Producer] Creating queue..." << std::endl;
            
            // 创建队列
            MessageQueue mq(queue_name, true, false, 10, 256);
            
            for (int i = 0; i < 5; ++i) {
                std::string msg = "Message " + std::to_string(i);
                unsigned int priority = i % 3;
                mq.send(msg, priority);
                std::cout << "[Producer] Sent: " << msg << "[priority = " << priority << "]" << std::endl;
                sleep(1);  // 等待1秒
            }
            
            std::cout << "[Producer] All messages sent!" << std::endl;
            
            // 等待子进程结束
            wait(nullptr);
            
        } catch (const std::exception& e) {
            std::cerr << "[Producer] Error: " << e.what() << std::endl;
        }
        
    } else {
        std::cerr << "Fork failed!" << std::endl;
    }
}

// 版本3：使用不同的队列名避免冲突
void unique_queue_example() {
    // 使用进程ID创建唯一的队列名
    pid_t pid = getpid();
    std::string queue_name = "/mq_unique_" + std::to_string(pid) + "_" + std::to_string(time(nullptr));
    
    try {
        std::cout << "Using queue: " << queue_name << std::endl;
        
        // 创建队列
        MessageQueue mq(queue_name, true, false, 5, 256);
        
        std::cout << "Queue created successfully!" << std::endl;
        
        // 测试发送和接收
        mq.send("Test message 1");
        mq.send("Test message 2");
        
        std::cout << "Messages in queue: " << mq.get_message_count() << std::endl;
        
        std::cout << "Received: " << mq.receive() << std::endl;
        std::cout << "Received: " << mq.receive() << std::endl;
        
        std::cout << "Unique queue example completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// 版本4：主程序，让用户选择运行哪个示例
int main(int argc, char* argv[]) {
    std::cout << "=== Message Queue Examples (Fixed Version) ===" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "1. Simple example with auto-cleanup" << std::endl;
    std::cout << "2. Producer-consumer example" << std::endl;
    std::cout << "3. Unique queue example" << std::endl;
    std::cout << "4. Clean all message queues" << std::endl;
    std::cout << "Enter choice (1-4): ";
    
    int choice = 1;
    std::cin >> choice;
    
    switch (choice) {
        case 1:
            std::cout << "\n=== Running Simple Example ===" << std::endl;
            simple_example_auto_cleanup();
            break;
            
        case 2:
            std::cout << "\n=== Running Producer-Consumer Example ===" << std::endl;
            producer_consumer_safe();
            break;
            
        case 3:
            std::cout << "\n=== Running Unique Queue Example ===" << std::endl;
            unique_queue_example();
            break;
            
        case 4:
            std::cout << "\n=== Cleaning Message Queues ===" << std::endl;
            system("rm -f /dev/mqueue/* 2>/dev/null");
            std::cout << "All message queues cleaned." << std::endl;
            break;
            
        default:
            std::cout << "Invalid choice. Running simple example." << std::endl;
            simple_example_auto_cleanup();
    }
    
    return 0;
}