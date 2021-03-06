#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

void run_mutex(int N) {
  using namespace std;
  using namespace std::chrono;

  mutex mtx;
  long count = 0;
  auto task = [&](int n, long v) { while( n-- ) { mtx.lock(); count += v; mtx.unlock(); }}; 

  auto start = high_resolution_clock::now();
  thread inc(task, N, 1L);
  thread dec(task, N,-1L);
  
  inc.join(); dec.join();
  auto stop = high_resolution_clock::now();

  cout << "mutex:  " 
       << duration_cast<milliseconds>(stop - start).count() << "[ms]"
          " count = " << count << endl;

}

void run_atomic(int N) {
  using namespace std;
  using namespace std::chrono;

  atomic<long> count = 0;
  auto task = [&](int n, long v) { while( n-- ) count += v; }; 

  auto start = high_resolution_clock::now();
  thread inc(task, N, 1L);
  thread dec(task, N,-1L);
  
  inc.join(); dec.join();
  auto stop = high_resolution_clock::now();

  cout << "atomic: "
       << duration_cast<milliseconds>(stop - start).count() << "[ms]"
          " count = " << count << endl;

}

int main() {
  const int N = 10000000;
  run_mutex(N);
  run_atomic(N);
}
