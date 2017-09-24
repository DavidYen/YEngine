#include "yengine/framework/framework.h"

#include "ycommon/containers/command_tree.h"
#include "ycommon/platform/platform.h"
#include "ycommon/utils/assert.h"

namespace yengine { namespace framework {

static volatile bool g_bDone = false;

static uintptr_t PlatformUpdateRoutine(void*) {
  ycommon::platform::Platform::Update();
  return 0;
}

static uintptr_t PrepareRenderRoutine(void*) {
  return 0;
}

static uintptr_t RenderRoutine(void*) {
  return 0;
}

Framework::Framework(const ycommon::platform::PlatformHandle& handle,
                       uint32_t global_heap_size)
    : mPlatformHandle(handle),
      mGlobalHeapSize(global_heap_size) {
  ycommon::platform::Platform::Init(handle,
                                     Framework::EndFramework);
}

Framework::~Framework() {
  ycommon::platform::Platform::Release();
}

ycommon::platform::ThreadRoutine Framework::GetPlatformUpdateRoutine() {
  return PlatformUpdateRoutine;
}

ycommon::platform::ThreadRoutine Framework::GetPrepareRenderRoutine() {
  return PrepareRenderRoutine;
}

ycommon::platform::ThreadRoutine Framework::GetRenderRoutine() {
  return RenderRoutine;
}

void Framework::Run() {
  while( !g_bDone ) {
    if (mPendingTree) {
      mCommandTree = mPendingTree;
      mPendingTree = nullptr;
    }

    if (mCommandTree) {
      uintptr_t ret_value = mCommandTree->ExecuteCommands();
      (void) ret_value;
      YASSERT(ret_value == 0, "Command Tree execution failed.");
    } else {
      PlatformUpdateRoutine(nullptr);
    }
  }
}

void Framework::EndFramework() {
  g_bDone = true;
}

}} // namespace yengine { namespace framework {
