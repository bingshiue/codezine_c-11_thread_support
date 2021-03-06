#include <mutex>
#include <condition_variable>
#include <queue>
#include <stdexcept>

template<typename T>
class concurrent_queue {
public:
  typedef typename std::queue<T>::size_type size_type;
private:
  std::queue<T> queue_;
  size_type capacity_;

  std::mutex mtx_;
  std::condition_variable can_pop_;
  std::condition_variable can_push_;
public:
  concurrent_queue(size_type capacity) : capacity_(capacity) {
    if ( capacity_ == 0 ) {
      throw std::invalid_argument("capacity cannot be zero.");
    }
  }

  void push(const T& value) {
    std::unique_lock<std::mutex> guard(mtx_); 
    can_push_.wait(guard, [this]() { return queue_.size() < capacity_; });
    queue_.push(value);
    can_pop_.notify_one();
  }

  T pop() { 
    std::unique_lock<std::mutex> guard(mtx_); 
    can_pop_.wait(guard, [this]() { return !queue_.empty(); });
    T value = queue_.front();
    queue_.pop();
    can_push_.notify_one();
    return value;
  }
};

#include <iostream>
#include <random>
#include <string>

int produce(concurrent_queue<int>& out) {
  using namespace std;
  const int N = 100000;
  random_device gen;
  uniform_int_distribution<int> dist(0,3); // 0～3のランダムな整数
  auto rand = [&]() { return dist(gen); };
  int sum = 0;
  for ( int i = 0; i < N; ++i ) {
    int value = rand();
    out.push(value);
    sum += value;
  }
  cout << to_string(N) + " items produced. sum= " + to_string(sum) + "\n";
  return sum;
}

int consume(concurrent_queue<int>& in) {
  using namespace std;
  int count = 0;
  int sum = 0;
  while ( true ) {
    int value = in.pop();
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
  // ひとつのconcurrent_queueを介してデータを受け渡す
  concurrent_queue<int> q(5); // 待ち行列の最大長を与える
  future<int> p0 = async(produce, std::ref(q));
  future<int> p1 = async(produce, std::ref(q));
  future<int> c0 = async(consume, std::ref(q));
  future<int> c1 = async(consume, std::ref(q));

  cout << "produced sum = " << p0.get()+p1.get() << endl;
  q.push(-1);
  q.push(-1);
  cout << "consumed sum = " << c0.get()+c1.get() << endl;

}
