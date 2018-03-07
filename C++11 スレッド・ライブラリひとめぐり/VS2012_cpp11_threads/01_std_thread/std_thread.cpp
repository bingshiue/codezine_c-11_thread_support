#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

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

// loà»è„ hiñ¢ñû ÇÃíÜÇ…ëfêîÇÕÇ¢Ç≠Ç¬Ç†ÇÈ?
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
 * Mñ¢ñûÇÃëfêîÇÕÇ¢Ç≠Ç¬Ç†ÇÈ?
 */

void multi(int M, int nthr) {
  vector<thread> thr(nthr);
  vector<int> count(nthr);
  div_range<> rng(2,M,nthr);

  chrono::system_clock::time_point start = chrono::system_clock::now();
  for ( int i = 0; i < nthr; ++i ) {
    thr[i] = thread([&,i](int lo, int hi) { count[i] = count_prime(lo,hi); }, 
                    rng.lo(i), rng.hi(i));
  }
  int result = 0;
  for ( int i = 0; i < nthr; ++i ) {
    thr[i].join();
    result += count[i];
  }
  chrono::duration<double> sec = chrono::system_clock::now() - start;

  cout << result << ' ' << sec.count() << "[sec] : " << nthr << endl;
}

int main() {
  const int M = 100000;
  for ( int i = 1; i < 10; ++i ) multi(M, i);
}
