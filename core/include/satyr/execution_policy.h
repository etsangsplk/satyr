#pragma once

#include <satyr/policy.h>
#include <satyr/index.h>
#include <algorithm>

namespace satyr {
//------------------------------------------------------------------------------
// no_policy
//------------------------------------------------------------------------------
const struct no_policy : policy<no_policy> {} no_policy_v{};

//------------------------------------------------------------------------------
// simd_policy_group
//------------------------------------------------------------------------------
struct simd_policy_group : policy<simd_policy_group> {};

//------------------------------------------------------------------------------
// simd
//------------------------------------------------------------------------------
const struct simd : simd_policy_group {} simd_v{};

//------------------------------------------------------------------------------
// nosimd
//------------------------------------------------------------------------------
const struct nosimd : simd_policy_group {} nosimd_v{};

//------------------------------------------------------------------------------
// temporal_policy_group
//------------------------------------------------------------------------------
struct temporal_policy_group : policy<temporal_policy_group> {};

//------------------------------------------------------------------------------
// nontemporal
//------------------------------------------------------------------------------
const struct nontemporal : temporal_policy_group {} nontemporal_v{};

//------------------------------------------------------------------------------
// temporal
//------------------------------------------------------------------------------
const struct temporal : temporal_policy_group {} temporal_v{};

///------------------------------------------------------------------------------/
// parallel_policy_group
//------------------------------------------------------------------------------
struct parallel_policy_group : policy<parallel_policy_group> {};

//------------------------------------------------------------------------------
// serial
//------------------------------------------------------------------------------
const struct serial : parallel_policy_group {} serial_v{};

//------------------------------------------------------------------------------
// grainsize
//------------------------------------------------------------------------------
struct grainsize : parallel_policy_group {
  grainsize() : value{1} {}
  explicit grainsize(index_t value) : value{value} {}
  index_t value;
};

inline satyr::grainsize subdivide(satyr::grainsize grainsize,
                                     index_t n) {
  return satyr::grainsize{std::max<index_t>(grainsize.value / n, 1)};
}

//------------------------------------------------------------------------------
// parallel
//------------------------------------------------------------------------------
const grainsize parallel_v{1};

//------------------------------------------------------------------------------
// inline_policy_group
//------------------------------------------------------------------------------
struct inline_policy_group : policy<inline_policy_group> {};

//------------------------------------------------------------------------------
// suggest_inline
//------------------------------------------------------------------------------
const struct suggest_inline : inline_policy_group {} suggest_inline_v{};
} // namespace satyr
