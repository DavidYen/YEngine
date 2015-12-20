#include <YCommon/Headers/stdincludes.h>
#include "SimpleExecutionTree.h"

#include <YCommon/YContainers/CommandTree.h>
#include <YCommon/YPlatform/Platform.h>

namespace YEngine { namespace YFramework {

SimpleExecutionTree::SimpleExecutionTree(void* buffer, size_t buffer_size)
  : YCommon::YContainers::CommandTree(1, buffer, buffer_size) {
}

void SimpleExecutionTree::Setup(YCommon::YPlatform::ThreadRoutine* routines,
                                uint32_t num_routines) {
  
}

}} // namespace YEngine { namespace YFramework {
