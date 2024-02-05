// Copyright 2024-present 650 Industries. All rights reserved.

#include "SharedObject.h"
#include "JSIUtils.h"

namespace expo::common::SharedObject {

void installBaseClass(jsi::Runtime &runtime, const ObjectReleaser releaser) {
  auto klass = expo::common::createClass(runtime, "SharedObject", {});
  jsi::Object prototype = klass->getPropertyAsObject(runtime, "prototype");

  jsi::Function releaseFunction = jsi::Function::createFromHostFunction(
    runtime,
    jsi::PropNameID::forAscii(runtime, "release"),
    1,
    [releaser](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
      jsi::Object thisObject = args[0].asObject(runtime);

      if (thisObject.hasNativeState<NativeState>(runtime)) {
        auto nativeState = thisObject.getNativeState<NativeState>(runtime);

        releaser(nativeState->objectId);

        // Should we reset the native state?
        thisObject.setNativeState(runtime, nullptr);
      }
      return jsi::Value::undefined();
    });

  klass->setProperty(runtime, "release", releaseFunction);
//  prototype.setProperty(runtime, "release", releaseFunction);

  runtime
    .global()
    .getPropertyAsObject(runtime, "expo")
    .setProperty(runtime, "SharedObject", *klass);
}

jsi::Function getBaseClass(jsi::Runtime &runtime) {
  return runtime
    .global()
    .getPropertyAsObject(runtime, "expo")
    .getPropertyAsFunction(runtime, "SharedObject");
}

std::shared_ptr<jsi::Function> createClass(jsi::Runtime &runtime, const char *className, ClassConstructor constructor) {
  auto baseSharedObjectClass = getBaseClass(runtime);
  return createInheritingClass(runtime, className, baseSharedObjectClass, constructor);
}

NativeState::NativeState(const ObjectId objectId, const ObjectReleaser releaser)
  : objectId(objectId), releaser(releaser), jsi::NativeState() {}

NativeState::~NativeState() {
  releaser(objectId);
}

} // namespace expo::common::SharedObject
