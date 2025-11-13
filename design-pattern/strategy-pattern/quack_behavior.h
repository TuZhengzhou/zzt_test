#ifndef QUACK_BEHAVIOR_H
#define QUACK_BEHAVIOR_H

#include <iostream>

struct QuackBehavior {
    virtual void quack() = 0;
    virtual ~QuackBehavior() {

    };
};

struct GaGaQuack: public QuackBehavior {
    void quack() override {
        printf("GaGaQuack\n");
    }
};

struct ZiZiQuack: public QuackBehavior {
    void quack() override {
        printf("ZiZiQuack\n");
    }
};

#endif