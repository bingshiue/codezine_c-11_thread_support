#include <iostream>
#include <string>
#include <thread>
#include <mutex>

std::mutex mtxA;
std::mutex mtxB;

int countA;
int countB;

#if 0

void incA() {
  mtxA.lock(); // lock-A
  countA += 1;

  mtxB.lock(); // lock-B
  countB += 1;
  std::cout << "A=" + std::to_string(countA) + "  B=" << std::to_string(countB) + "\n";
  mtxB.unlock(); // unlock-B

  mtxA.unlock(); // unlock-A
}

void incB() {
  mtxB.lock(); // lock-B
  countB -= 1;

  mtxA.lock(); // lock-A
  countA -= 1;
  std::cout << "A=" + std::to_string(countA) + "  B=" << std::to_string(countB) + "\n";

  mtxA.unlock(); // unlock-A
  mtxB.unlock(); // unlock-B
}

#else

void incA() {
  std::lock(mtxA, mtxB);
  countA += 1;
  countB += 1;
  std::cout << "A=" + std::to_string(countA) + "  B=" << std::to_string(countB) + "\n";
  mtxB.unlock(); // unlock-B
  mtxA.unlock(); // unlock-A
}

void incB() {
  std::lock(mtxB, mtxA);
  countB -= 1;
  countA -= 1;
  std::cout << "A=" + std::to_string(countA) + "  B=" << std::to_string(countB) + "\n";

  mtxA.unlock(); // unlock-A
  mtxB.unlock(); // unlock-B
}

#endif

int main() {
  countA = 0;
  countB = 0;

  std::thread thrA([]() { for ( int i = 0; i < 10000; ++i ) incA(); });
  std::thread thrB([]() { for ( int i = 0; i < 10000; ++i ) incB(); });

  thrA.join();
  thrB.join();

  std::cout << "finally, A=" << countA << "  B=" << countB << std::endl;
}
