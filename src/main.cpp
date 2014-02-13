
#include "main.hpp"
#include <string>
#include <iostream>
#include <limits>


const std::size_t SEQUENCE_COUNT = 256;
const std::size_t SEQUENCE_LENGTH = 30;


int main(int argc, char** argv)
{
    std::vector<Node> nodes;
    for (std::size_t j = 0; j < SEQUENCE_COUNT; j++)
        nodes.push_back(Node(getSequence(SEQUENCE_LENGTH), NULL, NULL));

    while (nodes.size() > 1)
    {
        std::vector<Node> newList;

        while (!nodes.empty())
        {
            if (nodes.size() == 1)
            {
                newList.push_back(Node(nodes[0].sequence_, &nodes[0], NULL));
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
                    auto result = score(nodes[0].sequence_, nodes[j].sequence_);
                    if (result.second < best.second)
                    {
                        best = result;
                        bestIndex = j;
                    }
                }

                //std::cout << best.second << std::endl;

                newList.push_back(Node(best.first, &nodes[0], &nodes[bestIndex]));
                nodes.erase(nodes.begin() + (const long)bestIndex);
                nodes.erase(nodes.begin());
            }
        }

        nodes = newList;
    }

    std::cout << score(nodes[0].left_->sequence_, nodes[0].right_->sequence_).second << std::endl;
    std::cout << nodes.size() << std::endl;

    printTree(&nodes[0], 0);

    return EXIT_SUCCESS;
}



std::pair<List, int> score(const List& a, const List& b)
{
    List result(SEQUENCE_LENGTH, 0);
    int score = 0;

    for (std::size_t j = 0; j < SEQUENCE_LENGTH; j++)
    {
        auto aAndb = a[j] & b[j];
        if (aAndb == 0)
        {
            result[j] = a[j] | b[j];
            score++;
        }
        else
            result[j] = aAndb;
    }

    return std::make_pair(result, score);
}



List getSequence(std::size_t length)
{
    auto mersenneTwister = getMersenneTwister();
    std::uniform_int_distribution<int> randomInt(1, 4);

    List sequence;
    for (std::size_t j = 0; j < length; j++)
    {
        switch (randomInt(mersenneTwister))
        {
            case 1:
                sequence.push_back(0x1);
                break;
            case 2:
                sequence.push_back(0x2);
                break;
            case 3:
                sequence.push_back(0x4);
                break;
            case 4:
                sequence.push_back(0x8);
                break;
        }
    }

    return sequence;
}



void printTree(const Node* node, int depth)
{
    if (node == NULL)
        return;

    std::cout << depth << std::endl;

    printTree(node->left_, depth + 1);

    for (int j = 0; j < depth; j++)
        std::cout << " ";
    for (std::size_t j = 0; j < SEQUENCE_LENGTH; j++)
        std::cout << node->sequence_[j];
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
