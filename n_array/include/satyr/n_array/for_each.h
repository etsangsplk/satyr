#pragma once

#include <satyr/n_array/concept.h>
#include <satyr/n_array/map.h>
#include <satyr/n_array/execute.h>

namespace satyr {
//------------------------------------------------------------------------------
// is_scalar_functor_v
//------------------------------------------------------------------------------
namespace detail {
template <class, class>
constexpr bool is_scalar_functor_v = false;

template <class F, class T>
  requires std::is_copy_constructible_v<F> &&
           requires (F f, T x) {
              f(x);
           }
constexpr bool is_scalar_functor_v<F, T> = true;
} // namespace detail

//------------------------------------------------------------------------------
// is_scalar_index_functor_v
//------------------------------------------------------------------------------
namespace detail {
template <class, class, class>
constexpr bool is_scalar_index_functor_impl = false;

template <size_t... Indexes, class F, class T>
  requires std::is_copy_constructible_v<F> && 
           requires (F f, T x, 
                     std::enable_if_t<(Indexes, true), index_t>... indexes) {
             f(x, indexes...);
           }
constexpr bool
    is_scalar_index_functor_impl<std::index_sequence<Indexes...>, F, T> = true;

template <class F, size_t K, class T>
constexpr bool is_scalar_index_functor_v =
    is_scalar_index_functor_impl<std::make_index_sequence<K>, F, T>;
}  // namespace detail

//------------------------------------------------------------------------------
// for_each_evaluator
//------------------------------------------------------------------------------
namespace detail {
template <class, class, class>
class for_each_evaluator_impl;

template <size_t... Indexes, FlatEvaluator Evaluator, class F>
  requires is_scalar_functor_v<F, codomain_t<Evaluator>>
class for_each_evaluator_impl<std::index_sequence<Indexes...>, Evaluator, F> {
 public:
   for_each_evaluator_impl(const Evaluator& evaluator, const F& f)
     : evaluator_{evaluator}, f_{f} {}

   decltype(auto) operator()(index_t index) const {
     auto& x = evaluator_(index);
     f_(x);
     return x;
   }
 private:
   Evaluator evaluator_;
   F f_;
};

template <size_t... Indexes, KEvaluator<sizeof...(Indexes)> Evaluator, class F>
  requires is_scalar_functor_v<F, codomain_t<Evaluator>>
class for_each_evaluator_impl<std::index_sequence<Indexes...>, Evaluator, F> {
 public:
   for_each_evaluator_impl(const Evaluator& evaluator, F f)
     : evaluator_{evaluator}, f_{f} {}

   decltype(auto) operator()(
       const shape<sizeof...(Indexes)>& shape,
       std::enable_if_t<(Indexes, true), index_t>... indexes) const {
     auto& x = evaluator_(shape, indexes...);
     f_(x);
     return x;
   }

  private:
   Evaluator evaluator_;
   F f_;
};

template <size_t... Indexes, KEvaluator<sizeof...(Indexes)> Evaluator, class F>
  requires is_scalar_index_functor_v<F, sizeof...(Indexes), 
                                     codomain_t<Evaluator>>
class for_each_evaluator_impl<std::index_sequence<Indexes...>, Evaluator, F> {
 public:
   for_each_evaluator_impl(const Evaluator& evaluator, F f)
     : evaluator_{evaluator}, f_{f} {}

   decltype(auto) operator()(
       const satyr::shape<sizeof...(Indexes)>& shape,
       std::enable_if_t<(Indexes, true), index_t>... indexes) const {
     auto& x = evaluator_(shape, indexes...);
     f_(x, indexes...);
     return x;
   }
 private:
   Evaluator evaluator_;
   F f_;
};

template <size_t K, class Evaluator, class F>
class for_each_evaluator
    : public for_each_evaluator_impl<std::make_index_sequence<K>, Evaluator,
                                     F> {
  using base =
      for_each_evaluator_impl<std::make_index_sequence<K>, Evaluator, F>;

 public:
  using base::base;
};
} // namespace detail

//------------------------------------------------------------------------------
// make_for_each_evaluator
//------------------------------------------------------------------------------
namespace detail {
template <size_t K, FlatEvaluator Evaluator, class F>
  requires is_scalar_functor_v<F, codomain_t<Evaluator>>
for_each_evaluator<K, Evaluator, F> make_for_each_evaluator(
    const Evaluator& evaluator, F f) {
  return {evaluator, f};
}

template <size_t K, KEvaluator<K> Evaluator, class F>
  requires is_scalar_functor_v<F, codomain_t<Evaluator>> ||
           is_scalar_index_functor_v<F, K, codomain_t<Evaluator>>
for_each_evaluator<K, Evaluator, F> make_for_each_evaluator(
    const Evaluator& evaluator, F f) {
  return {evaluator, f};
}

template <size_t K, FlatEvaluator Evaluator, class F>
  requires is_scalar_index_functor_v<F, K, codomain_t<Evaluator>>
auto make_for_each_evaluator(const Evaluator& evaluator, F f) {
  return make_for_each_evaluator<K>(make_k_evaluator<K>(evaluator), f);
}
} // namespace detail

//------------------------------------------------------------------------------
// for_each
//------------------------------------------------------------------------------
template <Policy Policy, NArrayExpressible Expressible, class F>
  requires detail::is_scalar_functor_v<F, 
                                       detail::expressible_codomain_t<
                                                  structure_t<Expressible>, 
                                                  Expressible>> ||
           detail::is_scalar_index_functor_v<
                                       F, 
                                       num_dimensions_v<Expressible>,
                                       detail::expressible_codomain_t<
                                                  structure_t<Expressible>, 
                                                  Expressible>>
void for_each(Policy policy, Expressible&& expressible, F f) {
  auto n_array_expression = make_expression(expressible) << policy;
  auto for_each_expression = make_n_array_expression<structure_t<Expressible>>(
      n_array_expression.shape(),
      detail::make_for_each_evaluator<num_dimensions_v<Expressible>>(
          n_array_expression.evaluator(), f));
  execute(for_each_expression);
}

template <NArrayExpressible Expressible, class F>
  requires detail::is_scalar_functor_v<F, 
                                       detail::expressible_codomain_t<
                                                  structure_t<Expressible>, 
                                                  Expressible>> ||
           detail::is_scalar_index_functor_v<
                                       F, 
                                       num_dimensions_v<Expressible>,
                                       detail::expressible_codomain_t<
                                                  structure_t<Expressible>, 
                                                  Expressible>>
void for_each(Expressible&& expressible, F f) {
  for_each(no_policy_v, expressible, f);
}

//------------------------------------------------------------------------------
// is_scalar_predicate_v
//------------------------------------------------------------------------------
namespace detail {
template <class, class>
constexpr bool is_scalar_predicate_v = false;

template <class F, class T>
  requires std::is_copy_constructible_v<F> &&
           requires (F f, T x) {
             { f(x) } -> bool;
           }
constexpr bool is_scalar_predicate_v<F, T> = true;
} // namespace detail

//------------------------------------------------------------------------------
// is_scalar_index_predicate_v
//------------------------------------------------------------------------------
namespace detail {
template <class, class, class>
constexpr bool is_scalar_index_predicate_impl = false;

template <size_t... Indexes, class F, class T>
  requires std::is_copy_constructible_v<F> && 
           requires (F f, int x, 
                     std::enable_if_t<(Indexes, true), index_t>... indexes) {
             { f(x, indexes...) } -> bool;
           }
constexpr bool
    is_scalar_index_predicate_impl<std::index_sequence<Indexes...>, F, T> =
        true;

template <class F, size_t K, class T>
constexpr bool is_scalar_index_predicate_v =
    is_scalar_index_predicate_impl<std::make_index_sequence<K>, F, T>;
}  // namespace detail

//------------------------------------------------------------------------------
// for_each_with_exit_evaluator
//------------------------------------------------------------------------------
namespace detail {
template <class, class, class>
class for_each_with_exit_evaluator_impl;

template <size_t... Indexes, FlatEvaluator Evaluator, class F>
  requires is_scalar_predicate_v<F, codomain_t<Evaluator>>
class for_each_with_exit_evaluator_impl<std::index_sequence<Indexes...>, Evaluator, F> {
 public:
   for_each_with_exit_evaluator_impl(const Evaluator& evaluator, const F& f)
     : evaluator_{evaluator}, f_{f} {}

   decltype(auto) operator()(index_t index) const {
     return f_(evaluator_(index));
   }
 private:
   Evaluator evaluator_;
   F f_;
};

template <size_t... Indexes, KEvaluator<sizeof...(Indexes)> Evaluator, class F>
  requires is_scalar_predicate_v<F, codomain_t<Evaluator>>
class for_each_with_exit_evaluator_impl<std::index_sequence<Indexes...>, Evaluator, F> {
 public:
   for_each_with_exit_evaluator_impl(const Evaluator& evaluator, F f)
     : evaluator_{evaluator}, f_{f} {}

   decltype(auto) operator()(
       const shape<sizeof...(Indexes)>& shape,
       std::enable_if_t<(Indexes, true), index_t>... indexes) const {
     return f_(evaluator_(shape, indexes...));
   }
 private:
   Evaluator evaluator_;
   F f_;
};

template <size_t... Indexes, KEvaluator<sizeof...(Indexes)> Evaluator, class F>
  requires is_scalar_index_predicate_v<F, sizeof...(Indexes), 
                                       codomain_t<Evaluator>>
class for_each_with_exit_evaluator_impl<std::index_sequence<Indexes...>, Evaluator, F> {
 public:
   for_each_with_exit_evaluator_impl(const Evaluator& evaluator, F f)
     : evaluator_{evaluator}, f_{f} {}

   decltype(auto) operator()(
       const satyr::shape<sizeof...(Indexes)>& shape,
       std::enable_if_t<(Indexes, true), index_t>... indexes) const {
     return f_(evaluator_(shape, indexes...), indexes...);
   }
 private:
   Evaluator evaluator_;
   F f_;
};

template <size_t K, class Evaluator, class F>
class for_each_with_exit_evaluator
    : public for_each_with_exit_evaluator_impl<std::make_index_sequence<K>, Evaluator,
                                     F> {
  using base =
      for_each_with_exit_evaluator_impl<std::make_index_sequence<K>, Evaluator, F>;

 public:
  using base::base;
};
} // namespace detail

//------------------------------------------------------------------------------
// make_for_each_evaluator
//------------------------------------------------------------------------------
namespace detail {
template <size_t K, FlatEvaluator Evaluator, class F>
  requires is_scalar_predicate_v<F, codomain_t<Evaluator>>
for_each_with_exit_evaluator<K, Evaluator, F> make_for_each_with_exit_evaluator(
    const Evaluator& evaluator, F f) {
  return {evaluator, f};
}

template <size_t K, KEvaluator<K> Evaluator, class F>
  requires is_scalar_predicate_v<F, codomain_t<Evaluator>> ||
           is_scalar_index_predicate_v<F, K, codomain_t<Evaluator>>
for_each_with_exit_evaluator<K, Evaluator, F> make_for_each_with_exit_evaluator(
    const Evaluator& evaluator, F f) {
  return {evaluator, f};
}

template <size_t K, FlatEvaluator Evaluator, class F>
  requires is_scalar_index_predicate_v<F, K, codomain_t<Evaluator>>
auto make_for_each_with_exit_evaluator(const Evaluator& evaluator, F f) {
  return make_for_each_with_exit_evaluator<K>(make_k_evaluator<K>(evaluator),
                                              f);
}
} // namespace detail

//------------------------------------------------------------------------------
// for_each_with_exit
//------------------------------------------------------------------------------
template <Policy Policy, NArrayExpressible Expressible, class F>
  requires detail::is_scalar_predicate_v<F, 
                                         detail::expressible_codomain_t<
                                                    structure_t<Expressible>, 
                                                    Expressible>> ||
           detail::is_scalar_index_predicate_v<
                                       F, 
                                       num_dimensions_v<Expressible>,
                                       detail::expressible_codomain_t<
                                                  structure_t<Expressible>, 
                                                  Expressible>>
bool for_each_with_exit(Policy policy, Expressible&& expressible, F f) {
  auto n_array_expression = make_expression(expressible) << policy;
  auto for_each_with_exit_expression = make_n_array_expression<
      structure_t<Expressible>>(
      n_array_expression.shape(),
      detail::make_for_each_with_exit_evaluator<num_dimensions_v<Expressible>>(
          n_array_expression.evaluator(), f));
  return execute_with_exit(for_each_with_exit_expression);
}

template <NArrayExpressible Expressible, class F>
  requires detail::is_scalar_predicate_v<F, 
                                       detail::expressible_codomain_t<
                                                  structure_t<Expressible>, 
                                                  Expressible>> ||
           detail::is_scalar_index_predicate_v<
                                       F, 
                                       num_dimensions_v<Expressible>,
                                       detail::expressible_codomain_t<
                                                  structure_t<Expressible>, 
                                                  Expressible>>
bool for_each_with_exit(Expressible&& expressible, F f) {
  return for_each_with_exit(no_policy_v, expressible, f);
}
} // namespace satyr
