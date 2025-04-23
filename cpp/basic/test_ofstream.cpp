#include <fstream>
#include <iostream>
#include <ctime>

int main(int argc, char* argv[]) {
    
    std::cout << "argc: " << argc << std::endl;
    for(int i = 0; i < argc; i++) {
        std::cout << "argv." << i << ": " << argv[i] << std::endl;
    }
    std::ofstream fout;
    fout.open("savetime.txt");
    fout << std::time(0);

    return 0;
}