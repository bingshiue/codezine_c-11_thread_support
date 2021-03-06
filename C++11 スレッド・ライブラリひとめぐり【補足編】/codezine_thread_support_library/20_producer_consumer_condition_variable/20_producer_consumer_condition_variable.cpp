template<typename T>
class box {
private:
  T value_;    // ハコのナカミ
  bool empty_; // 空か?
public:
  box() : empty_(true) {}
  box(const T& value) : value_(value), empty_(false) {}
  bool empty() const    { return  empty_; } // 空ならtrue
  bool occupied() const { return !empty_; } // 空でないならtrue
  void set(const T& value) { empty_ = false; value_ = value; }
  const T& get()           { empty_ = true;  return value_;  }
};

#include <mutex>
#include <condition_variable>

template<typename T>
class concurrent_box {
private:
  box<T> box_;    // ハコのナカミ
  std::mutex mtx_;
  std::condition_variable can_get_;
  std::condition_variable can_set_;
public:
  concurrent_box() : box_() {}
  concurrent_box(const T& value) : box_(value) {}

  void set(const T& value) {
    std::unique_lock<std::mutex> guard(mtx_); 
    can_set_.wait(guard, [this]() { return box_.empty(); });
    box_.set(value);
    can_get_.notify_one();
  }

  T get() { 
    std::unique_lock<std::mutex> guard(mtx_); 
    can_get_.wait(guard, [this]() { return box_.occupied(); });
    T value = box_.get();
    can_set_.notify_one();
    return value;
  }
};

#include <iostream>
#include <random>
#include <string>

int produce(concurrent_box<int>& out) {
  using namespace std;
  const int N = 100000;
  random_device gen;
  uniform_int_distribution<int> dist(0,3); // 0～3のランダムな整数
  auto rand = [&]() { return dist(gen); };
  int sum = 0;
  for ( int i = 0; i < N; ++i ) {
    int value = rand();
    out.set(value);
    sum += value;
  }
  cout << to_string(N) + " items produced. sum= " + to_string(sum) + "\n";
  return sum;
}

int consume(concurrent_box<int>& in) {
  using namespace std;
  int count = 0;
  int sum = 0;
  while ( true ) {
    int value = in.get();
    if ( value < 0 ) break;
    ++count;
    sum += value;
  }
  cout << to_string(count) + " items consumed. sum= " + to_string(sum) + "\n";
  return sum;
}

#include <thread>
#include <future>
#include <utility>

int main() {
  using namespace std;

  // ふたつのproducer/ふたつのconsumerが
  // ひとつのconcurrent_boxを介してデータを受け渡す
  concurrent_box<int> b;
  future<int> p0 = async(produce, std::ref(b));
  future<int> p1 = async(produce, std::ref(b));
  future<int> c0 = async(consume, std::ref(b));
  future<int> c1 = async(consume, std::ref(b));

  cout << "produced sum = " << p0.get()+p1.get() << endl;
  b.set(-1); // おしまいのシルシ
  b.set(-1); // おしまいのシルシ
  cout << "consumed sum = " << c0.get()+c1.get() << endl;

}
