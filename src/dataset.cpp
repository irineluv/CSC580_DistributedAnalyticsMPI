#include <iostream>
#include <vector>
#include <random>

using namespace std;

// Generate synthetic numerical dataset
vector<double> generateDataset(int size)
{
    vector<double> dataset;
    dataset.reserve(size);

    mt19937_64 rng(42);
    uniform_real_distribution<double> dist(0.0, 10000.0);

    for (int i = 0; i < size; i++)
    {
        dataset.push_back(dist(rng));
    }

    return dataset;
}