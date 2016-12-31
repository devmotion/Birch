/**
 * @file
 */
#include "bi/common/OverloadedDictionary.hpp"

#include "bi/expression/FuncParameter.hpp"
#include "bi/expression/FuncReference.hpp"
#include "bi/exception/AmbiguousReferenceException.hpp"
#include "bi/exception/PreviousDeclarationException.hpp"

template<class ParameterType, class ReferenceType>
bool bi::OverloadedDictionary<ParameterType,ReferenceType>::contains(
    ParameterType* param) {
  auto iter = overloaded.find(param->name->str());
  return iter != overloaded.end() && iter->second.contains(param);
}

template<class ParameterType, class ReferenceType>
ParameterType* bi::OverloadedDictionary<ParameterType,ReferenceType>::get(
    ParameterType* param) {
  /* pre-condition */
  assert(contains(param));

  auto iter = overloaded.find(param->name->str());
  assert(iter != overloaded.end());
  return iter->second.get(param);
}

template<class ParameterType, class ReferenceType>
void bi::OverloadedDictionary<ParameterType,ReferenceType>::add(
    ParameterType* param) {
  // if any assertions fail here, either the unique names of functions, or
  // the partial order on expressions, is not well-defined

  /* check if already exists */
  if (contains(param)) {
    throw PreviousDeclarationException(param, get(param));
  }

  /* store in ordered list */
  this->ordered.push_back(param);

  /* store in poset */
  auto key = param->name->str();
  auto iter = overloaded.find(key);
  if (iter != overloaded.end()) {
    auto& val = iter->second;
    assert(!val.contains(param));
    val.insert(param);
  } else {
    auto val = poset_type();
    val.insert(param);
    auto pair = std::make_pair(key, val);
    overloaded.insert(pair);
  }
}

template<class ParameterType, class ReferenceType>
ParameterType* bi::OverloadedDictionary<ParameterType,ReferenceType>::resolve(
    const ReferenceType* ref) {
  auto iter = overloaded.find(ref->name->str());
  if (iter == overloaded.end()) {
    throw UnresolvedReferenceException(ref);
  } else {
    std::list<ParameterType*> matches;
    iter->second.find(ref, matches);
    if (matches.size() == 0) {
      throw UnresolvedReferenceException(ref);
    } else if (matches.size() > 1) {
      throw AmbiguousReferenceException(ref, matches);
    } else {
      return matches.front();
    }
  }
}

/*
 * Explicit instantiations.
 */
template class bi::OverloadedDictionary<bi::FuncParameter,bi::FuncReference>;