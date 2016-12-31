/**
 * @file
 */
#pragma once

#include "bi/expression/Expression.hpp"
#include "bi/expression/EmptyExpression.hpp"
#include "bi/primitive/unique_ptr.hpp"

namespace bi {
/**
 * Object with brackets.
 *
 * @ingroup compiler_common
 */
class Bracketed {
public:
  /**
   * Constructor.
   *
   * @param brackets Expression in square brackets.
   */
  Bracketed(Expression* brackets = new EmptyExpression());

  /**
   * Destructor.
   */
  virtual ~Bracketed() = 0;

  /**
   * Square bracket expression.
   */
  unique_ptr<Expression> brackets;
};
}