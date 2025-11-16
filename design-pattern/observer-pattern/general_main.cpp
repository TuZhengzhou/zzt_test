#include "general_subject.h"

// -------------------- 具体主题：WeatherData --------------------
class WeatherData : public Subject<WeatherData> {
public:
    float getTemp() const     { return temp_;      }
    float getHumidity() const { return humidity_;  }
    float getPressure() const { return pressure_;  }

    void setTemp(float t)     { temp_ = t; }
    void setHumidity(float h) { humidity_ = h; }
    void setPressure(float p) { pressure_ = p; }

private:
    float temp_     = 0.0f;
    float humidity_ = 0.0f;
    float pressure_ = 0.0f;
};

// -------------------- 两个显示器（观察者逻辑） --------------------

class CurrentDisplay {
public:
    void update(float t, float h, float p) {
        temp_ = t;
        humidity_ = h;
        pressure_ = p;
    }

    void display() const {
        std::cout << "[CurrentDisplay] "
                  << "Temp = " << temp_
                  << ", Humidity = " << humidity_
                  << ", Pressure = " << pressure_
                  << '\n';
    }

private:
    float temp_     = 0.0f;
    float humidity_ = 0.0f;
    float pressure_ = 0.0f;
};

class StatisticDisplay {
public:
    void update(float t, float h, float p) {
        count_++;
        sum_temp_     += t;
        sum_humidity_ += h;
        sum_pressure_ += p;
    }

    void display() const {
        if (count_ == 0) {
            std::cout << "[StatisticDisplay] no data yet.\n";
            return;
        }
        std::cout << "[StatisticDisplay] "
                  << "AvgTemp = "     << (sum_temp_     / count_)
                  << ", AvgHumidity = " << (sum_humidity_ / count_)
                  << ", AvgPressure = " << (sum_pressure_ / count_)
                  << " (based on " << count_ << " samples)\n";
    }

private:
    int   count_        = 0;
    float sum_temp_     = 0.0f;
    float sum_humidity_ = 0.0f;
    float sum_pressure_ = 0.0f;
};

// -------------------- main：把所有东西串起来 --------------------

int main() {
    auto current    = std::make_shared<CurrentDisplay>();
    auto statistic  = std::make_shared<StatisticDisplay>();

    auto subject = std::make_shared<WeatherData>();

    // 注册 CurrentDisplay：在 pull() 时，从 WeatherData 拉数据更新 current
    auto token_current = subject->regist(
        [w_current = std::weak_ptr<CurrentDisplay>(current)](std::shared_ptr<WeatherData> s) {
            auto s_current = w_current.lock();
            if (s_current) {
                s_current->update(s->getTemp(), s->getHumidity(), s->getPressure());
            }
        }
    );

    // 注册 StatisticDisplay：在 pull() 时，从 WeatherData 拉数据更新 statistic
    auto token_statistic = subject->regist(
        [w_statistic = std::weak_ptr<StatisticDisplay>(statistic)](std::shared_ptr<WeatherData> s) {
            auto s_statistic = w_statistic.lock();
            if (s_statistic) {
                s_statistic->update(s->getTemp(), s->getHumidity(), s->getPressure());
            }
        }
    );

    // 模拟一次数据更新
    subject->setHumidity(50);
    subject->setPressure(101);
    subject->setTemp(30);

    // 通知所有观察者（Pull 模式：把 subject 传进去）
    subject->pull();

    // 显示结果
    current->display();
    statistic->display();

    // 可以试试注销一个观察者
    subject->un_regist(token_current);

    std::cout << "After unregistering CurrentDisplay:\n";
    subject->setTemp(32);
    subject->setHumidity(55);
    subject->setPressure(100);
    subject->pull();

    current->display();      // 不会再更新
    statistic->display();    // 还会继续更新

    return 0;
}
