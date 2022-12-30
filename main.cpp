#include <iostream>
#include <string>
#include "Calculate.h"
using namespace std;
int main() {
    string example = "-3+5*3-(4+2)/log(4)+sin(3)-4^(2)";
    float res;
    res = calculate(example.c_str(), 0, example.length(), &res);
    cout << example << " = " << res << endl;
    string line;
    float result;
    while (true) {
        cin >> line;
        result = calculate(line.c_str(), 0, line.length(),&result);
        cout <<"=" << result << endl;
    }
}