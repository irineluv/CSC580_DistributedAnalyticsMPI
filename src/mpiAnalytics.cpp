#include <mpi.h>
#include <cmath>
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

    vector<double> datasetX;
    vector<double> datasetY;

    if(rank==0)
    {
        cout<<"======================================"<<endl;
        cout<<" Distributed Analytics using MPI"<<endl;
        cout<<"======================================"<<endl;

        cout<<"Total Processes : "<<worldSize<<endl;
        cout<<"Dataset Size    : "<<datasetSize<<endl;

        datasetX = generateDataset(datasetSize, 42);
        datasetY = generateDataset(datasetSize, 99);

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

    vector<double> localX(localSize);
    vector<double> localY(localSize);

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

    // Scatter Dataset X
    MPI_Scatter(
        datasetX.data(),
        localSize,
        MPI_DOUBLE,
        localX.data(),
        localSize,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD
    );

    // Scatter Dataset Y
    MPI_Scatter(
        datasetY.data(),
        localSize,
        MPI_DOUBLE,
        localY.data(),
        localSize,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD
    );
    // Each process calculates local statistics

//------------------------------------------------------
// Local Statistics
//------------------------------------------------------

    double localSum = 0.0;
    double localMin = localX[0];
    double localMax = localX[0];

    for(double value : localX)
    {
        localSum += value;

        if(value < localMin)
            localMin = value;

        if(value > localMax)
            localMax = value;
    }

    // Local Mean
    double localMean = localSum / localSize;

    //------------------------------------------------------
    // Local Histogram
    //------------------------------------------------------

    const int bins = 10;

    vector<int> localHistogram(bins, 0);

    for(double value : localX)
    {
        int index = min((int)(value / 10000.0 * bins), bins - 1);
        localHistogram[index]++;
    }

    //------------------------------------------------------
    // Pearson Correlation (Local)
    //------------------------------------------------------

    double localSumX = 0.0;
    double localSumY = 0.0;
    double localSumXY = 0.0;
    double localSumX2 = 0.0;
    double localSumY2 = 0.0;

    for(int i = 0; i < localSize; i++)
    {
        localSumX += localX[i];
        localSumY += localY[i];

        localSumXY += localX[i] * localY[i];

        localSumX2 += localX[i] * localX[i];

        localSumY2 += localY[i] * localY[i];
    }

    //------------------------------------------------------
    // Local Variance
    //------------------------------------------------------

    double localVariance = 0.0;

    for(double value : localX)
    {
        localVariance += (value - localMean) * (value - localMean);
    }


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
        double globalSumX = 0.0;
        double globalSumY = 0.0;
        double globalSumXY = 0.0;
        double globalSumX2 = 0.0;
        double globalSumY2 = 0.0;
        double globalMin = 0.0;
        double globalMax = 0.0;
        double globalVariance = 0.0;
        int globalOutliers = 0;
        double globalMean = 0.0;
        double globalStdDev = 0.0;
        

        vector<int> globalHistogram(bins, 0);

        

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

    MPI_Reduce(
        &localVariance,
        &globalVariance,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    MPI_Reduce(
        localHistogram.data(),
        globalHistogram.data(),
        bins,
        MPI_INT,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    MPI_Reduce(&localSumX, &globalSumX, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Reduce(&localSumY, &globalSumY, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Reduce(&localSumXY, &globalSumXY, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Reduce(&localSumX2, &globalSumX2, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Reduce(&localSumY2, &globalSumY2, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);


    // Stop timer

    MPI_Barrier(MPI_COMM_WORLD);

    double end = MPI_Wtime();

    // Master prints final result
    
    if(rank == 0)
    {
        globalMean = globalSum / datasetSize;

        globalVariance /= datasetSize;

        globalStdDev = sqrt(globalVariance);
    }

        //------------------------------------------------------
        // Broadcast Global Statistics
        //------------------------------------------------------

        MPI_Bcast(&globalMean,
                1,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

        MPI_Bcast(&globalStdDev,
                1,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    //------------------------------------------------------
    // Local Outlier Detection (All Processes)
    //------------------------------------------------------

    int localOutliers = 0;

    for(double value : localX)
    {
        double z = (value - globalMean) / globalStdDev;

        if(fabs(z) > 3.0)
        {
            localOutliers++;
        }
    }

    MPI_Reduce(
        &localOutliers,
        &globalOutliers,
        1,
        MPI_INT,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    if(rank == 0)
    {
                cout << "\n========== PROCESS SUMMARY ==========" << endl;

        double numerator =
            datasetSize * globalSumXY -
            globalSumX * globalSumY;

        double denominator =
            sqrt(
                (datasetSize * globalSumX2 - globalSumX * globalSumX) *
                (datasetSize * globalSumY2 - globalSumY * globalSumY)
            );

        double correlation = numerator / denominator;
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
        cout << "Global Standard Deviation : "<< globalStdDev << endl;
        cout << "Pearson Correlation       : "<< correlation << endl;
        cout << "Outliers Detected        : " << globalOutliers << endl;
        cout << "\n========== HISTOGRAM ==========" << endl;
        for(int i = 0; i < bins; i++)
        {
            cout << "Bin "
                << i + 1
                << " : "
                << globalHistogram[i]
                << endl;
        }

        cout << "===============================" << endl;
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