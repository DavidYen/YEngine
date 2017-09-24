#include "yengine/framework/simple_execution_tree.h"

#include "ycommon/containers/command_tree.h"
#include "ycommon/platform/platform.h"

namespace yengine { namespace framework {

SimpleExecutionTree::SimpleExecutionTree(void* buffer, size_t buffer_size)
  : ycommon::containers::CommandTree(1, buffer, buffer_size) {
}

void SimpleExecutionTree::Setup(ycommon::platform::ThreadRoutine* routines,
                                uint32_t num_routines) {
  
}

}} // namespace yengine { namespace framework {
