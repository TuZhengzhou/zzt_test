#include <iostream>
#include <memory>

#include "observer.h"

// using SharedPassiveObserver = std::shared_ptr<PassiveObserver>;
// using SharedActiveObserver = std::shared_ptr<ActiveObserver>;

int main() {
    SharedPassiveObserver current = std::make_shared<CurrentDisplay>();
    SharedActiveObserver statistic = std::make_shared<StatisticDisplay>();
    WeatherData wd;
    wd.regist("current", current);  // 在这个设计里, 仅注册了被动观察者

    wd.setHumidity(50);
    wd.setPressure(101);
    wd.setTemp(30);
    wd.measurementChanged();    // PassiveObserver 会被被动通知并更新
    current->display();
    statistic->update(static_cast<Subject&>(wd));   // ActiveObserver 主动获取更新
    statistic->display();

    wd.setHumidity(0);
    wd.setPressure(0);
    wd.setTemp(0);
    wd.measurementChanged();
    current->display();
    statistic->update(static_cast<Subject&>(wd));
    statistic->update(static_cast<Subject&>(wd));
    statistic->display();
}