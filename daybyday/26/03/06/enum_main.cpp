#include "enum.h"
#include "enum_wrapper.h"
#include <iostream>

int main() {
    EnumWrapper wrapper;
    wrapper.setColor(::BLACK);
    wrapper.setFeeling(Feeling::BLACK);
    std::cout << "color value = " << (short)wrapper.getColor() << std::endl;
    std::cout << "feeling value = " << (short)wrapper.getFeeling() << std::endl;

}