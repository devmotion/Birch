/**
 * @file
 */
#include "src/statement/Braces.hpp"

#include "src/visitor/all.hpp"

birch::Braces::Braces(Statement* single, Location* loc) :
    Statement(loc),
    Single<Statement>(single) {
  //
}

birch::Braces::~Braces() {
  //
}

birch::Statement* birch::Braces::strip() {
  return single->strip();
}

birch::Statement* birch::Braces::accept(Cloner* visitor) const {
  return visitor->clone(this);
}

birch::Statement* birch::Braces::accept(Modifier* visitor) {
  return visitor->modify(this);
}

void birch::Braces::accept(Visitor* visitor) const {
  visitor->visit(this);
}
