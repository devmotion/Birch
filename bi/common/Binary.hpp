/**
 * @file
 */
#pragma once

#include "bi/expression/Expression.hpp"
#include "bi/primitive/unique_ptr.hpp"

namespace bi {
/**
 * Object with left and right operands.
 *
 * @ingroup compiler_type
 */
template<class T>
class Binary {
public:
  /**
   * Constructor.
   *
   * @param left Left operand.
   * @param right Right operand.
   */
  explicit Binary(T* left, T* right);

  /**
   * Destructor.
   */
  virtual ~Binary() = 0;

  /**
   * Left operand.
   */
  unique_ptr<T> left;

  /**
   * Right operand.
   */
  unique_ptr<T> right;
};

/**
 * Expression binary.
 *
 * @ingroup compiler_common
 */
typedef Binary<Expression> ExpressionBinary;

/**
 * Type binary.
 *
 * @ingroup compiler_common
 */
typedef Binary<Type> TypeBinary;

}