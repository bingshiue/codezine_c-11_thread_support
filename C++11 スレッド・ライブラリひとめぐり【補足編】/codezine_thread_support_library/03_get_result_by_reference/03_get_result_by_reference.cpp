#include <iostream>
#include <thread>
#include <functional> // ref

int global_result;

/*
 * 0+1+...+n を求める
 *  グローバル変数を介した(イケてない)方法
 */
void sigma_global(int n) {
  int sum = 0;
  for ( int i = 0; i <= n; ++i ) {
    sum += i;
  }
  global_result = sum;
}

/*
 * 0+1+...+n を求め result にセットする
 *  参照を介した(すこしはマシな)方法
 */
void sigma_reference(int n, int& result) {
  int sum = 0;
  for ( int i = 0; i <= n; ++i ) {
    sum += i;
  }
  result = sum;
}

int main() {
  using namespace std;

  int local_result;
  thread thr_g(sigma_global, 10);
  // ※ 参照を渡す際には std::ref で囲むべし ↓ (const参照なら std::cref で)
  thread thr_r(sigma_reference, 10, ref(local_result));

  thr_g.join();
  thr_r.join();
  cout << "イケてない結果: "    << global_result << ", "
       << "少しはマシな結果:  " << local_result << endl;

}
