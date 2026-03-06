#include <iostream>
#include <string>
#include <time.h>

class Photo;

class Card {
friend class Photo;
public:
    Card(): card_id(-1) {};
    Card(int id): card_id(id) {};
private:
    std::string toString() {
        return "card_id is " + std::to_string(card_id);
    }
private:
    int card_id;
};

class Photo {
public:
    void setDesc(std::string& desc) {
        m_desc = std::move(desc);
    }
    const std::string& getDesc() const {
        return m_desc;
    }
    std::string toString () {
        std::string ans;
        ans += "desc is: " + m_desc + ", and ";
        ans += "card is is: " + std::to_string(m_card.card_id);
        return ans;
    }
private:
    std::string m_desc;
    Card m_card;
public:
    // friend std::ostream& operator<<(std::ostream& os, const Photo& p) {
    //     os << p.m_desc << ", and friend card";
    //     return os;
    // }
};

std::ostream& operator<<(std::ostream& os, const Photo& p) {
    os << p.getDesc() << ", and friend card";
    return os;
}

int main() {
    Photo p;
    std::string s = "This is a photo";
    p.setDesc(s);

    

    std::cout << "p is " << p << std::endl;
    std::cout << "s now is [" << s << "]" << std::endl;
    std::cout << "p.toString is " << p.toString() << std::endl;
}