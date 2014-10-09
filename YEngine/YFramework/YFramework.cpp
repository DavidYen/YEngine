#include <YEngine/stdincludes.h>
#include "YFramework.h"

#include "PlatformHandle.h"

namespace YEngine { namespace YFramework {

static volatile bool g_bDone = false;

YFramework::YFramework(PlatformHandle* platform_handle,
                       uint32_t global_heap_size)
    : mPlatformHandle(platform_handle),
      mGlobalHeapSize(global_heap_size) {
  mPlatformHandle->InitializePlatform();
}

YFramework::~YFramework() {
}

void YFramework::RegisterGameObject(GameObject* game_object) {
  (void) game_object;
}

void YFramework::Run() {
  while( !g_bDone ) {
    mPlatformHandle->ProcessPlatform();

    // m_pTaskManager->InitializeTaskManager();
    // m_pGameUpdateTask->UpdateFrame( m_FrameTimer.MeasureDeltaTime() );

    // m_pTaskManager->QueueTask( m_pGameRenderTask );
    // m_pTaskManager->QueueTask( m_pGameUpdateTask );

    // m_pTaskManager->ExecuteTasks();

    // This call should be made as a queued task after everything is done being put in the renderer and renderer is finished, move it there later
    // g_pRenderDevice->PostRender();
  }
}

void YFramework::EndFramework() {
  g_bDone = true;
}

}} // namespace YEngine { namespace YFramework {
