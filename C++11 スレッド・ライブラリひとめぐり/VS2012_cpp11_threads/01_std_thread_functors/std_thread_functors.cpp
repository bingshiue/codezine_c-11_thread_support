#include <iostream>
#include <thread>
#include <chrono>
#include <functional> // ref

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
 void count_prime_function(int lo, int hi, int& result) {
   result = count_prime(lo, hi);
 }

class count_prime_class {
   int& result_;
public:
  count_prime_class(int& result) : result_(result) {}
  void operator()(int lo, int hi) { result_ = count_prime(lo, hi); }
};

void multi(int M) {
  thread thr[3];
  int    count[3];
  div_range<> rng(2,M,3);

  auto count_prime_lambda = [&](int lo, int hi) { count[2] = count_prime(lo,hi); };

  chrono::system_clock::time_point start = chrono::system_clock::now();
  thr[0] = thread(count_prime_function,        rng.lo(0), rng.hi(0), ref(count[0]));
  thr[1] = thread(count_prime_class(count[1]), rng.lo(1), rng.hi(1));
  thr[2] = thread(count_prime_lambda,          rng.lo(2), rng.hi(2));
  for ( thread& t : thr ) t.join();
  chrono::duration<double> sec = chrono::system_clock::now() - start;

  cout << count[0] + count[1] + count[2] << ' '
       << sec.count() << "[sec]" << endl;
}

int main() {
  const int M = 100000;
  multi(M);
}
