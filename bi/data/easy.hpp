/**
 * @file
 *
 * Convenience functions for creating spans, ranges, frames, views, arrays,
 * etc.
 */

#include "bi/data/Span.hpp"
#include "bi/data/Index.hpp"
#include "bi/data/Range.hpp"
#include "bi/data/Frame.hpp"
#include "bi/data/View.hpp"
#include "bi/data/Array.hpp"
#include "bi/data/HeapGroup.hpp"

namespace bi {
/**
 * Make a span.
 *
 * @ingroup library
 *
 * @param length Length.
 */
inline auto make_span(const int_t length) {
  return Span<mutable_value,1,mutable_value>(length);
}

/**
 * Make an index.
 *
 * @ingroup library
 *
 * @param index Index.
 */
inline auto make_index(const int_t i) {
  return Index<mutable_value>(i);
}

/**
 * Make a range.
 *
 * @ingroup library
 *
 * @param start First index.
 * @param end Last index.
 */
inline auto make_range(const int_t start, const int_t end) {
  return Range<mutable_value,mutable_value,1>(start, end - start + 1);
}

/**
 * Make a frame.
 *
 * @ingroup library
 */
//@{
/*
 * No arguments.
 */
inline auto make_frame() {
  return EmptyFrame();
}

/*
 * Single argument.
 */
template<int_t length_value, int_t stride_value, int_t lead_value>
auto make_frame(const Span<length_value,stride_value,lead_value>& arg) {
  auto tail = make_frame();
  auto head = arg;
  return NonemptyFrame<decltype(tail),decltype(head)>(tail, head);
}

inline auto make_frame(const int_t arg) {
  return make_frame(make_span(arg));
}

/*
 * Multiple arguments.
 */
template<int_t length_value, int_t stride_value, int_t lead_value,
    class ... Args>
auto make_frame(const Span<length_value,stride_value,lead_value>& arg,
    Args ... args) {
  return make_frame(make_frame(arg), args...);
}

template<class ... Args>
auto make_frame(const int_t arg, Args ... args) {
  return make_frame(make_frame(arg), args...);
}

/*
 * Tail plus single argument.
 */
template<class Tail, class Head, int_t length_value, int_t stride_value,
    int_t lead_value>
auto make_frame(const NonemptyFrame<Tail,Head>& frame,
    const Span<length_value,stride_value,lead_value>& arg) {
  auto tail = frame;
  auto head = arg;
  return NonemptyFrame<decltype(tail),decltype(head)>(tail, head);
}

template<class Tail, class Head>
auto make_frame(const NonemptyFrame<Tail,Head>& frame, const int_t arg) {
  return make_frame(frame, make_span(arg));
}

/*
 * Tail plus multiple arguments.
 */
template<class Tail, class Head, class Arg, class ... Args>
auto make_frame(const NonemptyFrame<Tail,Head>& tail, const Arg& arg,
    Args ... args) {
  return make_frame(make_frame(tail, arg), args...);
}
//@}

/**
 * Make a view.
 *
 * @ingroup library
 */
//@{
/*
 * No arguments.
 */
inline auto make_view() {
  return EmptyView();
}

/*
 * Single argument.
 */
template<int_t offset_value, int_t length_value, int_t stride_value>
auto make_view(const Range<offset_value,length_value,stride_value>& arg) {
  auto tail = make_view();
  auto head = arg;
  return NonemptyView<decltype(tail),decltype(head)>(tail, head);
}

template<int_t offset_value>
auto make_view(const Index<offset_value>& arg) {
  auto tail = make_view();
  auto head = arg;
  return NonemptyView<decltype(tail),decltype(head)>(tail, head);
}

inline auto make_view(const int_t arg) {
  return make_view(make_index(arg));
}

/*
 * Multiple arguments.
 */
template<int_t offset_value, int_t length_value, int_t stride_value,
    class ... Args>
auto make_view(const Range<offset_value,length_value,stride_value>& arg,
    Args ... args) {
  return make_view(make_view(arg), args...);
}

template<int_t offset_value, class ... Args>
auto make_view(const Index<offset_value>& arg, Args ... args) {
  return make_view(make_view(arg), args...);
}

template<class ... Args>
auto make_view(const int_t arg, Args ... args) {
  return make_view(make_view(arg), args...);
}

/*
 * Tail plus single argument.
 */
template<class Tail, class Head, int_t offset_value, int_t length_value,
    int_t stride_value>
auto make_view(const NonemptyView<Tail,Head>& view,
    const Range<offset_value,length_value,stride_value>& arg) {
  auto tail = view;
  auto head = arg;
  return NonemptyView<decltype(tail),decltype(head)>(tail, head);
}

template<class Tail, class Head, int_t offset_value>
auto make_view(const NonemptyView<Tail,Head>& view,
    const Index<offset_value>& arg) {
  auto tail = view;
  auto head = arg;
  return NonemptyView<decltype(tail),decltype(head)>(tail, head);
}

template<class Tail, class Head>
auto make_view(const NonemptyView<Tail,Head>& view, const int_t arg) {
  return make_view(view, make_index(arg));
}

/*
 * Tail plus multiple arguments.
 */
template<class Tail, class Head, class Arg, class ... Args>
auto make_view(const NonemptyView<Tail,Head>& tail, Arg arg, Args ... args) {
  return make_view(make_view(tail, arg), args...);
}

/**
 * Make an array.
 *
 * @ingroup library
 *
 * @tparam Value Value type.
 * @tparam Frame Frame type.
 *
 * @param frame Frame.
 */
template<class Type, class Frame = EmptyFrame, class Group = HeapGroup>
auto make_array(const Frame& frame = EmptyFrame(), const char* name = nullptr,
    const Group& group = HeapGroup()) {
  return Array<PrimitiveValue<Type,Group>,Frame>(frame, name, group);
}
}