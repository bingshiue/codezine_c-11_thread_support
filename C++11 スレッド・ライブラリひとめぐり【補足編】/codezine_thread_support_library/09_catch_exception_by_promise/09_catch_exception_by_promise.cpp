#include <iostream>
#include <thread>
#include <functional> // ref
#include <stdexcept>  // invalid_argument
#include <exception>  // current_exception/rethrow_exception
#include <string>     // to_string
#include <future>     // future/promise

/*
 * 0+1+...+n を求め resultにセットする
 * n < 0 のときは invalid_argument 例外をthrowする
 */
void sigma_promise(int n, std::promise<int> result) try {
  if ( n < 0 ) {
    throw std::invalid_argument(std::to_string(n) + " は勘弁して");
  }
  int sum = 0;
  for ( int i = 0; i <= n; ++i ) {
    sum += i;
  }
  result.set_value(sum);
} catch ( ... ) { // 全例外を捕まえて promiseにセットする
  result.set_exception(std::current_exception());
}

int main() {
  using namespace std;

  for ( int n : { 5, -3, 10 } ) try {
    promise<int> prm;
    future<int> result = prm.get_future();    
    thread thr(sigma_promise, n, move(prm));
    thr.detach();
    // 例外のthrowはfuture<T>::get()で起こるので、
    // この位置からtryしてもいい
    cout << "1+2+...+" << n <<  "= " << result.get() << endl;
  } catch ( invalid_argument& err ) { // 再throwされた例外をcatch
    cerr << err.what() << endl;    
  }

}
