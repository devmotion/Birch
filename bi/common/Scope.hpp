/**
 * @file
 */
#pragma once

#include "bi/common/Dictionary.hpp"
#include "bi/common/OverloadedDictionary.hpp"
#include "bi/common/Named.hpp"
#include "bi/primitive/definitely.hpp"
#include "bi/primitive/possibly.hpp"

namespace bi {
class VarParameter;
class FuncParameter;
class Dispatcher;
class ModelParameter;
class ProgParameter;

class VarReference;
class FuncReference;
class ModelReference;
class ProgReference;

/**
 * Scope.
 *
 * @ingroup compiler_common
 */
class Scope {
public:
  /**
   * Does the scope contain the declaration?
   *
   * @param param Declaration.
   *
   * For functions, matching is done by signature. For all others, matching
   * is done by name only.
   */
  bool contains(VarParameter* param);
  bool contains(FuncParameter* param);
  bool contains(ModelParameter* param);
  bool contains(ProgParameter* param);

  /**
   * Add parameter.
   *
   * @param param Parameter.
   */
  void add(VarParameter* param);
  void add(FuncParameter* param);
  void add(ModelParameter* param);
  void add(ProgParameter* param);

  /**
   * Resolve a reference to a parameter.
   *
   * @param ref Reference to resolve.
   *
   * @return Target of the reference.
   */
  void resolve(VarReference* ref);
  void resolve(FuncReference* ref);
  void resolve(ModelReference* ref);

  /**
   * Inherit another scope into this scope. This is used to import
   * declarations from a base class into a derived class.
   *
   * @param scope Scope to inherit.
   */
  void inherit(Scope* scope);

  /**
   * Import another scope into this scope. This is used to import
   * declarations from one file into another file.
   *
   * @param scope Scope to inherit.
   */
  void import(Scope* scope);

  /**
   * Get the parent of a dispatcher.
   */
  Dispatcher* parent(Dispatcher* o);

  /**
   * Base scopes.
   */
  std::set<Scope*> bases;

  /**
   * Overloaded declarations, by name.
   */
  Dictionary<VarParameter,VarReference> vars;
  Dictionary<ModelParameter,ModelReference> models;
  OverloadedDictionary<FuncParameter,FuncReference,definitely> funcs;
  OverloadedDictionary<Dispatcher,FuncReference,possibly> dispatchers;
  Dictionary<ProgParameter,ProgReference> progs;

private:
  /**
   * Defer resolution to imported scopes.
   */
  template<class ParameterType, class ReferenceType>
  void resolveDefer(ReferenceType* ref);
};
}

template<class ParameterType, class ReferenceType>
void bi::Scope::resolveDefer(ReferenceType* ref) {
  auto iter = bases.begin();
  while (!ref->target && iter != bases.end()) {
    (*iter)->resolve(ref);
    ++iter;
  }
}
