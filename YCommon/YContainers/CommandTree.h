#ifndef YCOMMON_YCONTAINERS_COMMANDTREE_H
#define YCOMMON_YCONTAINERS_COMMANDTREE_H

#include <string>

#include <YCommon/YContainers/ThreadPool.h>
#include <YCommon/YPlatform/Semaphore.h>

#define MAX_DEPENDS 8
#define MAX_NODES 256

namespace YCommon { namespace YContainers {

class CommandTree {
 public:
  CommandTree();
  CommandTree(size_t num_threads, void* buffer, size_t buffer_size);
  ~CommandTree();

  void Initialize(size_t num_threads, void* buffer, size_t buffer_size);

  struct TreeConstructorNode {
    YCommon::YPlatform::ThreadRoutine mCommandRoutine;
    void* mCommandArg;
    uint8_t mNumDependencies;
    uint32_t mDependencies[MAX_DEPENDS]; // Index in nodes array.

    void Initialize(YCommon::YPlatform::ThreadRoutine routine,
                    void* arg,
                    uint8_t num_deps = 0,
                    uint32_t* dep_indexes = NULL) {
      mCommandRoutine = routine;
      mCommandArg = arg;
      mNumDependencies = num_deps;
      memcpy(mDependencies, dep_indexes, sizeof(uint32_t) * num_deps);
    }
    void InitializeWithDependency(YCommon::YPlatform::ThreadRoutine routine,
                                  void* arg,
                                  uint32_t dep_index) {
      mCommandRoutine = routine;
      mCommandArg = arg;
      mNumDependencies = 1;
      mDependencies[0] = dep_index;
    }

  };
  void ConstructTree(uint32_t num_nodes, const TreeConstructorNode* nodes);

  // Returns 0 on success, failures stop execution and returns error code.
  uintptr_t ExecuteCommands(size_t milliseconds = -1, bool* timed_out = NULL);

 private:
  struct CommandData;

 protected:
  struct CommandNode {
    CommandData* mCmdTreeData;
    YCommon::YPlatform::ThreadRoutine mCmdRoutine;
    void* mCmdArg;
    CommandNode** mChildren;
    volatile uint32_t mNumDependenciesFinished;
    uint32_t mNumDependencies;
    uint32_t mNumChildren;

    void Initialize(CommandData* command_tree_data,
                    YCommon::YPlatform::ThreadRoutine command_routine,
                    void* command_arg, uint8_t num_dependencies) {
      mCmdTreeData = command_tree_data;
      mCmdRoutine = command_routine;
      mCmdArg = command_arg;
      mNumDependenciesFinished = 0;
      mNumDependencies = num_dependencies;
      mNumChildren = 0;
      mChildren = NULL;
    }
  };

  static uintptr_t CommandTreeThread(void* arg);

 private:
  void* mBuffer;
  size_t mBufferSize;

  enum CommandTreeState {
    kCommandTreeState_Uninitialized,
    kCommandTreeState_Initialized,
    kCommandTreeState_ReadyToExecute,
    kCommandTreeState_Executing,
  } mCommandTreeState;

  struct CommandData {
    ThreadPool* mThreadPool;
    YPlatform::Semaphore* mSemaphore;
    volatile uintptr_t mReturnValue;
    volatile uint32_t mNumNodesFinished;
    uint32_t mNumNodes;

    CommandData()
        : mThreadPool(NULL),
          mSemaphore(NULL),
          mReturnValue(0),
          mNumNodesFinished(0),
          mNumNodes(0) {
    }

    void Initialize(ThreadPool* thread_pool, YPlatform::Semaphore* semaphore) {
      mThreadPool = thread_pool;
      mSemaphore = semaphore;
      mReturnValue = 0;
      mNumNodesFinished = 0;
      mNumNodes = 0;
    }

    void SetNumNodes(uint32_t num_nodes) {
      mNumNodes = num_nodes;
    }

    void PrepareStart() {
      mReturnValue = 0;
      mNumNodesFinished = 0;
    }
  } mTreeData;

  YPlatform::Semaphore mSemaphore;

  // Root nodes are nodes without any dependencies.
  size_t mNumRootNodes;
  CommandNode** mRootNodes;
};

}} // namespace YCommon { namespace YContainers {

#endif //YCOMMON_YCONTAINERS_COMMANDTREE_H
