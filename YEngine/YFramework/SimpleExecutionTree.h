#ifndef YENGINE_YFRAMEWORK_SIMPLEEXECUTIONTREE_H
#define YENGINE_YFRAMEWORK_SIMPLEEXECUTIONTREE_H

#include <YCommon/YContainers/CommandTree.h>
#include <YCommon/YPlatform/Thread.h>

namespace YEngine { namespace YFramework {

class SimpleExecutionTree : public YCommon::YContainers::CommandTree {
 public:
  SimpleExecutionTree(void* buffer, size_t buffer_size);
  ~SimpleExecutionTree();

  void Setup(YCommon::YPlatform::ThreadRoutine* routines,
             uint32_t num_routines);

 private:
  
};

}}

#endif // YENGINE_YFRAMEWORK_SIMPLEEXECUTIONTREE_H
