#include <iostream>
#include <ppl.h>
#include <atomic>
#include <chrono>

using namespace std;
using namespace Concurrency;

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
void multi(int M) {
  atomic<int> result = 0;

  chrono::system_clock::time_point start = chrono::system_clock::now();
  parallel_for(2, M, [&](int n) { if ( is_prime(n) ) ++result;});
  chrono::duration<double> sec = chrono::system_clock::now() - start;

  cout << result << ' ' << sec.count() << "[sec]" << endl;
}

int main() {
  const int M = 100000;
  multi(M);
}
