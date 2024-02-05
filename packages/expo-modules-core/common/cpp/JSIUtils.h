// Copyright 2022-present 650 Industries. All rights reserved.

#pragma once
#ifdef __cplusplus

#include <jsi/jsi.h>

namespace jsi = facebook::jsi;

namespace expo::common {

/**
 Type of the native constructor of the JS classes.
 */
typedef std::function<void(jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *args, size_t count)> ClassConstructor;

/**
 Creates a class with the given name and native constructor.
 */
std::shared_ptr<jsi::Function> createClass(jsi::Runtime &runtime, const char *name, ClassConstructor constructor);

/**
 Creates a prototype object that has a prototype of the provided class in its prototype chain.
 */
std::shared_ptr<jsi::Object> createInheritingPrototype(jsi::Runtime &runtime, jsi::Function &baseClass);

/**
 Creates a class (function) that inherits from the provided base class.
 */
std::shared_ptr<jsi::Function> createInheritingClass(jsi::Runtime &runtime, const char *className, jsi::Function &baseClass, ClassConstructor constructor);

/**
 Creates an object from the given prototype, without calling the constructor.
 */
std::shared_ptr<jsi::Object> createObjectWithPrototype(jsi::Runtime &runtime, std::shared_ptr<jsi::Object> prototype);

/**
 Converts `jsi::Array` to a vector with prop name ids (`std::vector<jsi::PropNameID>`).
 */
std::vector<jsi::PropNameID> jsiArrayToPropNameIdsVector(jsi::Runtime &runtime, const jsi::Array &array);

/**
 Calls Object.defineProperty(jsThis, name, descriptor)`.
 */
void definePropertyOnJSIObject(
  jsi::Runtime &runtime,
  jsi::Object *jsthis,
  const char *name,
  jsi::Object descriptor
);

} // namespace expo::common

#endif // __cplusplus
