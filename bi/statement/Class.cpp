/**
 * @file
 */
#include "bi/statement/Class.hpp"

#include "bi/visitor/all.hpp"

bi::Class::Class(shared_ptr<Name> name, Type* base, Statement* braces,
    shared_ptr<Location> loc) :
    Statement(loc),
    Named(name),
    Based(base),
    Braced(braces) {
  //
}

bi::Class::~Class() {
  //
}

bi::Statement* bi::Class::accept(Cloner* visitor) const {
  return visitor->clone(this);
}

bi::Statement* bi::Class::accept(Modifier* visitor) {
  return visitor->modify(this);
}

void bi::Class::accept(Visitor* visitor) const {
  visitor->visit(this);
}

void bi::Class::addSuper(const Class* o) {
  supers.insert(o);
  for (auto x : o->supers) {
    supers.insert(x);
  }
}

bool bi::Class::hasSuper(const Class* o) const {
  return supers.find(o) != supers.end();
}

void bi::Class::addConversion(const Type* o) {
  conversions.push_back(o);
}

bool bi::Class::hasConversion(const Type* o) const {
  for (auto x : conversions) {
    if (o->equals(*x)) {
      return true;
    }
  }
  return false;
}

void bi::Class::addAssignment(const Type* o) {
  assignments.push_back(o);
}

bool bi::Class::hasAssignment(const Type* o) const {
  for (auto x : assignments) {
    if (o->definitely(x)) {
      return true;
    }
  }
  return false;
}

bool bi::Class::dispatchDefinitely(const Statement& o) const {
  return o.definitely(*this);
}

bool bi::Class::definitely(const Class& o) const {
  return true;
}

bool bi::Class::dispatchPossibly(const Statement& o) const {
  return o.possibly(*this);
}

bool bi::Class::possibly(const Class& o) const {
  return true;
}
