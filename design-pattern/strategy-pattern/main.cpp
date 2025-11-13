#include <iostream>
#include <memory>

#include "fly_behavior.h"
#include "quack_behavior.h"

class Duck {
    using FlyBehaviorPtr = std::unique_ptr<FlyBehavior>;
    using QuackBehaviorPtr = std::unique_ptr<QuackBehavior>;
public:
    void Swim();
    void PerformFly();
    void PerformQuack();
    void SetFly(FlyBehaviorPtr other);
    void SetQuack(QuackBehaviorPtr other);
protected:
    FlyBehaviorPtr fly_behave_;
    QuackBehaviorPtr quack_behave_;
};

void Duck::Swim() {
    std::cout << "duck swim" << std::endl;
}

void Duck::PerformFly() {
    this->fly_behave_->fly();
}

void Duck::PerformQuack() {
    this->quack_behave_->quack();
}

void Duck::SetFly(FlyBehaviorPtr other) {
    printf("Change Fly Behave\n");
    this->fly_behave_ = std::move(other);
}

void Duck::SetQuack(QuackBehaviorPtr other) {
    printf("Change Quack Behave\n");
    this->quack_behave_ = std::move(other);
}

class RocketDuck: public Duck {
public:
    RocketDuck() {
        this->fly_behave_ = std::make_unique<FlyWithRocket>();
        this->quack_behave_ = std::make_unique<ZiZiQuack>();
    }
};

int main() {
    RocketDuck duck;
    printf("This is a RocketDuck, it can fly and quack\n");
    duck.PerformFly();
    duck.PerformQuack();
    
    printf("\nWe can change its behave, becasue this is\n");
    printf("------------STRATERGY PATTERN------------\n");
    duck.SetFly(std::make_unique<FlyWithWings>());
    duck.SetQuack(std::make_unique<GaGaQuack>());
    duck.PerformFly();
    duck.PerformQuack();
}