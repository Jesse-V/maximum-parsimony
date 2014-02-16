
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
        {
            //sequenceSSE_ = reinterpret_cast<__m128i*>(&sequence_);
        }

        uint8_t* sequence_;
        //__m128i* sequenceSSE_;
        Node* left_ = NULL;
        Node* right_ = NULL;
};

uint8_t* getSequence();
std::pair<uint8_t*, int> score(const Node* nodeA, const Node* nodeB);
std::pair<__m128i*, int> scoreSSE(const Node* nodeA, const Node* nodeB);
void printTree(const Node* node, int depth);
std::mt19937 getMersenneTwister();

#endif
