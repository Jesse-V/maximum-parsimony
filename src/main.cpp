
#include "main.hpp"
#include <string>
#include <iostream>


const std::size_t SEQUENCE_COUNT = 500;
const std::size_t SEQUENCE_LENGTH = 3000;


int main(int argc, char** argv)
{
    std::vector<std::string> sequences;

    for (std::size_t j = 0; j < SEQUENCE_COUNT; j++)
        sequences.push_back(getDNA(SEQUENCE_LENGTH));

    for (std::size_t j = 0; j < SEQUENCE_COUNT; j++)
        sequences[j] = toNumerical(sequences[j]);




    return EXIT_SUCCESS;
}


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



std::mt19937 getMersenneTwister()
{
    std::array<int, std::mt19937::state_size> seed_data;
    std::random_device r;
    std::generate_n(seed_data.data(), seed_data.size(), std::ref(r));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
    std::mt19937 mersenneTwister(seq);
    return mersenneTwister;
}

