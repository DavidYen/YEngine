#include <YCommon/Headers/stdincludes.h>
#include "YFramework.h"

#include <YCommon/YContainers/CommandTree.h>
#include <YCommon/YPlatform/Platform.h>

namespace YEngine { namespace YFramework {

static volatile bool g_bDone = false;

static uintptr_t PlatformUpdateRoutine(void*) {
  YCommon::YPlatform::Platform::Update();
  return 0;
}

static uintptr_t PrepareRenderRoutine(void*) {
  return 0;
}

static uintptr_t RenderRoutine(void*) {
  return 0;
}

YFramework::YFramework(const YCommon::YPlatform::PlatformHandle& handle,
                       uint32_t global_heap_size)
    : mPlatformHandle(handle),
      mGlobalHeapSize(global_heap_size) {
  YCommon::YPlatform::Platform::Init(handle,
                                     YFramework::EndFramework);
}

YFramework::~YFramework() {
  YCommon::YPlatform::Platform::Release();
}

YCommon::YPlatform::ThreadRoutine YFramework::GetPlatformUpdateRoutine() {
  return PlatformUpdateRoutine;
}

YCommon::YPlatform::ThreadRoutine YFramework::GetPrepareRenderRoutine() {
  return PrepareRenderRoutine;
}

YCommon::YPlatform::ThreadRoutine YFramework::GetRenderRoutine() {
  return RenderRoutine;
}

void YFramework::Run() {
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

void YFramework::EndFramework() {
  g_bDone = true;
}

}} // namespace YEngine { namespace YFramework {
