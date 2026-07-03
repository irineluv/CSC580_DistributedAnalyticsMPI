#include <mpi.h>
#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include <algorithm>
#include <chrono>
#include "dataset.cpp"

using namespace std;


// Main Program

int main(int argc,char* argv[])
{

    MPI_Init(&argc,&argv);

    int rank;
    int worldSize;

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&worldSize);

    int datasetSize=1000000;

    if(argc < 2)
{
    if(rank == 0)
        cout << "Usage: mpiexec -n <processes> mpi_analytics <dataset_size>" << endl;

    MPI_Finalize();
    return 1;
}

datasetSize = stoi(argv[1]);

    vector<double> dataset;

    if(rank==0)
    {
        cout<<"======================================"<<endl;
        cout<<" Distributed Analytics using MPI"<<endl;
        cout<<"======================================"<<endl;

        cout<<"Total Processes : "<<worldSize<<endl;
        cout<<"Dataset Size    : "<<datasetSize<<endl;

        dataset=generateDataset(datasetSize);

        cout<<"Dataset Generated Successfully"<<endl;
    }

    // Broadcast dataset size

    MPI_Bcast(&datasetSize,
              1,
              MPI_INT,
              0,
              MPI_COMM_WORLD);

    // Divide dataset

   int localSize = datasetSize / worldSize;

    vector<double> localData(localSize);

// Master calculates workload distribution

if(rank == 0)
{
    cout << "\n========== JOB SCHEDULER ==========" << endl;

    for(int i = 0; i < worldSize; i++)
    {
        int startIndex = i * localSize;

        int endIndex;

        if(i == worldSize - 1)
            endIndex = datasetSize - 1;
        else
            endIndex = startIndex + localSize - 1;

        cout << "Process "
             << i
             << " -> Data Index ["
             << startIndex
             << " - "
             << endIndex
             << "]"
             << endl;
    }

    cout << "===================================\n" << endl;
}

    // Synchronize before timing

    MPI_Barrier(MPI_COMM_WORLD);

    double start=MPI_Wtime();

    // Scatter dataset from Master to all processes

    MPI_Scatter(
        dataset.data(),          // Send buffer (Master only)
        localSize,               // Number of elements for each process
        MPI_DOUBLE,
        localData.data(),        // Receive buffer
        localSize,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD
    );

    // Each process calculates local statistics


    double localSum = 0.0;
    double localMin = localData[0];
    double localMax = localData[0];

    for(double value : localData)
    {
        localSum += value;

        if(value < localMin)
            localMin = value;

        if(value > localMax)
            localMax = value;
    }

    // Local mean (for display/debugging)
    double localMean = localSum / localSize;

    vector<double> processMean(worldSize);
 
    // Gather local means to Master
    MPI_Gather(
        &localMean,
        1,
        MPI_DOUBLE,
        processMean.data(),
        1,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD
    );

    MPI_Barrier(MPI_COMM_WORLD);

    for(int i = 0; i < worldSize; i++)
    {
        if(rank == i)
        {
            cout << "Process "
                << rank
                << " processed "
                << localSize
                << " values."
                << endl;
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }
    // Combine results from all processes
  
    // Master Result Variables

        double globalSum = 0.0;
        double globalMin = 0.0;
        double globalMax = 0.0;

        

    MPI_Reduce(
        &localSum,
        &globalSum,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    MPI_Reduce(
        &localMin,
        &globalMin,
        1,
        MPI_DOUBLE,
        MPI_MIN,
        0,
        MPI_COMM_WORLD
    );

    MPI_Reduce(
        &localMax,
        &globalMax,
        1,
        MPI_DOUBLE,
        MPI_MAX,
        0,
        MPI_COMM_WORLD
    );

    // Stop timer

    MPI_Barrier(MPI_COMM_WORLD);

    double end = MPI_Wtime();

    // Master prints final result
    
    if(rank == 0)
    {
        double globalMean = globalSum / datasetSize;

        cout << "\n========== PROCESS SUMMARY ==========" << endl;

        for(int i = 0; i < worldSize; i++)
        {
            cout << "Process "
                << i
                << " Local Mean : "
                << processMean[i]
                << endl;
        }

        cout << "=====================================\n" << endl;

        cout << endl;
        cout << "======================================" << endl;
        cout << " FINAL ANALYTICS RESULT" << endl;
        cout << "======================================" << endl;

        cout << "Global Mean : " << globalMean << endl;
        cout << "Global Min  : " << globalMin << endl;
        cout << "Global Max  : " << globalMax << endl;

        cout << endl;

        cout << "MPI Execution Time : "
            << (end - start)
            << " seconds"
            << endl;
    }

    // Finish MPI

    MPI_Finalize();

    return 0;
}