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

    ofstream file("seq_log.csv");
    file << "Task,Time(ms)\n";

    vector<double> a = generate(N);
    vector<double> b = generate(N);

    double s, e;

    // 1 STATS
    s = timeNow();
    double mean, minV, maxV, stdv;
    stats(a, mean, minV, maxV, stdv);
    e = timeNow();
    file << "Stats," << e - s << "\n";

    // 2 HISTOGRAM
    s = timeNow();
    auto h = histogram(a);
    e = timeNow();
    file << "Histogram," << e - s << "\n";

    // 3 SORT
    s = timeNow();
    sortData(a);
    e = timeNow();
    file << "Sort," << e - s << "\n";

    // 4 CORR
    s = timeNow();
    double c = corr(a, b);
    e = timeNow();
    file << "Correlation," << e - s << "\n";

    // 5 MOVING AVG
    s = timeNow();
    auto m = movingAvg(a);
    e = timeNow();
    file << "MovingAvg," << e - s << "\n";

    // 6 OUTLIERS
    s = timeNow();
    auto o = outliers(a, mean, stdv);
    e = timeNow();
    file << "Outliers," << e - s << "\n";

    file.close();

    cout << "Sequential done\n";
}
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

    ofstream file("seq_log.csv");
    file << "Task,Time(ms)\n";

    vector<double> a = generate(N);
    vector<double> b = generate(N);

    double s, e;

    // 1 STATS
    s = timeNow();
    double mean, minV, maxV, stdv;
    stats(a, mean, minV, maxV, stdv);
    e = timeNow();
    file << "Stats," << e - s << "\n";

    // 2 HISTOGRAM
    s = timeNow();
    auto h = histogram(a);
    e = timeNow();
    file << "Histogram," << e - s << "\n";

    // 3 SORT
    s = timeNow();
    sortData(a);
    e = timeNow();
    file << "Sort," << e - s << "\n";

    // 4 CORR
    s = timeNow();
    double c = corr(a, b);
    e = timeNow();
    file << "Correlation," << e - s << "\n";

    // 5 MOVING AVG
    s = timeNow();
    auto m = movingAvg(a);
    e = timeNow();
    file << "MovingAvg," << e - s << "\n";

    // 6 OUTLIERS
    s = timeNow();
    auto o = outliers(a, mean, stdv);
    e = timeNow();
    file << "Outliers," << e - s << "\n";

    file.close();

    cout << "Sequential done\n";
}