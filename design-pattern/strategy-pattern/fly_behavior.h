#ifndef FLY_BEHAVIOR_H
#define FLY_BEHAVIOR_H

#include <iostream>

struct FlyBehavior {
    virtual void fly() = 0;
    virtual ~FlyBehavior() {

    };
};

struct FlyWithWings: public FlyBehavior {
    void fly() override {
        printf("FlyWithWings\n");
    }
};

struct FlyWithRocket: public FlyBehavior {
    void fly() override {
        printf("FlyWithRocket\n");
    }
};

struct FlyNoWay: public FlyBehavior {
    void fly() override {
        printf("FlyNoWay\n");
    }
};

#endif