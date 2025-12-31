#include <vector>
#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

using namespace std;
int main() {

    YAML::Node config = YAML::LoadFile("config.yaml");
    
    vector<string> illegal_rules = config["illegal_rules"].as<vector<string>>();
    cout << illegal_rules.size() << endl;
    for (const string& rule : illegal_rules) {
        cout << rule << endl;
    }
    return 0;
}

