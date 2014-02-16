
#include "main.hpp"
#include <algorithm>
#include <chrono>
#include <string>
#include <iostream>
#include <limits>
#include <memory.h>


const std::size_t SEQUENCE_COUNT = 256;
const std::size_t SEQUENCE_LENGTH = 2048;
const int UNITS_PER_SSE = 16;
const int NUMBER_OF_TESTS = 200000;


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
                                            std::numeric_limits<int>::min());
                std::size_t bestIndex = 0;
                for (std::size_t j = 1; j < nodes.size(); j++)
                {
                    auto result = score(nodes[0], nodes[j]);
                    if (result.second > best.second)
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

    std::cout << "In-order traversal of tree:" << std::endl;
    printTree(nodes[0], 1);
    std::cout << "End of in-order traversal of tree." << std::endl;

    std::cout << "Score between first two children (traditional): " <<
        score(nodes[0]->left_, nodes[0]->right_).second <<
        std::endl;

    std::cout << "Score between first two children (SSE optimized): " <<
        scoreSSE(nodes[0]->left_, nodes[0]->right_).second <<
        std::endl;

    using namespace std::chrono;
    std::cout << "Testing scoring " << NUMBER_OF_TESTS << " times:" << std::endl;

    std::cout << "Testing speed of traditional scoring... ";
    auto start1 = steady_clock::now();
    for (int j = 0; j < NUMBER_OF_TESTS; j++)
        score(nodes[0]->left_, nodes[0]->right_);
    auto diff1 = duration_cast<microseconds>(steady_clock::now() - start1).count();
    std::cout << diff1 << " microseconds" << std::endl;

    std::cout << "Testing speed of SSE-optimized scoring... ";
    auto start2 = steady_clock::now();
    for (int j = 0; j < NUMBER_OF_TESTS; j++)
        scoreSSE(nodes[0]->left_, nodes[0]->right_);
    auto diff2 = duration_cast<microseconds>(steady_clock::now() - start2).count();
    std::cout << diff2 << " microseconds" << std::endl;

    std::cout << "Speedup from SSE: " << (diff2 * 100 / (float)diff1) << "%" << std::endl;

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



std::pair<uint8_t*, int> scoreSSE(const Node* nodeA, const Node* nodeB)
{
    static __m128i zero = _mm_set1_ps(0.f), comp, aOrB, a, b, r;
    static uint8_t* rUnit = reinterpret_cast<uint8_t*>(&r);

    int score = 0;
    __m128i* result = new __m128i[SEQUENCE_LENGTH / UNITS_PER_SSE];
    for (int j = 0; j <= SEQUENCE_LENGTH - UNITS_PER_SSE; j += UNITS_PER_SSE)
    {
        memcpy(&a, &nodeA->sequence_[j], sizeof(__m128i));
        memcpy(&b, &nodeB->sequence_[j], sizeof(__m128i));

        r = _mm_and_si128(a, b);
        for (int k = 0; k < UNITS_PER_SSE; k++)
            score += !(int)rUnit[k];

        aOrB = _mm_or_si128(a, b);
        comp = _mm_cmpeq_epi8(zero, r);
        comp = _mm_and_si128(comp, aOrB);
        result[j / UNITS_PER_SSE] = _mm_or_si128(comp, r);
    }

    return std::make_pair(reinterpret_cast<uint8_t*>(&result), score);
}



uint8_t* getSequence()
{
    auto mersenneTwister = getMersenneTwister();
    std::uniform_int_distribution<int> randomInt(1, 5);

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
            case 5:
                sequence[j] = 0x10;
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
    std::cout << "|-> ";

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
                case 16:
                    std::cout << "-";
                    break;
            }
        }
    }

    std::cout << std::endl;

    printTree(node->right_, depth + 1);
}



std::mt19937_64 getMersenneTwister()
{
    std::array<int, std::mt19937_64::state_size> seed_data;
    std::random_device r;
    std::generate_n(seed_data.data(), seed_data.size(), std::ref(r));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
    std::mt19937_64 mersenneTwister(seq);
    return mersenneTwister;
}
