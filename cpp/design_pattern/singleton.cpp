#include <iostream>

class Random
{
public:
    Random(const Random &) = delete;            // Prevent copying
    Random &operator=(const Random &) = delete; // Prevent assignment
    Random(Random &&) = delete;                 // Prevent moving
    Random &operator=(Random &&) = delete;      // Prevent move assignment

    static Random &Get()
    {
        static Random instance; // Guaranteed to be destroyed
        return instance;        // Instantiated on first use
    }

    static float Float()
    {
        return Get().IFloat();
    }

private:
    float IFloat()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
    Random() {};
};

int main() {
    // Example usage
    std::cout << "Random float: " << Random::Float() << std::endl;
    std::cout << "Random float: " << Random::Float() << std::endl;
    std::cout << "Random float: " << Random::Float() << std::endl;

    return 0;
}