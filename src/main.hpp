
#ifndef MAIN
#define MAIN

#include <algorithm>
#include <string>
#include <xmmintrin.h>

class Node
{
    public:
        Node(uint8_t* sequence, Node* left, Node* right) :
            sequence_(sequence), left_(left), right_(right)
        {}

        uint8_t* sequence_;
        Node* left_ = NULL;
        Node* right_ = NULL;
};

uint8_t* getSequence();
std::pair<uint8_t*, int> score(const Node* nodeA, const Node* nodeB);
std::pair<uint8_t*, int> scoreSSE(const Node* nodeA, const Node* nodeB);
void printTree(const Node* node, int depth);
std::mt19937_64 getMersenneTwister();

#endif
