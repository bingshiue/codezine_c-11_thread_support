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

#include <thread>

template<typename T>
class concurrent_box {
private:
  box<T> box_;    // ハコのナカミ
public:
  concurrent_box() : box_() {}
  concurrent_box(const T& value) : box_(value) {}
  void set(const T& value) { 
    while ( box_.occupied() ) { std::this_thread::yield(); }
    box_.set(value);
  }
  const T& get() { 
    while ( box_.empty() ) { std::this_thread::yield();  }
    return box_.get();
  }
};
#include <iostream>
#include <random>
#include <string>

void produce(concurrent_box<int>& out) {
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
  out.set(-1); // おしまいのシルシ
  cout << to_string(N) + " items produced. sum= " + to_string(sum) + "\n";
}

void consume(concurrent_box<int>& in) {
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
}

#include <utility>

int main() {
  concurrent_box<int> b;
  std::thread producer(produce, std::ref(b));
  std::thread consumer(consume, std::ref(b));
  producer.join();
  consumer.join();
}
