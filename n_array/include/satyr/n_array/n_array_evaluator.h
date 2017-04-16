#pragma once

#include <satyr/index.h>
#include <satyr/concept.h>

namespace satyr {
//------------------------------------------------------------------------------
// contiguous_n_array_evaluator
//------------------------------------------------------------------------------
template <Scalar T>
class contiguous_n_array_evaluator {
 public:
  explicit contiguous_n_array_evaluator(T* data) : data_{data} {}

  decltype(auto) operator()(index_t index) const { return *(data_ + index); }

 private:
  T* data_;
};
} // namespace satyr