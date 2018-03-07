#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <utility> // ref

#include "div_range.h"

using namespace std;

// nÇÕëfêî?
bool is_prime(int n) {
  for ( int i = 2; i < n; ++i ) {
    if ( n % i == 0 ) {
      return false;
    }
  }
  return true;
}

/* 
 * Mñ¢ñûÇÃëfêîÇÕÇ¢Ç≠Ç¬Ç†ÇÈ?
 */

void multi(int M, int nthr) {
  vector<thread> thr(nthr);
  div_range<> rng(2,M,nthr);

  mutex mtx;
  int result = 0;

  chrono::system_clock::time_point start = chrono::system_clock::now();
  for ( int t = 0; t < nthr; ++t ) {
    thr[t] = thread([&](int lo, int hi) {
                      for ( int n = lo; n < hi; ++n ) {
                        if ( is_prime(n) ) {
                          lock_guard<mutex> lock(mtx);
                          ++result;
                        }
                      }
                    }, 
                    rng.lo(t), rng.hi(t));
  }
  for ( thread& th : thr ) { th.join(); }
  chrono::duration<double> sec = chrono::system_clock::now() - start;

  cout << result << ' ' << sec.count() << "[sec] : " << nthr <<  endl;
}

int main() {
  const int M = 100000;
  for ( int i = 1; i < 10; ++i ) multi(M, i);
}
