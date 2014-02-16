
#include "main.hpp"
#include <algorithm>
#include <string>
#include <iostream>
#include <limits>


const std::size_t SEQUENCE_COUNT = 8;
const std::size_t SEQUENCE_LENGTH = 16;


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
                uint8_t* blank;
                std::pair<uint8_t*, int> best = std::make_pair(blank,
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



std::pair<uint8_t*, int> score(const Node* nodeA, const Node* nodeB)
{
    uint8_t* result = new uint8_t[SEQUENCE_LENGTH];
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



std::pair<__m128i*, int> scoreSSE(const Node* nodeA, const Node* nodeB)
{
    static __m128i zero = _mm_set1_ps(0.f), comp, tempOr, a, b, r;
    static uint8_t* aUnit = reinterpret_cast<uint8_t*>(&a);
    static uint8_t* bUnit = reinterpret_cast<uint8_t*>(&b);
    static uint8_t* rUnit = reinterpret_cast<uint8_t*>(&r);

    int score = 0;
    __m128i* result = new __m128i[SEQUENCE_LENGTH / 16];
    for (int j = 0; j < SEQUENCE_LENGTH - 15; j += 16)
    {
        for (int k = 0; k < 16; k++)
        {
            aUnit[k] = nodeA->sequence_[j + k];
            bUnit[k] = nodeB->sequence_[j + k];
        }

        r = _mm_and_si128(a, b);
        for (int k = 0; k < 16; k++)
            score += !(int)rUnit[k];

        tempOr = _mm_or_si128(a, b);
        comp = _mm_cmpeq_epi8(zero, r);
        comp = _mm_and_si128(comp, tempOr);
        result[j / 16] = _mm_or_si128(comp, r);
    }

    return std::make_pair(result, score);
}



uint8_t* getSequence()
{
    auto mersenneTwister = getMersenneTwister();
    std::uniform_int_distribution<int> randomInt(1, 4);

    uint8_t* sequence = new uint8_t[SEQUENCE_LENGTH];
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
