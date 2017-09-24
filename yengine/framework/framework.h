#ifndef YENGINE_FRAMEWORK_Y_FRAMEWORK_H
#define YENGINE_FRAMEWORK_Y_FRAMEWORK_H

#include <stdint.h>

#include "ycommon/platform/platform_handle.h"
#include "ycommon/platform/thread.h"

namespace ycommon { namespace containers {
class CommandTree;
}}

namespace yengine { namespace framework {

class Framework {
public:
  Framework(const ycommon::platform::PlatformHandle& handle,
             uint32_t global_heap_size = 128 * 1024 * 1024);
  ~Framework();

  // [Common tasks that should be included in the command tree.]
  // Call platform specific function calls, should be called regularly.
  static ycommon::platform::ThreadRoutine GetPlatformUpdateRoutine();
  // Prepare the render, should be called after render device gets all the
  // commands but before actual render call. Best called at the end of the
  // previous frame.
  static ycommon::platform::ThreadRoutine GetPrepareRenderRoutine();
  // Render call, called after Prepare Render to draw any commands prepared.
  // Should probably be called at the beginning of a frame to draw the previous
  // frame.
  static ycommon::platform::ThreadRoutine GetRenderRoutine();

  // [Thread-Safe] Prepares command tree to be swapped at beginning of a frame.
  void SetCommandTree(ycommon::containers::CommandTree* command_tree);

  void Run();

  static void EndFramework();

private:
  // Platform Variables
  ycommon::platform::PlatformHandle mPlatformHandle;

  // Command Tree
  ycommon::containers::CommandTree* mCommandTree = NULL;
  ycommon::containers::CommandTree* volatile mPendingTree = NULL;

  // Game Variables
  uint32_t mGlobalHeapSize;
  // YTimer m_FrameTimer;

  // YTaskManager* m_pTaskManager;

  // Base Game Tasks
  // YGameRenderTask* m_pGameRenderTask;
  // YGameUpdateTask* m_pGameUpdateTask;
};

}}

#endif // YENGINE_FRAMEWORK_Y_FRAMEWORK_H
