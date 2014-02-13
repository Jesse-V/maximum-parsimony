
#ifndef MAIN
#define MAIN

#include <algorithm>
#include <string>

typedef std::vector<unsigned long> List;

class Node
{
    public:
        Node(const List& sequence, Node* left, Node* right) :
            sequence_(sequence), left_(left), right_(right)
        {}

        List sequence_;
        Node* left_ = NULL;
        Node* right_ = NULL;
};

List getSequence(std::size_t length);
std::pair<List, int> score(const List& a, const List& b);
void printTree(const Node* node, int depth);
std::mt19937 getMersenneTwister();

#endif
