#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>
int main() {
    std::cout << "hello" << std::endl;
    std::vector<int> vtNum{1, 2, 3, 4};
    for (const auto &i : vtNum) 
    { 
        std::cout << i << std::endl;
        std::cout<< "hell"<<std::endl; 
    }
}