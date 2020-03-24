/**
 * @file
 *
 * Standard headers that should be included for all C++ files generated by
 * the compiler.
 */
#pragma once

#include "libbirch/external.hpp"
#include "libbirch/assert.hpp"
#include "libbirch/thread.hpp"
#include "libbirch/memory.hpp"
#include "libbirch/stacktrace.hpp"
#include "libbirch/basic.hpp"
#include "libbirch/class.hpp"
#include "libbirch/type.hpp"

#include "libbirch/SharedPtr.hpp"
#include "libbirch/WeakPtr.hpp"
#include "libbirch/InitPtr.hpp"
#include "libbirch/Lazy.hpp"
#include "libbirch/Dimension.hpp"
#include "libbirch/Index.hpp"
#include "libbirch/Range.hpp"
#include "libbirch/Shape.hpp"
#include "libbirch/Slice.hpp"
#include "libbirch/Array.hpp"
#include "libbirch/Tuple.hpp"
#include "libbirch/Tie.hpp"
#include "libbirch/Any.hpp"
#include "libbirch/Nil.hpp"
#include "libbirch/Optional.hpp"
#include "libbirch/Fiber.hpp"
#include "libbirch/Eigen.hpp"
#include "libbirch/EigenFunctions.hpp"
#include "libbirch/EigenOperators.hpp"

/**
 * LibBirch.
 */
namespace libbirch {
/**
 * Make a range.
 *
 * @ingroup libbirch
 *
 * @param start First index.
 * @param end Last index.
 */
inline Range<> make_range(const int64_t start, const int64_t end) {
  int64_t length = std::max(int64_t(0), end - start + 1);
  return Range<>(start, length);
}

/**
 * Make a shape, no arguments.
 *
 * @ingroup libbirch
 */
inline EmptyShape make_shape() {
  return EmptyShape();
}

/**
 * Make a shape, single argument.
 *
 * @ingroup libbirch
 */
inline Shape<Dimension<>,EmptyShape> make_shape(const int64_t arg) {
  auto tail = EmptyShape();
  auto head = Dimension<>(arg, tail.volume());
  return Shape<Dimension<>,EmptyShape>(head, tail);
}

/**
 * Make a shape, multiple arguments.
 *
 * @ingroup libbirch
 */
template<class ... Args>
auto make_shape(const int64_t arg, Args ... args) {
  auto tail = make_shape(args...);
  auto head = Dimension<>(arg, tail.volume());
  return Shape<decltype(head),decltype(tail)>(head, tail);
}

/**
 * Make a shape, recursively.
 *
 * @ingroup libbirch
 */
template<class ... Args>
auto make_shape(const int64_t arg, const Shape<Args...>& tail) {
  auto head = Dimension<>(arg, tail.volume());
  return Shape<decltype(head),decltype(tail)>(head, tail);
}

/**
 * Make a slice, no arguments.
 *
 * @ingroup libbirch
 */
inline auto make_slice() {
  return EmptySlice();
}

/**
 * Make a slice, single argument.
 *
 * @ingroup libbirch
 */
template<int64_t offset_value, int64_t length_value>
auto make_slice(const Range<offset_value,length_value>& arg) {
  auto head = arg;
  auto tail = make_slice();
  return Slice<decltype(head),decltype(tail)>(head, tail);
}

/**
 * Make a slice, single argument.
 *
 * @ingroup libbirch
 */
inline auto make_slice(const int64_t arg) {
  auto head = Index<>(arg);
  auto tail = EmptySlice();
  return Slice<Index<>,EmptySlice>(head, tail);
}

/**
 * Make a slice, multiple arguments.
 *
 * @ingroup libbirch
 */
template<int64_t offset_value, int64_t length_value, class ... Args>
auto make_slice(const Range<offset_value,length_value>& arg, Args ... args) {
  auto head = arg;
  auto tail = make_slice(args...);
  return Slice<decltype(head),decltype(tail)>(head, tail);
}

/**
 * Make a slice, multiple arguments.
 *
 * @ingroup libbirch
 */
template<class ... Args>
auto make_slice(const int64_t arg, Args ... args) {
  auto head = Index<mutable_value>(arg);
  auto tail = make_slice(args...);
  return Slice<decltype(head),decltype(tail)>(head, tail);
}

/**
 * Make an array.
 *
 * @ingroup libbirch
 *
 * @tparam T Value type.
 * @tparam F Shape type.
 * @tparam Args Constructor parameter types.
 *
 * @param shape Shape.
 * @param args Constructor arguments.
 *
 * @return The array.
 */
template<class T, class F, class ... Args>
auto make_array(const F& shape, Args... args) {
  return Array<T,F>(shape, args...);
}

/**
 * Make an array.
 *
 * @ingroup libbirch
 *
 * @tparam T Value type.
 *
 * @param values Values.
 *
 * @return The array.
 */
template<class T>
auto make_array(const std::initializer_list<T>& values) {
  return Array<T,typename DefaultShape<1>::type>(values);
}

/**
 * Make an array.
 *
 * @ingroup libbirch
 *
 * @tparam T Value type.
 *
 * @param values Values.
 *
 * @return The array.
 */
template<class T>
auto make_array(const std::initializer_list<std::initializer_list<T>>& values) {
  return Array<T,typename DefaultShape<2>::type>(values);
}

/**
 * Make an array.
 *
 * @ingroup libbirch
 *
 * @tparam T Value type.
 * @tparam F Shape type.
 * @tparam L Lambda type.
 *
 * @param l Lambda called to construct each element.
 * @param shape Shape.
 *
 * @return The array.
 */
template<class T, class F, class L>
auto make_array_from_lambda(const F& shape, const L& l) {
  return Array<T,F>(l, shape);
}

/**
 * Make an array and assign a value to it.
 *
 * @ingroup libbirch
 *
 * @tparam T Value type.
 * @tparam F Shape type.
 * @tparam Value Initial value type.
 *
 * @param shape Shape.
 * @param value Initial value.
 *
 * @return The array.
 */
template<class T, class F, class Value>
auto make_array_and_assign(const F& shape, const Value& value) {
  Array<T,F> result;
  result.enlarge(shape, value);
  return result;
}

/**
 * Make a pointer, with in-place object construction.
 *
 * @ingroup libbirch
 *
 * @tparam P Pointer type.
 * @tparam Args Constructor parameter types.
 *
 * @param args Constructor arguments.
 *
 * @return A pointer of the given type.
 */
template<class P, class ... Args>
auto make_pointer(Args... args) {
  return P(new typename P::value_type(args...));
}

/**
 * Make a tuple.
 *
 * @tparam Head First element type.
 * @tparam Tail Remaining element types.
 *
 * @param head First element.
 * @param tail Remaining elements.
 */
template<class Head, class ... Tail>
auto make_tuple(const Head& head, const Tail&... tail) {
  return Tuple<Head,Tail...>(head, tail...);
}

/**
 * Make an assignable tuple.
 *
 * @tparam Head First element type.
 * @tparam Tail Remaining element types.
 *
 * @param head First element.
 * @param tail Remaining elements.
 */
template<class Head, class ... Tail>
auto tie(Head& head, Tail&... tail) {
  return Tie<Head&,Tail&...>(head, tail...);
}

/**
 * Make a value.
 *
 * @tparam T Value type.
 *
 * @return An optional with a default-constructed value of the given type.
 */
template<class T, IS_VALUE(T)>
Optional<T> make() {
  return Optional<T>(T());
}

/**
 * Make an object.
 *
 * @tparam T Pointer type.
 *
 * @return An optional with a value of the given type if that type is
 * a default-constructible class type, otherwise no value.
 */
template<class T, std::enable_if_t<is_pointer<T>::value &&
    std::is_default_constructible<typename T::value_type>::value,int> = 0>
Optional<T> make() {
  return Optional<T>(make_pointer<T>());
}

/**
 * Make an object.
 *
 * @tparam T Pointer type.
 *
 * @return An optional with a value of the given type if that type is
 * a default-constructible class type, otherwise no value.
 */
template<class T, std::enable_if_t<is_pointer<T>::value &&
    !std::is_default_constructible<typename T::value_type>::value,int> = 0>
Optional<T> make() {
  return Optional<T>();
}

/**
 * Identity cast of anything.
 */
template<class To, class From,
    std::enable_if_t<std::is_same<To,From>::value,int> = 0>
Optional<To> cast(const From& from) {
  return from;
}

/**
 * Non-identity cast of a pointer.
 */
template<class To, class From,
    std::enable_if_t<!std::is_same<To,From>::value &&
    is_pointer<To>::value && is_pointer<From>::value,int> = 0>
Optional<To> cast(const From& from) {
  auto ptr = dynamic_cast<typename To::value_type*>(from.get());
  if (ptr) {
    return To(ptr, from.getLabel());
  } else {
    return nil;
  }
}

/**
 * Non-identity cast of a non-pointer.
 */
template<class To, class From,
    std::enable_if_t<!std::is_same<To,From>::value &&
    (!is_pointer<To>::value || !is_pointer<From>::value),int> = 0>
Optional<To> cast(const From& from) {
  return nil;
}

/**
 * Cast of an optional of anything.
 */
template<class To, class From>
Optional<To> cast(const Optional<From>& from) {
  if (from.query()) {
    return cast<To>(from.get());
  } else {
    return nil;
  }
}

}
