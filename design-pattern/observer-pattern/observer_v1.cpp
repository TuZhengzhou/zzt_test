#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

class PassiveObserver;
class ActiveObserver;
class Subject;
class WeatherData;
class Displayer;
using SharedPassiveObserver = std::shared_ptr<PassiveObserver>;
using SharedActiveObserver = std::shared_ptr<ActiveObserver>;
using SharedSubject = std::shared_ptr<Subject>;

class Subject {
public:
    virtual void regist(std::string name, SharedPassiveObserver observer) = 0;
    virtual void remove(std::string name) = 0;
protected:
    std::unordered_map<std::string, SharedPassiveObserver> map_;

};

class PassiveObserver {
public:
    virtual void update(float temp, float humidity, float pressure) = 0;
    virtual void display() = 0;
};

class ActiveObserver {
public:
    virtual void update(Subject& subj) = 0;
    virtual void display() = 0;
};

class WeatherData: public Subject {
public:
    float getTemp() { return temp_; }
    float getHumidity() { return humidity_;   }
    float getPressure() { return pressure_;   }
    void setTemp(float temp) {  temp_ = temp;   }
    void setHumidity(float humidity)    {   humidity_ = humidity;   }
    void setPressure(float pressure)    {   pressure_ = pressure;   }
    void regist(std::string name, SharedPassiveObserver observer) override {
        if (map_.find(name) == map_.end()) {
            map_[name] = observer;
        } else {
            printf("This name has been registered. Change your name please\n");
        }
    }

    void remove(std::string name) override {
        auto iter = map_.find(name);
        if (iter != map_.end()) {
            map_.erase(iter);
        }
    }

    void measurementChanged() {
        for (auto iter: map_) {
            notify(iter.second);
        }
    }
private:
    void notify(SharedPassiveObserver ob) {
        ob->update(temp_, humidity_, pressure_);
    }

    float temp_ = 0;
    float humidity_ = 0;
    float pressure_ = 0;
};

// 由 Subject 推数据
class CurrentDisplay: public PassiveObserver {
public:
    void update(float temp, float humidity, float pressure) override {
        temp_ = temp;
        humidity_ = humidity;
        pressure_ = pressure;
    }
    void display() override {
        printf("CurrentDisplay. temp = %.2f, humidity = %.2f, pressure = %.2f\n\n", 
                temp_, humidity_, pressure_);
    }
private:
    float temp_ = 0;
    float humidity_ = 0;
    float pressure_ = 0;
    std::string name_ = "CurrentDisplay";
};

class StatisticDisplay: public ActiveObserver {
    void update(Subject& subj) override {
        if (typeid(subj) == typeid(WeatherData)) {
            auto& weather_data = dynamic_cast<WeatherData&>(subj);
            ave_temp_ = (ave_temp_ * cnt_ + weather_data.getTemp()) / (cnt_ + 1);
            ave_humidity_ = (ave_humidity_ * cnt_ + weather_data.getHumidity()) / (cnt_ + 1);
            ave_pressure_ = (ave_pressure_ * cnt_ + weather_data.getPressure()) / (cnt_ + 1);
        }

        cnt_ += 1;
    }
    void display() override {
        printf("StatisticDisplay. ave_temp_ = %.2f, ave_humidity_ = %.2f, ave_pressure_ = %.2f\n\n", 
                ave_temp_, ave_humidity_, ave_pressure_);
    }
    float ave_temp_ = 0;
    float ave_humidity_ = 0;
    float ave_pressure_ = 0;
    int cnt_ = 0;
    std::string name_ = "StatisticDisplay";


};

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