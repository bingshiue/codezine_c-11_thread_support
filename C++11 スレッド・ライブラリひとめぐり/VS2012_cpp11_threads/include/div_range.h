#ifndef DIV_RANGE_H_
#define DIV_RANGE_H_

template<typename T =int>
class div_range {
private:
  T lo_;
  T hi_;
  T stride_;
  int n_;
public:
  div_range(T lo, T hi, int n) 
    : lo_(lo), hi_(hi), n_(n) { stride_ = (hi-lo)/n; }
  T lo(int n) const { return lo_ + stride_ * n; }
  T hi(int n) const { return (++n < n_) ? lo_ + stride_*n : hi_; }
};

#endif
 
