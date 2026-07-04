#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <fstream>
#include <algorithm>

using namespace std;

// ================= TIMER =================
double timeNow()
{
    return chrono::duration<double, milli>(
               chrono::high_resolution_clock::now().time_since_epoch())
        .count();
}

// ================= DATA GENERATION =================
vector<double> generate(int N)
{
    mt19937_64 rng(42);
    uniform_real_distribution<double> dist(0.0, 10000.0);

    vector<double> data(N);
    for (auto &x : data)
        x = dist(rng);

    return data;
}

// ================= TASK 1: STATS =================
void stats(vector<double> &a, double &mean, double &minV, double &maxV, double &stdv)
{
    double sum = 0;
    minV = 1e18;
    maxV = -1e18;

    for (double x : a)
    {
        sum += x;
        minV = min(minV, x);
        maxV = max(maxV, x);
    }

    mean = sum / a.size();

    double var = 0;
    for (double x : a)
        var += (x - mean) * (x - mean);

    stdv = sqrt(var / a.size());
}

// ================= TASK 2: HISTOGRAM =================
vector<int> histogram(vector<double> &a, int bins = 10)
{
    vector<int> h(bins, 0);

    for (double x : a)
    {
        int idx = min((int)(x / 10000.0 * bins), bins - 1);
        h[idx]++;
    }

    return h;
}

// ================= TASK 3: SORT =================
void sortData(vector<double> &a)
{
    sort(a.begin(), a.end());
}

// ================= TASK 4: CORRELATION =================
double corr(vector<double> &x, vector<double> &y)
{
    double mx = 0, my = 0;

    for (int i = 0; i < x.size(); i++)
    {
        mx += x[i];
        my += y[i];
    }

    mx /= x.size();
    my /= y.size();

    double num = 0, dx = 0, dy = 0;

    for (int i = 0; i < x.size(); i++)
    {
        num += (x[i] - mx) * (y[i] - my);
        dx += pow(x[i] - mx, 2);
        dy += pow(y[i] - my, 2);
    }

    return num / sqrt(dx * dy);
}

// ================= TASK 5: MOVING AVERAGE =================
vector<double> movingAvg(vector<double> &a, int w = 5)
{
    vector<double> res;

    for (int i = 0; i < a.size(); i++)
    {
        double sum = 0;
        int cnt = 0;

        for (int j = max(0, i - w + 1); j <= i; j++)
        {
            sum += a[j];
            cnt++;
        }

        res.push_back(sum / cnt);
    }

    return res;
}

// ================= TASK 6: OUTLIERS =================
vector<int> outliers(vector<double> &a, double mean, double stdv)
{
    vector<int> idx;

    for (int i = 0; i < a.size(); i++)
    {
        double z = (a[i] - mean) / stdv;
        if (abs(z) > 3)
            idx.push_back(i);
    }

    return idx;
}

// ================= MAIN =================
int main(int argc, char *argv[])
{
    int N = stoi(argv[1]);

    vector<double> a = generate(N);
    vector<double> b = generate(N);

    double mean, minV, maxV, stdv;
    double t_stats, t_hist, t_sort, t_corr, t_mov, t_out;

    // ================= 1. STATS =================
    double s = timeNow();
    stats(a, mean, minV, maxV, stdv);
    double e = timeNow();
    t_stats = e - s;

    // ================= 2. HISTOGRAM =================
    s = timeNow();
    auto h = histogram(a);
    e = timeNow();
    t_hist = e - s;

    // ================= 3. SORT =================
    s = timeNow();
    sortData(a);
    vector<double> sorted = a;
    e = timeNow();
    t_sort = e - s;

    // ================= 4. CORRELATION =================
    s = timeNow();
    double c = corr(a, b);
    e = timeNow();
    t_corr = e - s;

    // ================= 5. MOVING AVG =================
    s = timeNow();
    auto m = movingAvg(a);
    e = timeNow();
    t_mov = e - s;

    // ================= 6. OUTLIERS =================
    s = timeNow();
    auto o = outliers(a, mean, stdv);
    e = timeNow();
    t_out = e - s;

    // ================= OUTPUT FILE =================
    ofstream file("seq_log.txt");

    file << "Mean: " << mean << "\n";
    file << "Min: " << minV << "\n";
    file << "Max: " << maxV << "\n";
    file << "StdDev: " << stdv << "\n\n";

    file << "Stats: " << t_stats << " ms\n";
    file << "Histogram: " << t_hist << " ms\n";
    file << "Sort: " << t_sort << " ms\n";
    file << "Correlation: " << t_corr << " ms\n";
    file << "MovingAvg: " << t_mov << " ms\n";
    file << "Outliers: " << t_out << " ms\n";

    file.close();

    // ================= CONSOLE OUTPUT =================
    cout << "\n======================================\n";
    cout << "        SEQUENTIAL ANALYTICS RESULTS\n";
    cout << "======================================\n";

    cout << "\n[1] STATISTICS\n";
    cout << "------------------------------\n";
    cout << "Mean    : " << mean << endl;
    cout << "Min     : " << minV << endl;
    cout << "Max     : " << maxV << endl;
    cout << "Std Dev : " << stdv << endl;

    cout << "\n[2] HISTOGRAM (first 5 bins)\n";
    cout << "------------------------------\n";
    for (int i = 1; i < 10; i++)
        cout << "Bin " << i << ": " << h[i] << endl;

    cout << "\n[3] SORT (first 20 values)\n";
    cout << "------------------------------\n";
    cout << "\n";
    for (int i = 0; i < 20; i++)
    {
        cout << sorted[i] << endl;
    }

    cout << "\n[4] CORRELATION\n";
    cout << "------------------------------\n";
    cout << "Value : " << c << endl;

    cout << "\n[5] MOVING AVERAGE (first 10)\n";
    cout << "------------------------------\n";
    cout << "\n";
    for (int i = 0; i < 10; i++)
    {
        cout << m[i] << endl;
    }

    cout << "\n[6] OUTLIERS\n";
    cout << "------------------------------\n";
    cout << "Count : " << o.size() << endl;

    cout << "\n========== EXECUTION TIMES (ms) ==========\n";
    cout << "Stats       : " << t_stats << endl;
    cout << "Histogram   : " << t_hist << endl;
    cout << "Sort        : " << t_sort << endl;
    cout << "Correlation : " << t_corr << endl;
    cout << "Moving Avg  : " << t_mov << endl;
    cout << "Outliers    : " << t_out << endl;

    cout << "\nTOTAL TIME : "
         << (t_stats + t_hist + t_sort + t_corr + t_mov + t_out)
         << " ms\n";

    cout << "======================================\n";
    cout << "Sequential Done\n";

    return 0;
}