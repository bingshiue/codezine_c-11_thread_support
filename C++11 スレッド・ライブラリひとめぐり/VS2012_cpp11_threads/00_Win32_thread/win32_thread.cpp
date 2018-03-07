#include <iostream>
#include <tuple>
#include <vector>
#include <Windows.h>
#include <chrono>

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

/* Win32 thread ÇÃÇΩÇﬂÇÃwrapper */

// <0>:loi, <1>:hi , <1>:result
typedef tuple<int,int,int> thread_io;

DWORD WINAPI thread_entry(LPVOID argv) {
  thread_io& io = *static_cast<thread_io*>(argv);
  get<2>(io) = count_prime(get<0>(io), get<1>(io));
  return 0;
}

/* 
 * Mñ¢ñûÇÃëfêîÇÕÇ¢Ç≠Ç¬Ç†ÇÈ?
 */
void single(int M) {
  chrono::system_clock::time_point start = chrono::system_clock::now();
  int result = count_prime(2,M);
  chrono::duration<double> sec = chrono::system_clock::now() - start;
  cout << result << ' ' << sec.count() << "[sec]" << endl;
}

void multi(int M, int nthr) {
  vector<HANDLE> handle(nthr);
  vector<thread_io> io(nthr);
  div_range<> rng(2,M,nthr);

  for ( int i = 0; i< nthr; ++i ) {
    io[i] = thread_io(rng.lo(i), rng.hi(i), 0);
  }

  chrono::system_clock::time_point start = chrono::system_clock::now();
  for ( int i = 0; i< nthr; ++i ) {
    handle[i] = CreateThread(NULL, 0, &thread_entry, &io[i], 0, NULL);
  }
  WaitForMultipleObjects(nthr, &handle[0], TRUE, INFINITE);
  chrono::duration<double> sec = chrono::system_clock::now() - start;

  int result = 0;
  for ( int i = 0; i < nthr; ++i ) {
    CloseHandle(handle[i]);
    result += get<2>(io[i]);
  }
  cout << result << ' ' << sec.count() << "[sec] : " << nthr << endl;
}

int main() {
  const int M = 100000;
  single(M);
  for ( int i = 1; i < 10; ++i ) multi(M, i);
}
