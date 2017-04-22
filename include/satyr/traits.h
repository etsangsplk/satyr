#pragma once

#include <satyr/type_list.h>
#include <type_traits>
#include <memory>

namespace satyr {
//------------------------------------------------------------------------------
// reference_t
//------------------------------------------------------------------------------
template <class T> struct reference_type {};

// clang-format off
template <class T>
  requires requires { typename T::reference; }
struct reference_type<T> {
  // clang-format on
  using type = typename T::reference;
};

// clang-format off
template <class T>
  requires requires(T* t) { { *t } -> auto&&; }
struct reference_type<T*> {
  // clang-format on
  using type = T&;
};

// clang-format off
template <class T>
  requires requires(
      typename reference_type<std::remove_cv_t<T>>::type reference) {
    { reference } -> auto&&;
  }
using reference_t = typename reference_type<std::remove_cv_t<T>>::type;
// clang-format on

//------------------------------------------------------------------------------
// value_type_t
//------------------------------------------------------------------------------
template <class T> struct value_type {};

// clang-format off
template <class T>
  // requires DETAIL_NS::has_member_value_type_v<T>
  requires requires { typename T::value_type; }
struct value_type<T> {
  // clang-format on
  using type = typename T::value_type;
};

// clang-format off
template <class T>
  requires requires { typename T::element_type; }
struct value_type<T> {
  // clang-format on
  using type = typename T::element_type;
};

// clang-format off
template <class T>
  requires requires(T* t) { { *t } -> auto&&; }
struct value_type<T*> {
  // clang-format on
  using type = std::decay_t<T>;
};

// clang-format off
template <class T>
  requires requires(T x) { { x.data() } -> auto*; } &&
           !requires { typename T::value_type; } &&
           !requires { typename T::element_type; }
struct value_type<T> {
  // clang-format on
  using type = typename value_type<decltype(std::declval<T>().data())>::type;
};

// clang-format off
template <class T>
  requires requires(typename value_type<std::remove_cv_t<T>>::type value) {
    { value } -> auto&&;
  }
using value_type_t = typename value_type<std::remove_cv_t<T>>::type;
// clang-format on

//------------------------------------------------------------------------------
// pointer_t
//------------------------------------------------------------------------------
template <class> struct pointer_type {};

// clang-format off
template <class T>
  requires requires { typename T::pointer; }
struct pointer_type<T> {
  // clang-format on
  using type = typename T::pointer;
};

template <class T> struct pointer_type<T*> { using type = T*; };

// clang-format off
template <class T>
  requires requires { typename value_type_t<T>; } &&
           !requires { typename T::pointer; }
struct pointer_type<T> {
  // clang-format on
  using type = value_type_t<T>*;
};

// clang-format off
template <class T>
  requires requires { typename pointer_type<std::remove_cv_t<T>>::type; }
using pointer_t = typename pointer_type<std::remove_cv_t<T>>::type;
// clang-format on

//------------------------------------------------------------------------------
// const_pointer_t
//------------------------------------------------------------------------------
template <class> struct const_pointer_type {};

// clang-format off
template <class T>
  requires requires { typename T::const_pointer; }
struct const_pointer_type<T> {
  // clang-format on
  using type = typename T::const_pointer;
};

// clang-format off
template <class T>
  requires requires { 
              typename pointer_t<T>; 
              typename value_type_t<T>;
           } &&
           !requires { typename T::const_pointer; }
struct const_pointer_type<T> {
  // clang-format on
  using type = typename std::pointer_traits<pointer_t<T>>::template rebind<
      const value_type_t<T>>;
};

// clang-format off
template <class T>
  requires requires { typename const_pointer_type<std::remove_cv_t<T>>::type; }
using const_pointer_t = typename const_pointer_type<std::remove_cv_t<T>>::type;
// clang-format on

//------------------------------------------------------------------------------
// void_pointer_t
//------------------------------------------------------------------------------
template <class> struct void_pointer_type {};

// clang-format off
template <class T>
  requires requires { typename T::void_pointer; }
struct void_pointer_type<T> {
  // clang-format on
  using type = typename T::void_pointer;
};

// clang-format off
template <class T>
  requires requires { typename pointer_t<T>; } &&
           !requires { typename T::void_pointer; }
struct void_pointer_type<T> {
  // clang-format on
  using type =
      typename std::pointer_traits<pointer_t<T>>::template rebind<void>;
};

// clang-format off
template <class T>
  requires requires { typename void_pointer_type<std::remove_cv_t<T>>::type; }
using void_pointer_t = typename void_pointer_type<std::remove_cv_t<T>>::type;
// clang-format on

//------------------------------------------------------------------------------
// const_void_pointer_t
//------------------------------------------------------------------------------
template <class> struct const_void_pointer_type {};

// clang-format off
template <class T>
  requires requires { typename T::const_void_pointer; }
struct const_void_pointer_type<T> {
  // clang-format on
  using type = typename T::const_void_pointer;
};

// clang-format off
template <class T>
  requires requires { typename pointer_t<T>; } &&
           !requires { typename T::const_void_pointer; }
struct const_void_pointer_type<T> {
  // clang-format on
  using type =
      typename std::pointer_traits<pointer_t<T>>::template rebind<const void>;
};

// clang-format off
template <class T>
  requires requires { 
    typename const_void_pointer_type<std::remove_cv_t<T>>::type; 
  }
using const_void_pointer_t = 
  typename const_void_pointer_type<std::remove_cv_t<T>>::type;
// clang-format on

//------------------------------------------------------------------------------
// difference_type_t
//------------------------------------------------------------------------------
template <class> struct difference_type {};

// clang-format off
template <class T>
  requires requires { typename T::difference_type; }
struct difference_type<T> {
  // clang-format on
  using type = typename T::difference_type;
};

// clang-format off
template <class T>
  requires requires { typename pointer_t<T>; } &&
           !requires { typename T::difference_type; }
struct difference_type<T> {
  // clang-format on
  using type = typename std::pointer_traits<pointer_t<T>>::difference_type;
};

// clang-format off
template <class T>
  requires requires(
      typename difference_type<std::remove_cv_t<T>>::type difference)
  {
    requires std::is_integral<decltype(difference)>::value;
    requires std::is_signed<decltype(difference)>::value;
  }
using difference_type_t = typename difference_type<std::remove_cv_t<T>>::type;
// clang-format on

//------------------------------------------------------------------------------
// size_type_t
//------------------------------------------------------------------------------
template <class> struct size_type {};

// clang-format off
template <class T>
  requires requires { typename T::size_type; }
struct size_type<T> {
  // clang-format on
  using type = typename T::size_type;
};

// clang-format off
template <class T>
  requires requires { typename difference_type_t<T>; } &&
           !requires { typename T::size_type; }
struct size_type<T> {
  // clang-format on
  using type = std::make_unsigned_t<difference_type_t<T>>;
};

// clang-format off
template <class T>
  requires requires { typename size_type<std::remove_cv_t<T>>::type; }
using size_type_t = typename size_type<std::remove_cv_t<T>>::type;
// clang-format on

//------------------------------------------------------------------------------
// uncvref_t
//------------------------------------------------------------------------------
template <class T>
using uncvref_t = typename std::remove_cv_t<std::remove_reference_t<T>>;

//------------------------------------------------------------------------------
// arity_v
//------------------------------------------------------------------------------
namespace detail {
template <class> struct arity_impl {};

template <class Return, class... Args> struct arity_impl<Return(Args...)> {
  static constexpr size_t value = sizeof...(Args);
};

template <class Return, class... Args> struct arity_impl<Return (*)(Args...)> {
  static constexpr size_t value = sizeof...(Args);
};

template <class Return, class... Args> struct arity_impl<Return (&)(Args...)> {
  static constexpr size_t value = sizeof...(Args);
};

#define CVREF_QUALIFY(CV_QUALIFER, REF_QUALIFER)                       \
  template <class Return, class Record, class... Args>                 \
  struct arity_impl<Return (Record::*)(Args...) CV_QUALIFER REF_QUALIFER> { \
    static constexpr size_t value = sizeof...(Args);                   \
  };
#include <satyr/cvref_qualifer.def>
#undef CVREF_QUALIFY

// clang-format off
template <class T> 
  requires requires {
    { &T::operator() } -> auto&&;
  }
struct arity_impl<T> {
  static constexpr size_t value = arity_impl<decltype(&T::operator())>::value;
  // clang-format on
};
} // namespace detail

// clang-format off
template <class T>
  requires requires {
    detail::arity_impl<std::remove_cv_t<T>>::value;
  }
static constexpr size_t arity_v = 
                          detail::arity_impl<std::remove_cv_t<T>>::value;
// clang-format on

//------------------------------------------------------------------------------
// argument_t
//------------------------------------------------------------------------------
template <size_t, class> struct argument_type {};

// clang-format off
template <size_t I, class Return, class... Args>
  requires I < sizeof...(Args)
struct argument_type<I, Return (Args...)> {
  // clang-format on
  using type = detail::type_list_element_t<I, detail::type_list<Args...>>;
};

// clang-format off
template <size_t I, class Return, class... Args>
  requires I < sizeof...(Args)
struct argument_type<I, Return (*)(Args...)> {
  // clang-format on
  using type = detail::type_list_element_t<I, detail::type_list<Args...>>;
};

// clang-format off
template <size_t I, class Return, class... Args>
  requires I < sizeof...(Args)
struct argument_type<I, Return (&)(Args...)> {
  // clang-format on
  using type = detail::type_list_element_t<I, detail::type_list<Args...>>;
};

// clang-format off
#define CVREF_QUALIFY(CV_QUALIFER, REF_QUALIFER)                               \
  template <size_t I, class Return, class Record, class... Args>               \
    requires I < sizeof...(Args)                                               \
  struct argument_type<I,                                                      \
                       Return (Record::*)(Args...) CV_QUALIFER REF_QUALIFER> { \
    using type = detail::type_list_element_t<I, detail::type_list<Args...>>;                    \
  };
#include <satyr/cvref_qualifer.def>
#undef CVREF_QUALIFY
// clang-format on

// clang-format off
template <size_t I, class T>
  requires requires { { &T::operator() } -> auto&&; } && I < arity_v<T>
struct argument_type<I, T> {
  // clang-format on
  using type = typename argument_type<I, decltype(&T::operator())>::type;
};

// clang-format off
template <size_t I, class T>
  requires I < arity_v<T>
using argument_t = typename argument_type<I, std::remove_cv_t<T>>::type;
// clang-format on

//------------------------------------------------------------------------------
// codomain_t
//------------------------------------------------------------------------------
template <class> struct codomain_type {};

template <class Return, class... Args> struct codomain_type<Return(Args...)> {
  using type = Return;
};

template <class Return, class... Args>
struct codomain_type<Return (*)(Args...)> {
  using type = Return;
};

template <class Return, class... Args>
struct codomain_type<Return (&)(Args...)> {
  using type = Return;
};

#define CVREF_QUALIFY(CV_QUALIFER, REF_QUALIFER)                               \
  template <class Return, class Record, class... Args>                         \
  struct codomain_type<Return (Record::*)(Args...) CV_QUALIFER REF_QUALIFER> { \
    using type = Return;                                                       \
  };
#include <satyr/cvref_qualifer.def>
#undef CVREF_QUALIFY

template <class T> requires requires {
  { &T::operator() }
  ->auto&&;
}
struct codomain_type<T> {
  using type = typename codomain_type<decltype(&T::operator())>::type;
};

template <class T>
using codomain_t = typename codomain_type<std::remove_cv_t<T>>::type;

//------------------------------------------------------------------------------
// domain_t
//------------------------------------------------------------------------------
template <class> struct domain_type {};

// clang-format off
template <class Return, class ArgFirst, class... ArgsRest>
  requires (std::is_same_v<ArgFirst, ArgsRest> && ...)
struct domain_type<Return (ArgFirst, ArgsRest...)> {
  // clang-format on
  using type = ArgFirst;
};

// clang-format off
template <class Return, class ArgFirst, class... ArgsRest>
  requires (std::is_same_v<ArgFirst, ArgsRest> && ...)
struct domain_type<Return (*)(ArgFirst, ArgsRest...)> {
  // clang-format on
  using type = ArgFirst;
};

// clang-format off
template <class Return, class ArgFirst, class... ArgsRest>
  requires (std::is_same_v<ArgFirst, ArgsRest> && ...)
struct domain_type<Return (&)(ArgFirst, ArgsRest...)> {
  // clang-format on
  using type = ArgFirst;
};

// clang-format off
#define CVREF_QUALIFY(CV_QUALIFER, REF_QUALIFER)                           \
  template <class Return, class Record, class ArgFirst, class... ArgsRest> \
    requires (std::is_same_v<ArgFirst, ArgsRest> && ...) \
  struct domain_type<Return (Record::*)(ArgFirst, ArgsRest...)             \
                         CV_QUALIFER REF_QUALIFER> {                       \
    using type = ArgFirst;                                                 \
  };
#include <satyr/cvref_qualifer.def>
#undef CVREF_QUALIFY
// clang-format on

template <class T> 
  requires requires {
    { &T::operator() } ->auto&&;
  }
struct domain_type<T> {
  using type = typename domain_type<decltype(&T::operator())>::type;
};

template <class T>
using domain_t = typename domain_type<std::remove_cv_t<T>>::type;

//------------------------------------------------------------------------------
// codomain_value_type_t
//------------------------------------------------------------------------------
template <class T>
using codomain_value_type_t =
    std::remove_cv_t<std::remove_reference_t<codomain_t<T>>>;

//------------------------------------------------------------------------------
// domain_value_type_t
//------------------------------------------------------------------------------
template <class T>
using domain_value_type_t =
    std::remove_cv_t<std::remove_reference_t<domain_t<T>>>;
} // namespace traits
