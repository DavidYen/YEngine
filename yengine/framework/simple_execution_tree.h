#ifndef YENGINE_FRAMEWORK_SIMPLE_EXECUTION_TREE_H
#define YENGINE_FRAMEWORK_SIMPLE_EXECUTION_TREE_H

#include "ycommon/containers/command_tree.h"
#include "ycommon/platform/thread.h"

namespace yengine { namespace framework {

class SimpleExecutionTree : public ycommon::containers::CommandTree {
 public:
  SimpleExecutionTree(void* buffer, size_t buffer_size);
  ~SimpleExecutionTree();

  void Setup(ycommon::platform::ThreadRoutine* routines,
             uint32_t num_routines);

 private:
  
};

}}

#endif // YENGINE_FRAMEWORK_SIMPLE_EXECUTION_TREE_H
