#include <iostream>
#include <utility>   // move
#include <stdexcept> // invalid_argument
#include <string>    // to_string
#include <future>    // future/async

/*
 * 0+1+...+n を返す
 * n < 0 のときは invalid_argument 例外をthrowする
 */
int sigma(int n) {
  if ( n < 0 ) {
    throw std::invalid_argument(std::to_string(n) + " は勘弁して");
  }
  int sum = 0;
  for ( int i = 0; i <= n; ++i ) {
    sum += i;
  }
  return sum;
}

int main() {
  using namespace std;

  for ( int n : { 5, -3, 10 } ) try {
    future<int> result = async(sigma, n);
    cout << "1+2+...+" << n <<  "= " << result.get() << endl;
  } catch ( invalid_argument& err ) { // 再throwされた例外をcatch
    cerr << err.what() << endl;    
  }

}
