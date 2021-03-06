#if 0

#include <thread>
#include <iostream>
#include <string>

int main() {
  using namespace std;
  const int Nthr = 4;
  std::thread threads[Nthr];

  auto thread_fun = [](int id, int n) {
    while ( n-- ) {
      cout << to_string(id) + ": 位置について\n";
      cout << to_string(id) + ": よーい・・・\n";
      cout << to_string(id) + ": ドン!!\n";
    }
  };

  for ( int i = 0; i < Nthr; ++i ) {
    threads[i] = std::thread(thread_fun, i, 20);
  }

  for ( auto& thr : threads ) thr.join();
}

#else
#include <stdexcept>
#include <mutex>
#include <condition_variable>

class rendezvous {
public:
  explicit rendezvous(unsigned int count) 
    : threshold_(count), count_(count), generation_(0) {
    if ( count == 0 ) throw std::invalid_argument("count cannot be zero.");
  }

  bool wait() {
    std::unique_lock<std::mutex> guard(mtx_);
    unsigned int gen = generation_;
    if ( --count_ == 0 ) {
      // ここを実行する最後にwaitしたスレッドは
      ++generation_;
      count_ = threshold_; // count_を初期値に戻して
      condition_.notify_all(); // 残る全員のwaitを解く
      return true;
    }
    // 他のスレッドは最後のスレッドが到着するのを待つ
    condition_.wait(guard, [&]() { return gen != generation_;} );
    return false;
  }

private:
  std::mutex mtx_;

  std::condition_variable condition_;
  unsigned int threshold_;
  unsigned int count_;
  unsigned int generation_;
};

#include <thread>
#include <iostream>
#include <string>

int main() {
  using namespace std;
  const int Nthr = 4;
  std::thread threads[Nthr];

  rendezvous period(Nthr);

  auto thread_fun = [&](int id, int n) {
    while ( n-- ) {
      cout << to_string(id) + ": 位置について\n";
      period.wait();
      cout << to_string(id) + ": よーい・・・\n";
      period.wait();
      cout << to_string(id) + ": ドン!!\n";
      period.wait();
    }
  };

  for ( int i = 0; i < Nthr; ++i ) {
    threads[i] = std::thread(thread_fun, i, 10);
  }

  for ( auto& thr : threads ) thr.join();
}
#endif
