
#ifndef MAIN
#define MAIN

#include <algorithm>
#include <string>

struct Node
{
    Node(const std::vector<short>& sequence) :
        sequence_(sequence), score_(0)
    {}

    Node(const std::vector<short>& sequence, int score) :
        sequence_(sequence), score_(score)
    {}

    std::vector<short> sequence_;
    int score_;
};

std::vector<short> getSequence(std::size_t length);
std::string getDNA(std::size_t length);
std::string toNumerical(const std::string& str);
std::mt19937 getMersenneTwister();

#endif
