
#ifndef MAIN
#define MAIN

#include <algorithm>
#include <string>
#include <xmmintrin.h>

typedef unsigned long* List;

class Node
{
    public:
        Node(List sequence, Node* left, Node* right) :
            sequence_(sequence), left_(left), right_(right)
        {
            sequenceSSE_ = reinterpret_cast<__m128*>(&sequence_);
        }

        List sequence_;
        __m128* sequenceSSE_;
        Node* left_ = NULL;
        Node* right_ = NULL;
};

List getSequence();
std::pair<List, int> score(const List& a, const List& b);
std::pair<List, int> scoreSSE(const List& a, const List& b);
void printTree(const Node* node, int depth);
std::mt19937 getMersenneTwister();

#endif
