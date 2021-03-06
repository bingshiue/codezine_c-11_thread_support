#include <iostream>
#include <thread>
#include <future>  // async/future

/*
 * 0+1+...+n を返す
 */
int sigma(int n) {
  int sum = 0;
  for ( int i = 0; i <= n; ++i ) {
    sum += i;
  }
  return sum;
}

int main() {
  using namespace std;

  // asyncでスレッドを起動し、さらにfutureを手に入れる
  future<int> result = async(sigma, 10);

  // 結果は future<T>::get() で受け取る
  cout << "0+1+...+n = " << result.get() << endl;
}
