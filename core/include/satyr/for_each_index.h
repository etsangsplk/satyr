#pragma once

#include <satyr/serial_for.h>
#include <satyr/parallel_for.h>
#include <satyr/matrix.h>

namespace satyr {
//------------------------------------------------------------------------------
// for_each_index
//------------------------------------------------------------------------------
template <Policy Policy, size_t K, IndexFunctor<K> Functor>
  requires !has_policy_v<grainsize, Policy>
void for_each_index(Policy policy, std::array<index_t, K> extents,
    Functor f) {
  if constexpr(K == 1) { for_(policy, 0, extents[0], f); }
  else {
    for_(serial_v, 0, extents[K - 1], [=](index_t i) {
      auto f_prime = [=](auto... indexes) { f(indexes..., i); };
      return for_each_index(
          policy, reinterpret_cast<const std::array<index_t, K - 1>&>(extents),
          f_prime);
    });
  }
}

namespace detail {
template <class Policy, size_t K, class Functor>
void parallel_for_each_index_impl(Policy policy,
                                  std::array<index_t, K-1> cardinalities,
                                  std::array<index_t, K> extents,
                                  Functor f) {
  if constexpr(K == 1) { 
    for_(policy, 0, extents[0], f); 
  } else {
    auto n = cardinalities[K - 2];
    auto grainsize = subdivide(get_policy<satyr::grainsize>(policy), n);
    for_(grainsize, 0, n, [=](index_t i) {
      auto f_prime = [=](auto... indexes) { f(indexes..., i); };
      return parallel_for_each_index_impl(
          policy,
          reinterpret_cast<const std::array<index_t, K - 2>&>(extents),
          reinterpret_cast<const std::array<index_t, K - 1>&>(extents),
          f_prime);
    });
  }
}
} // namespace detail

template <Policy Policy, size_t K, IndexFunctor<K> Functor>
  requires has_policy_v<grainsize, Policy> 
void for_each_index(Policy policy, std::array<index_t, K> extents, Functor f) {
  std::array<index_t, K - 1> cardinalities;
  index_t cardinality = 1;
  for (index_t i = 0; i < K - 1; ++i) {
    cardinality *= extents[i];
    cardinalities[i] = cardinality;
  }
  detail::parallel_for_each_index_impl(policy, cardinalities, extents, f);
}

//------------------------------------------------------------------------------
// for_each_index_triangular
//------------------------------------------------------------------------------
namespace detail {
template <uplo_t Uplo, class Policy, class Functor>
    requires Uplo == uplo_t::lower 
void for_each_index_triangular_impl(Policy policy, index_t j, index_t n,
                                    Functor f) {
  for_(policy, j, n, [=](index_t i) { f(i, j); });
}

template <uplo_t Uplo, class Policy, class Functor>
    requires Uplo == uplo_t::upper 
void for_each_index_triangular_impl(Policy policy, index_t j, index_t n,
                                    Functor f) {
  for_(policy, 0, j+1, [=](index_t i) { f(i, j); });
}
}  // namespace detail

template <uplo_t Uplo, Policy Policy, IndexFunctor<2> Functor>
  requires !has_policy_v<grainsize, Policy>
void for_each_index_triangular(Policy policy, index_t n, Functor f) {
  for_(no_policy_v, 0, n, [=](index_t j) {
    detail::for_each_index_triangular_impl<Uplo>(policy, j, n, f);
  });
}

template <uplo_t Uplo, Policy Policy, IndexFunctor<2> Functor>
  requires has_policy_v<grainsize, Policy>
void for_each_index_triangular(Policy policy, index_t n, Functor f) {
  auto grainsize_outer = subdivide(get_policy<grainsize>(policy), n + 1);
  auto n_div_2 = n / 2;
  auto p = n / 2 + (n % 2);
  for_(grainsize_outer, 0, p, [=](index_t j1) {
    auto j2 = n - j1 - 1;
    if (j1 != j2) {
      detail::for_each_index_triangular_impl<Uplo>(policy, j1, n, f);
      detail::for_each_index_triangular_impl<Uplo>(policy, j2, n, f);
    } else {
      detail::for_each_index_triangular_impl<Uplo>(policy, j1, n, f);
    }
  });
}

//------------------------------------------------------------------------------
// for_each_index_with_exit
//------------------------------------------------------------------------------
template <Policy Policy, size_t K, IndexPredicate<K> Functor>
  requires !has_policy_v<grainsize, Policy>
bool for_each_index_with_exit(Policy policy, std::array<index_t, K> extents,
                                Functor f) {
  if constexpr(K == 1) { return for_with_exit(policy, 0, extents[0], f); }
  else {
    return for_with_exit(serial_v, 0, extents[K - 1], [=](index_t i) {
      auto f_prime = [=](auto... indexes) { return f(indexes..., i); };
      return for_each_index_with_exit(
          policy, reinterpret_cast<const std::array<index_t, K - 1>&>(extents),
          f_prime);
    });
  }
}

//------------------------------------------------------------------------------
// for_each_index_triangular_with_exit
//------------------------------------------------------------------------------
namespace detail {
template <uplo_t Uplo, class Policy, class Functor>
    requires Uplo == uplo_t::lower 
bool for_each_index_triangular_with_exit_impl(Policy policy, index_t j,
                                                index_t n, Functor f) {
  return for_with_exit(policy, j, n, [=](index_t i) { return f(i, j); });
}

template <uplo_t Uplo, class Policy, class Functor>
    requires Uplo == uplo_t::upper 
bool for_each_index_triangular_with_exit_impl(Policy policy, index_t j,
                                                index_t n, Functor f) {
  return for_with_exit(policy, 0, j+1, [=](index_t i) { return f(i, j); });
}
}  // namespace detail

template <uplo_t Uplo, Policy Policy, IndexFunctor<2> Functor>
  requires !has_policy_v<grainsize, Policy>
bool for_each_index_triangular_with_exit(Policy policy, index_t n,
                                           Functor f) {
  return for_with_exit(no_policy_v, 0, n, [=](index_t j) {
    return detail::for_each_index_triangular_with_exit_impl<Uplo>(policy, j,
                                                                    n, f);
  });
}
} // namespace satyr
