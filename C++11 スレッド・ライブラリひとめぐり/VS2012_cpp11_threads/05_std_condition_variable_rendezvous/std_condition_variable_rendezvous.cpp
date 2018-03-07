#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

#include "div_range.h"
#include "rendezvous.h"

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
  atomic<int> result = 0;
  rendezvous quit(nthr+1);

  chrono::system_clock::time_point start = chrono::system_clock::now();
  for ( int t = 0; t < nthr; ++t ) {
    thr[t] = thread([&](int lo, int hi) { 
                      for ( int n = lo; n < hi; ++n ) {
                        if ( is_prime(n) ) ++result; 
                      }
                      quit.wait();
                    },
                    rng.lo(t), rng.hi(t));
  }
  quit.wait();
  chrono::duration<double> sec = chrono::system_clock::now() - start;

  cout << result << ' ' << sec.count() << "[sec] : " << nthr << endl;
  for ( thread& th : thr ) { th.join(); }
}

int main() {
  const int M = 100000;
  for ( int i = 1; i < 10; ++i ) multi(M, i);
}
