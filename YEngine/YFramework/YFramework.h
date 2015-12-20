#ifndef YENGINE_YFRAMEWORK_YFRAMEWORK_H
#define YENGINE_YFRAMEWORK_YFRAMEWORK_H

#include <YCommon/YPlatform/PlatformHandle.h>
#include <YCommon/YPlatform/Thread.h>

namespace YCommon { namespace YContainers {
class CommandTree;
}}

namespace YEngine { namespace YFramework {

class YFramework {
public:
  YFramework(const YCommon::YPlatform::PlatformHandle& handle,
             uint32_t global_heap_size = 128 * 1024 * 1024);
  ~YFramework();

  // [Common tasks that should be included in the command tree.]
  // Call platform specific function calls, should be called regularly.
  static YCommon::YPlatform::ThreadRoutine GetPlatformUpdateRoutine();
  // Prepare the render, should be called after render device gets all the
  // commands but before actual render call. Best called at the end of the
  // previous frame.
  static YCommon::YPlatform::ThreadRoutine GetPrepareRenderRoutine();
  // Render call, called after Prepare Render to draw any commands prepared.
  // Should probably be called at the beginning of a frame to draw the previous
  // frame.
  static YCommon::YPlatform::ThreadRoutine GetRenderRoutine();

  // [Thread-Safe] Prepares command tree to be swapped at beginning of a frame.
  void SetCommandTree(YCommon::YContainers::CommandTree* command_tree);

  void Run();

  static void EndFramework();

private:
  // Platform Variables
  YCommon::YPlatform::PlatformHandle mPlatformHandle;

  // Command Tree
  YCommon::YContainers::CommandTree* mCommandTree = NULL;
  YCommon::YContainers::CommandTree* volatile mPendingTree = NULL;

  // Game Variables
  uint32_t mGlobalHeapSize;
  // YTimer m_FrameTimer;

  // YTaskManager* m_pTaskManager;

  // Base Game Tasks
  // YGameRenderTask* m_pGameRenderTask;
  // YGameUpdateTask* m_pGameUpdateTask;
};

}}

#endif // YENGINE_YFRAMEWORK_YFRAMEWORK_H
