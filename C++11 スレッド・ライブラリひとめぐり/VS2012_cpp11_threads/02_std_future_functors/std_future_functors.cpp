#include <iostream>
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
class count_prime_class {
public:
  int operator()(int lo, int hi) { return count_prime(lo, hi); }
};

void multi(int M) {
  future<int> fut[3];
  div_range<> rng(2,M,3);

  auto count_prime_lambda = [&](int lo, int hi) { return count_prime(lo,hi); };

  chrono::system_clock::time_point start = chrono::system_clock::now();
  // 関数ポインタ, クラスインスタンス, lambda式からfutureを作る
  fut[0] = async(count_prime,         rng.lo(0), rng.hi(0));
  fut[1] = async(count_prime_class(), rng.lo(1), rng.hi(1));
  fut[2] = async(count_prime_lambda,  rng.lo(2), rng.hi(2));
  int result = fut[0].get() + fut[1].get() + fut[2].get();
  chrono::duration<double> sec = chrono::system_clock::now() - start;

  cout << result << sec.count() << "[sec]" << endl;
}

int main() {
  const int M = 100000;
  multi(M);
}
