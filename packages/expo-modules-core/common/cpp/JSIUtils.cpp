// Copyright 2022-present 650 Industries. All rights reserved.

#include <sstream>
#include "JSIUtils.h"

namespace expo::common {

std::shared_ptr<jsi::Function> createClass(jsi::Runtime &runtime, const char *name, ClassConstructor constructor) {
  std::string nativeConstructorKey("__native_constructor__");

  // Create a string buffer of the source code to evaluate.
  std::stringstream source;
  source << "(function " << name << "(...args) { this." << nativeConstructorKey << "(...args); return this; })";
  std::shared_ptr<jsi::StringBuffer> sourceBuffer = std::make_shared<jsi::StringBuffer>(source.str());

  // Evaluate the code and obtain returned value (the constructor function).
  jsi::Object klass = runtime.evaluateJavaScript(sourceBuffer, "").asObject(runtime);

  // Set the native constructor in the prototype.
  jsi::Object prototype = klass.getPropertyAsObject(runtime, "prototype");
  jsi::PropNameID nativeConstructorPropId = jsi::PropNameID::forAscii(runtime, nativeConstructorKey);
  jsi::Function nativeConstructor = jsi::Function::createFromHostFunction(
                                                                          runtime,
                                                                          nativeConstructorPropId,
                                                                          // The paramCount is not obligatory to match, it only affects the `length` property of the function.
                                                                          0,
                                                                          [constructor](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
                                                                            constructor(runtime, thisValue, args, count);
                                                                            return jsi::Value::undefined();
                                                                          });

  jsi::Object descriptor(runtime);
  descriptor.setProperty(runtime, "value", jsi::Value(runtime, nativeConstructor));

  common::definePropertyOnJSIObject(runtime, &prototype, nativeConstructorKey.c_str(), std::move(descriptor));

  return std::make_shared<jsi::Function>(klass.asFunction(runtime));
}

std::shared_ptr<jsi::Object> createInheritingPrototype(jsi::Runtime &runtime, jsi::Function &baseClass) {
  auto prototype = std::make_shared<jsi::Object>(runtime);
  auto baseClassPrototype = baseClass.getProperty(runtime, "prototype");

  prototype->setProperty(runtime, "__proto__", baseClassPrototype);

  return prototype;
}

std::shared_ptr<jsi::Function> createInheritingClass(jsi::Runtime &runtime, const char *className, jsi::Function &baseClass, ClassConstructor constructor) {
  auto prototypePropNameId = jsi::PropNameID::forAscii(runtime, "prototype", 9);
  auto baseClassPrototype = baseClass
    .getProperty(runtime, prototypePropNameId)
    .asObject(runtime);

  auto klass = createClass(runtime, className, constructor);
  auto klassPrototype = klass->getProperty(runtime, prototypePropNameId).asObject(runtime);

  klassPrototype.setProperty(runtime, "__proto__", baseClassPrototype);

  return klass;
}

std::shared_ptr<jsi::Object> createObjectWithPrototype(jsi::Runtime &runtime, std::shared_ptr<jsi::Object> prototype) {
  // Get the "Object" class.
  jsi::Object objectClass = runtime
    .global()
    .getPropertyAsObject(runtime, "Object");

  // Call "Object.create(prototype)" to create an object with the given prototype without calling the constructor.
  jsi::Object object = objectClass
    .getPropertyAsFunction(runtime, "create")
    .callWithThis(runtime, objectClass, {
      jsi::Value(runtime, *prototype)
    })
    .asObject(runtime);

  return std::make_shared<jsi::Object>(std::move(object));
}

std::vector<jsi::PropNameID> jsiArrayToPropNameIdsVector(jsi::Runtime &runtime, const jsi::Array &array) {
  size_t size = array.size(runtime);
  std::vector<jsi::PropNameID> vector;

  vector.reserve(size);

  for (size_t i = 0; i < size; i++) {
    jsi::String name = array.getValueAtIndex(runtime, i).getString(runtime);
    vector.push_back(jsi::PropNameID::forString(runtime, name));
  }
  return vector;
}

void definePropertyOnJSIObject(
  jsi::Runtime &runtime,
  jsi::Object *jsthis,
  const char *name,
  jsi::Object descriptor
) {
  jsi::Object global = runtime.global();
  jsi::Object objectClass = global.getPropertyAsObject(runtime, "Object");
  jsi::Function definePropertyFunction = objectClass.getPropertyAsFunction(
    runtime,
    "defineProperty"
  );

  // This call is basically the same as `Object.defineProperty(object, name, descriptor)` in JS
  definePropertyFunction.callWithThis(runtime, objectClass, {
    jsi::Value(runtime, *jsthis),
    jsi::String::createFromUtf8(runtime, name),
    std::move(descriptor),
  });
}

} // namespace expo::common
