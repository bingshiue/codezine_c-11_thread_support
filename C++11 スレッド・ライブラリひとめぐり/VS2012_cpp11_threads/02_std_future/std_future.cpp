#include <iostream>
#include <thread>
#include <future>
#include <chrono>

#include "div_range.h"

using namespace std;

// nは素数?
bool is_prime(int n) {
  for ( int i = 2; i < n; ++i ) {
    if ( n % i == 0 ) {
      return false;
    }
  }
  return true;
}

// lo以上 hi未満 の中に素数はいくつある?
int count_prime(int lo, int hi) {
  int result = 0;
  for ( int i = lo; i < hi; ++i ) {
    if ( is_prime(i) ) {
      ++result;
    }
  }
  return result;
}

/* 
 * M未満の素数はいくつある?
 */
void multi(int M, int nthr) {
  vector<future<int>> fut(nthr);
  div_range<> rng(2,M,nthr);

  chrono::system_clock::time_point start = chrono::system_clock::now();
  for ( int i = 0; i< nthr; ++i ) {
    fut[i] = async(count_prime, rng.lo(i), rng.hi(i));
  }
  int result = 0;
  for ( future<int>& f : fut ) {
    result += f.get();
  }
  chrono::duration<double> sec = chrono::system_clock::now() - start;

  cout << result << ' ' << sec.count() << "[sec] : " << nthr << endl;
}

int main() {
  const int M = 100000;
  for ( int i = 1; i < 10; ++i ) multi(M, i);
}
