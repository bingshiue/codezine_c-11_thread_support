#include <iostream>
#include <random>
#include <thread>
#include <future>
#include <numeric>
#include <vector>
#include <chrono>

int main() {
  using namespace std;
  using namespace std::chrono;

  const int N = 1000000;
  vector<double> data(N);

  mt19937 gen; // メルセンヌ・ツイスター
  normal_distribution<double> dist; // 正規分布(平均:0,標準偏差:1)
  auto rand = [&]() { return dist(gen); };
  generate_n(begin(data), N, rand);

  double sum;
  long long duration;
  high_resolution_clock::time_point start;
  high_resolution_clock::time_point stop;
  
  // single-thread
  start = high_resolution_clock::now();
  sum = accumulate(begin(data), end(data), 0.0);
  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start).count();
  cout << sum << " in " << duration << "[us]\n";

  // 2-threads
  start = high_resolution_clock::now();
  future<double> f0 = async([&]() { return accumulate(begin(data),     begin(data)+N/2, 0.0); });
  future<double> f1 = async([&]() { return accumulate(begin(data)+N/2, begin(data)+N  , 0.0); });
  sum = f0.get() + f1.get();
  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start).count();
  cout << sum << " in " << duration << "[us]\n";
}
