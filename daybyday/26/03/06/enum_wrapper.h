enum Color: unsigned char;
enum class Feeling: unsigned char;

class EnumWrapper {
public:
    void setColor(Color color) {
        m_color = color;
    }
    Color getColor() {
        return m_color;
    }
    void setFeeling(Feeling f) {
        m_feeling = f;
    }
    Feeling getFeeling() {
        return m_feeling;
    }
private:
    Feeling m_feeling;
    Color m_color;
};