#include "adjacent_iterator.h"
#include <iostream>
#include <vector>
#include <tuple>

using namespace hsng;

void test2(const std::vector<int> &v)
{
    std::cout << "2: [ ";

    for (const auto &p : make_adjacent_range<2>(v))
    {
        std::cout << std::get<0>(p) << "/" << std::get<1>(p) << " ";
    }
    std::cout << "]"
              << std::endl;
}

void test3(const std::vector<int> &v)
{

    std::cout << "3: [ ";

    for (const auto &p : make_adjacent_range<3>(v))
    {
        // std::cout << std::get<0>(p) << "/" << std::get<1>(p) << " ";
        std::cout << std::get<0>(p) << "/" << std::get<1>(p) << "/" << std::get<2>(p) << " ";
    }
    std::cout << "]"
              << std::endl;
}

int main()
{
    test2({});
    test2({11});
    test2({22, 33});
    test2({44, 55, 66});
    test2({10, 20, 30, 40});

    std::cout << std::endl;

    test3({});
    test3({11});
    test3({22, 33});
    test3({44, 55, 66});
    test3({10, 20, 30, 40});
}