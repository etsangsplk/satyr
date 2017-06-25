// Fallback to serial version if there's no task-based parallelism enabled.
#pragma once

#include <satyr/serial_for.h>
#include <satyr/matrix.h>

namespace satyr {
//------------------------------------------------------------------------------
// for_
//------------------------------------------------------------------------------
template <Policy Policy, IndexFunctor<1> F>
  requires has_policy_v<grainularity, Policy>
void for_(Policy policy, index_t first, index_t last, F f) {
  for_(policy | serial_v, first, last, f);
}

//------------------------------------------------------------------------------
// for_with_exit
//------------------------------------------------------------------------------
template <Policy Policy, IndexPredicate<1> F>
  requires has_policy_v<grainularity, Policy>
bool for_with_exit(Policy policy, index_t first, index_t last, F f) {
  return for_with_exit(policy | serial_v, first, last, f);
}

//------------------------------------------------------------------------------
// for_each_index_triangular_with_exit
//------------------------------------------------------------------------------
template <uplo_t Uplo, Policy Policy, IndexPredicate<2> Functor>
  requires has_policy_v<grainularity, Policy>
bool for_each_index_triangular_with_exit(Policy policy, index_t n,
                                           Functor f) {
  return for_each_index_triangular_with_exit(policy | serial_v, n, f);
}
} // namespace satyr