#include <thread>
#include <chrono>
#include <string>
#include <iostream>
#include <exception>

void bad_termination() {
  std::cerr << "異常終了!!\n";
}

int main() {
  using namespace std;

  std::set_terminate(bad_termination);
  cout << "main:\n";
  thread thr([](int n) {
               cout << "lambda_exp: " + to_string(n) + " 秒後に終了します...\n";
               this_thread::sleep_for(chrono::seconds(n));
               cout << "lambda_exp: おしまい\n";
             }, 2);
  cout << "main:おしまい\n";
  // 異常終了を避けたいなら thr.join() : 完了待ち
  //            さもなくば thr.detach() : 親権放棄 すべし
}
