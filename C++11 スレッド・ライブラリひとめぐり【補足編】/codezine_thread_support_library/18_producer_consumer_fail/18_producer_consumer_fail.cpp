/*
   ※ マトモに動くと思うなよ!
 */
template<typename T>
class box {
private:
  T value_;    // ハコのナカミ
public:
  box()  {}
  box(const T& value) : value_(value) {}
  void set(const T& value) { value_ = value; }
  const T& get() const     { return value_; }
};

#include <iostream>
#include <random>
#include <string>

void produce(box<int>& out) {
  using namespace std;
  const int N = 100000;
  random_device gen;
  uniform_int_distribution<int> dist(0,3); // 0～3のランダムな整数
  auto rand = [&]() { return dist(gen); };
  int sum = 0;
  for ( int i = 0; i < N; ++i ) {
    int value = rand();
    out.set(value); // 送出 
    sum += value;
  }
  cout << to_string(N) + " items produced. sum= " + to_string(sum) + "\n";
}

void consume(box<int>& in) {
  using namespace std;
  int count = 0;
  int sum = 0;
  while ( true ) {
    int value = in.get(); // 受取
    if ( value < 0 ) break; // ここでloopを抜ける
    ++count;
    sum += value;
  }
  cout << to_string(count) + " items consumed. sum= " + to_string(sum) + "\n";
}

#include <thread>
#include <utility>

int main() {
  box<int> b;
  std::thread producer(produce, std::ref(b));
  std::thread consumer(consume, std::ref(b));
  producer.join();
  b.set(-1); // consumerを停止させるため
  consumer.join();
}