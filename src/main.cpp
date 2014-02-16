
#include "main.hpp"
#include <algorithm>
#include <string>
#include <iostream>
#include <limits>


const std::size_t SEQUENCE_COUNT = 128;
const std::size_t SEQUENCE_LENGTH = 128;


int main(int argc, char** argv)
{
    std::vector<Node*> nodes;
    for (std::size_t j = 0; j < SEQUENCE_COUNT; j++)
        nodes.push_back(new Node(getSequence(), NULL, NULL));

    while (nodes.size() > 1)
    {
        std::vector<Node*> newList;

        while (!nodes.empty())
        {
            if (nodes.size() == 1)
            {
                newList.push_back(new Node(nodes[0]->sequence_, nodes[0], NULL));
                nodes.erase(nodes.begin());
            }
            else
            {
                List blank;
                std::pair<List, int> best = std::make_pair(blank,
                                            std::numeric_limits<int>::max());
                std::size_t bestIndex = 0;
                for (std::size_t j = 1; j < nodes.size(); j++)
                {
                    auto result = score(nodes[0], nodes[j]);
                    if (result.second < best.second)
                    {
                        best = result;
                        bestIndex = j;
                    }
                }

                newList.push_back(new Node(best.first, nodes[0], nodes[bestIndex]));
                nodes.erase(nodes.begin() + (const long)bestIndex);
                nodes.erase(nodes.begin());
            }
        }

        nodes = newList;
    }

    std::cout << "Comparison of first two children: " <<
        score(nodes[0]->left_, nodes[0]->right_).second <<
        std::endl;

    std::cout << "Comparison of first two children: " <<
        scoreSSE(nodes[0]->left_, nodes[0]->right_).second <<
        std::endl;

    std::cout << "In-order traversal of tree:" << std::endl;
    //printTree(nodes[0], 0);
    std::cout << "End of in-order traversal of tree." << std::endl;

    return EXIT_SUCCESS;
}



std::pair<List, int> score(const Node* nodeA, const Node* nodeB)
{
    List result = new unsigned long[SEQUENCE_LENGTH];
    int score = 0;

    for (std::size_t j = 0; j < SEQUENCE_LENGTH; j++)
    {
        auto a = nodeA->sequence_[j], b = nodeB->sequence_[j];
        auto aAndb = a & b;
        if (aAndb == 0)
        {
            result[j] = a | b;
            score++;
        }
        else
            result[j] = aAndb;
    }

    return std::make_pair(result, score);
}



std::pair<__m128*, int> scoreSSE(const Node* nodeA, const Node* nodeB)
{
    static const __m128 zero = _mm_set1_ps(0.f);
    static const auto length = SEQUENCE_LENGTH / 4;

    __m128* result = new __m128[length];
    for (std::size_t j = 0; j < length; j++)
    {
        auto a = nodeA->sequenceSSE_[j];
        auto b = nodeB->sequenceSSE_[j];

        auto aAndB = _mm_and_ps(a, b);
        auto comp  = _mm_cmpeq_ps(zero, aAndB);
        auto c     = _mm_and_ps(comp, _mm_or_ps(a, b));
        auto d     = _mm_or_ps(c, aAndB);

        result[j] = d;
    }

    auto values = reinterpret_cast<unsigned long*>(&result);

    int score = 0;
    for (int j = 0; j < SEQUENCE_LENGTH; j += 4)
    {
        score += !values[j + 0];
        score += !values[j + 1];
        score += !values[j + 2];
        score += !values[j + 3];
    }

    return std::make_pair(result, score);
}



List getSequence()
{
    auto mersenneTwister = getMersenneTwister();
    std::uniform_int_distribution<int> randomInt(1, 4);

    List sequence = new unsigned long[SEQUENCE_LENGTH];
    for (std::size_t j = 0; j < SEQUENCE_LENGTH; j++)
    {
        switch (randomInt(mersenneTwister))
        {
            case 1:
                sequence[j] = 0x1;
                break;
            case 2:
                sequence[j] = 0x2;
                break;
            case 3:
                sequence[j] = 0x4;
                break;
            case 4:
                sequence[j] = 0x8;
                break;
        }
    }

    return sequence;
}



void printTree(const Node* node, int depth)
{
    if (node == NULL)
        return;

    printTree(node->left_, depth + 1);

    for (int j = 0; j < depth - 1; j++)
        std::cout << "    ";
    std::cout << "|--";

    if (node->right_ != NULL && node->right_ != NULL)
        std::cout << score(node->left_, node->right_).second;
    else
    {
        for (std::size_t j = 0; j < std::min(100, (int)SEQUENCE_LENGTH); j++)
        {
            switch(node->sequence_[j])
            {
                case 1:
                    std::cout << "A";
                    break;
                case 2:
                    std::cout << "T";
                    break;
                case 4:
                    std::cout << "C";
                    break;
                case 8:
                    std::cout << "G";
                    break;
            }
        }
    }

    std::cout << std::endl;

    printTree(node->right_, depth + 1);
}



std::mt19937 getMersenneTwister()
{
    std::array<int, std::mt19937::state_size> seed_data;
    std::random_device r;
    std::generate_n(seed_data.data(), seed_data.size(), std::ref(r));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
    std::mt19937 mersenneTwister(seq);
    return mersenneTwister;
}
