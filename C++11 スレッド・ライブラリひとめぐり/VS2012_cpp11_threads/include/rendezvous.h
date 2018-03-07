#ifndef RENDEZVOUS_H_
#define RENDEZVOUS_H_

#include <condition_variable>
#include <stdexcept>          // invalid_argument

/*
 * ランデブー またの名を バリア(barrier)
 */
class rendezvous {
public:
  rendezvous(unsigned int count) 
    : threshold_(count), count_(count), generation_(0) {
    if (count == 0) { throw std::invalid_argument("count cannot be zero."); }
  }

  bool wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    unsigned int gen = generation_;
    if ( --count_ == 0) {
      generation_++;
      count_ = threshold_;
      condition_.notify_all();
      return true;
    }
    condition_.wait(lock, [&](){return gen != generation_;});
    return false;
  }

private:
  std::mutex mutex_;
  std::condition_variable condition_;
  unsigned int threshold_;
  unsigned int count_;
  unsigned int generation_;
};

#endif
