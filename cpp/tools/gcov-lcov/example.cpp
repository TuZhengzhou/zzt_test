// Project Identifier : NEILIAAPPLICATION
#include <iostream>
#include <string>
using namespace std;

/*
  - 生成  example.gcno example.gcda
    ./example < test-1.txt
  
  - 生成 example.cpp.gcov. 使用 gcov-13 跟 g++13 保持版本一致
    gcov-13 example.cpp

  - 生成 coverage.info. 
    lcov --capture --directory . --output-file coverage.info --gcov-tool gcov-13

  - 生成可视化网页. 页面在 res/index.html
    genhtml coverage.info --output-directory res
*/

int main() {
  char command;
  do {
    cin >> command;
    switch (command) {
      case 'Q':
        break;
      case 'A':
        cout << "command A executed!\n";
        break;
      case 'B':
        cout << "command B executed!\n";
        break;
      default:
        cout << "I did not recognize the following command: " << command << '\n';
        break;
      } 
  } while (command != 'Q');
  return 0;
}