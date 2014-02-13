
#include "main.hpp"
#include <string>
#include <iostream>


const std::size_t SEQUENCE_COUNT = 5;
const std::size_t SEQUENCE_LENGTH = 8;


int main(int argc, char** argv)
{
    std::vector<Node> sequences;
    for (std::size_t j = 0; j < SEQUENCE_COUNT; j++)
        sequences.push_back(Node(getSequence(SEQUENCE_LENGTH)));

    while (sequences.size() > 1)
    {
        Node a = sequences.back();
        sequences.pop_back();

        Node b = sequences.back();
        sequences.pop_back();

        //std::cout << a.score_ << "," << b.score_ << std::endl;

        std::vector<short> newSequence(SEQUENCE_LENGTH, 0);
        int newScore = a.score_;
        for (std::size_t j = 0; j < SEQUENCE_LENGTH; j++)
        {
            auto aAndb = a.sequence_[j] & b.sequence_[j];
            if (aAndb == 0)
            {
                newSequence[j] = a.sequence_[j] | b.sequence_[j];
                newScore++;
            }
            else
                newSequence[j] = aAndb;
        }

        sequences.push_back(Node(newSequence, newScore));
    }

    for (std::size_t j = 0; j < SEQUENCE_LENGTH; j++)
        std::cout << sequences[0].sequence_[j] << " ";
    std::cout << std::endl << "Score: " << sequences[0].score_ << std::endl;

    return EXIT_SUCCESS;
}



std::vector<short> getSequence(std::size_t length)
{
    auto mersenneTwister = getMersenneTwister();
    std::uniform_int_distribution<int> randomInt(1, 4);

    std::vector<short> sequence;
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
/*


std::string getDNA(std::size_t length)
{
    auto mersenneTwister = getMersenneTwister();
    std::uniform_int_distribution<int> randomInt(1, 4);

    std::string dna(length, 'A');

    for (std::size_t j = 0; j < length; j++)
    {
        switch (randomInt(mersenneTwister))
        {
            case 1:
                dna[j] = 'A';
                break;
            case 2:
                dna[j] = 'T';
                break;
            case 3:
                dna[j] = 'C';
                break;
            case 4:
                dna[j] = 'G';
                break;
        }
    }

    return dna;
}



std::string toNumerical(const std::string& str)
{
    std::string converted(str);

    for (std::size_t j = 0; j < str.size(); j++)
    {
        switch (str[j])
        {
            case 'A' :
                converted[j] = 0x1;
                break;
            case 'T' :
                converted[j] = 0x2;
                break;
            case 'C' :
                converted[j] = 0x4;
                break;
            case 'G' :
                converted[j] = 0x8;
                break;
        }
    }

    return converted;
}
*/


std::mt19937 getMersenneTwister()
{
    std::array<int, std::mt19937::state_size> seed_data;
    std::random_device r;
    std::generate_n(seed_data.data(), seed_data.size(), std::ref(r));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
    std::mt19937 mersenneTwister(seq);
    return mersenneTwister;
}
