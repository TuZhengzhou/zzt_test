#ifndef GENERAL_SUBJECT_H
#define GENERAL_SUBJECT_H

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

// -------------------- Observer 定义 --------------------

// 观察者：接受 shared_ptr<ConcreteSubject>
template <class ConcreteSubject>
using Observer = std::function<void(std::shared_ptr<ConcreteSubject>)>;

// -------------------- Subject 基类（带 CRTP + shared_from_this） --------------------
template <class ConcreteSubject>
class Subject : public std::enable_shared_from_this<ConcreteSubject> {
public:
    using Self       = ConcreteSubject;
    using Target     = Observer<ConcreteSubject>;
    using WeakTarget = std::weak_ptr<Target>;
    using Token      = std::shared_ptr<void>;

protected:
    Subject() { // 把构造/析构设为 protected，就不能在类外 new / 栈上构造 Subject<ConcreteSubject>
        // 可选：顺便校验 CRTP 写法是否正确
        static_assert(
            std::is_base_of<Subject<ConcreteSubject>, ConcreteSubject>::value,
            "ConcreteSubject must derive from Subject<ConcreteSubject>"
        );
    }
public:
    // 注册观察者，返回一个 token，释放 token 时自动清理过期观察者
    Token regist(Target target) {
        // 关键：这里的 shared_from_this() 是 enable_shared_from_this<ConcreteSubject> 的，
        // 返回 shared_ptr<ConcreteSubject>（也就是 WeatherData）
        std::shared_ptr<Self> self = this->shared_from_this();

        // 按值捕获 self，避免悬空引用
        auto token = std::shared_ptr<Target>(
            new Target(std::move(target)),
            [wself = std::weak_ptr<Self>(self)](Target* p) {
                // 确保 Target 被 delete 掉
                std::unique_ptr<Target> guard(p);

                // 尝试拿回主题对象，存在的话做一次清理
                if (auto self = wself.lock()) {
                    self->clean_up();
                }
            }
        );

        observers_.push_back(WeakTarget(token));
        // 对外隐藏真实类型，只暴露 shared_ptr<void>
        return std::static_pointer_cast<void>(token);
    }

    // 主动注销：reset 掉 token 即可
    void un_regist(Token& token) {
        token.reset();
        // 真正清理在下次 clean_up 时进行
        static_cast<Self*>(this)->clean_up();
    }

    // Push 模式：直接把数据推给观察者
    template <typename... Args>
    void push(Args&&... args) {
        for (auto& w : observers_) {
            if (auto sp = w.lock()) {
                // 由上层自己决定 Observer 的签名是否用 push
                (*sp)(std::forward<Args>(args)...);
            }
        }
    }

    // Pull 模式：把自己（shared_ptr<Self>）交给观察者，让观察者自己拉数据
    void pull() {
        std::shared_ptr<Self> self = this->shared_from_this();
        for (auto& w : observers_) {
            if (auto sp = w.lock()) {
                (*sp)(self);
            }
        }
    }

    // 清理已经失效的 weak_ptr
    void clean_up() {
        observers_.erase(
            std::remove_if(
                observers_.begin(),
                observers_.end(),
                [](const WeakTarget& w) { return w.expired(); }
            ),
            observers_.end()
        );
    }

private:
    std::vector<WeakTarget> observers_;
};

#endif