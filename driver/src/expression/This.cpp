/**
 * @file
 */
#include "src/expression/This.hpp"

#include "src/visitor/all.hpp"

birch::This::This(Location* loc) :
    Expression(loc) {
  //
}

birch::This::~This() {
  //
}

bool birch::This::isThis() const {
  return true;
}

birch::Expression* birch::This::accept(Cloner* visitor) const {
  return visitor->clone(this);
}

birch::Expression* birch::This::accept(Modifier* visitor) {
  return visitor->modify(this);
}

void birch::This::accept(Visitor* visitor) const {
  visitor->visit(this);
}
