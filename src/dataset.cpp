#include <iostream>
#include <vector>
#include <random>

using namespace std;

// Generate synthetic dataset
vector<double> generateDataset(int size, int seed = 42)
{
    // Create a vector to store the dataset
    vector<double> dataset;

    // Reserve memory for better performance
    dataset.reserve(size);

    // Initialize random number generator with the specified seed
    mt19937_64 rng(seed);

    // Generate random double values between 0.0 and 10000.0
    uniform_real_distribution<double> dist(0.0, 10000.0);

    // Generate the dataset
    for (int i = 0; i < size; i++)
    {
        dataset.push_back(dist(rng));
    }

    // Return the generated dataset
    return dataset;
}