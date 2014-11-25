#ifndef YFRAMEWORK_YFRAMEWORK_H
#define YFRAMEWORK_YFRAMEWORK_H

#include <YCommon/YPlatform/PlatformHandle.h>

namespace YEngine { namespace YFramework {

class GameObject;

class YFramework {
public:
  YFramework(const YCommon::YPlatform::PlatformHandle& handle,
             uint32_t global_heap_size = 128 * 1024 * 1024);
  ~YFramework();

  void RegisterGameObject(GameObject* game_object);
  void Run();

  static void EndFramework();

private:
  // Platform Variables
  YCommon::YPlatform::PlatformHandle mPlatformHandle;

  // Game Variables
  uint32_t mGlobalHeapSize;
  // YTimer m_FrameTimer;

  // YTaskManager* m_pTaskManager;

  // Base Game Tasks
  // YGameRenderTask* m_pGameRenderTask;
  // YGameUpdateTask* m_pGameUpdateTask;
};

}}

#endif // YFRAMEWORK_YFRAMEWORK_H
