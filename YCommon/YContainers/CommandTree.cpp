#include <YCommon/Headers/stdincludes.h>
#include "CommandTree.h"

#include <YCommon/Headers/Atomics.h>
#include <YCommon/YPlatform/Timer.h>

namespace YCommon { namespace YContainers {

CommandTree::CommandTree()
    : mBuffer(NULL),
      mBufferSize(0),
      mCommandTreeState(kCommandTreeState_Uninitialized),
      mSemaphore(0, 1),
      mNumRootNodes(0),
      mRootNodes(NULL) {
}

CommandTree::CommandTree(size_t num_threads, void* buffer, size_t buffer_size)
    : mBuffer(NULL),
      mBufferSize(0),
      mCommandTreeState(kCommandTreeState_Uninitialized),
      mSemaphore(0, 1),
      mNumRootNodes(0),
      mRootNodes(NULL) {
  Initialize(num_threads, buffer, buffer_size);
}

CommandTree::~CommandTree() {
}

void CommandTree::Initialize(size_t num_threads,
                             void* buffer, size_t buffer_size) {
  YASSERT(mCommandTreeState == kCommandTreeState_Uninitialized,
          "Command Tree cannot be initialized twice.");

  size_t used_buffer = 0;

  uint8_t* buffer_iter = static_cast<uint8_t*>(buffer);

  void* thread_pool_loc = buffer_iter;
  buffer_iter += sizeof(YCommon::YContainers::ThreadPool);
  used_buffer = buffer_iter - static_cast<uint8_t*>(buffer);
  YASSERT(used_buffer <= buffer_size,
         "Buffer size not large enough to contain thread pool: (%d > %d)",
         static_cast<int>(used_buffer), static_cast<int>(buffer_size));

  uint8_t* thread_pool_data = buffer_iter;
  buffer_iter += sizeof(YCommon::YPlatform::Thread) * num_threads +
                 sizeof(ThreadPool::RunArgs) * MAX_NODES;

  used_buffer = buffer_iter - static_cast<uint8_t*>(buffer);
  YASSERT(used_buffer <= buffer_size,
         "Buffer size not large enough to contain pool data: (%d > %d)",
         static_cast<int>(used_buffer), static_cast<int>(buffer_size));

  ThreadPool* thread_pool = new (thread_pool_loc) ThreadPool(
      num_threads, thread_pool_data, buffer_iter - thread_pool_data);

  mBuffer = buffer_iter;
  mBufferSize = buffer_size - used_buffer;

  mCommandTreeState = kCommandTreeState_Initialized;
  mTreeData.Initialize(thread_pool, &mSemaphore);
}

void CommandTree::ConstructTree(uint32_t num_nodes,
                                const TreeConstructorNode* nodes) {
  YASSERT(mCommandTreeState == kCommandTreeState_Initialized,
          "Command Tree cannot be initialized twice.");

  uint8_t* buffer_iter = static_cast<uint8_t*>(mBuffer);
  uint8_t* buffer_end = buffer_iter + mBufferSize;

  // Initialize all the Command Nodes first.
  CommandNode* command_nodes = reinterpret_cast<CommandNode*>(buffer_iter);
  buffer_iter += sizeof(CommandNode) * num_nodes;
  YASSERT(buffer_iter <= buffer_end,
          "Buffer size not large enough to contain command nodes.");
  for (size_t i = 0; i < num_nodes; ++i) {
    const TreeConstructorNode& node = nodes[i];
    YASSERT(node.mNumDependencies <= MAX_DEPENDS,
            "Maximum number of dependencies per node exceeded: %d > %d",
            static_cast<int>(node.mNumDependencies),
            static_cast<int>(MAX_DEPENDS));

    command_nodes[i].Initialize(&mTreeData, node.mCommandRoutine,
                                node.mCommandArg, node.mNumDependencies);
  }

  // Count the number of children per node and root nodes.
  size_t total_num_dependencies = 0;
  size_t num_root_nodes = 0;
  for (size_t i = 0; i < num_nodes; ++i) {
    const TreeConstructorNode& node = nodes[i];
    const uint8_t num_dependencies = node.mNumDependencies;
    if (num_dependencies) {
      for (uint8_t n = 0; n < num_dependencies; ++n) {
        const uint32_t dependent_index = node.mDependencies[n];
        YASSERT(dependent_index < num_nodes,
                "Invalid Dependent Node Index: %d. Max %d.",
                static_cast<int>(dependent_index), static_cast<int>(num_nodes));

        command_nodes[dependent_index].mNumChildren++;
      }

      total_num_dependencies += num_dependencies;
    } else {
      num_root_nodes++;
    }
  }

  // Allocate root nodes pointers
  CommandNode** root_nodes = reinterpret_cast<CommandNode**>(buffer_iter);
  buffer_iter += num_root_nodes * sizeof(CommandNode*);
  YASSERT(buffer_iter <= buffer_end,
          "Buffer size not large enough to contain root nodes.");

  mNumRootNodes = num_root_nodes;
  mRootNodes = root_nodes;

  // Allocate children nodes.
  CommandNode** children_nodes = reinterpret_cast<CommandNode**>(buffer_iter);
  buffer_iter += total_num_dependencies * sizeof(CommandNode*);
  YASSERT(buffer_iter <= buffer_end,
          "Buffer size not large enough to contain all children nodes.");

  int used_children_nodes = 0;
  for (size_t i = 0; i < num_nodes; ++i) {
    CommandNode& command_node = command_nodes[i];

    if (command_node.mNumChildren) {
      command_node.mChildren = &children_nodes[used_children_nodes];
      used_children_nodes += command_node.mNumChildren;
    }
  }

  // Assign Children Nodes and root nodes.
  int current_root_nodes = 0;
  int current_children_nodes_array[MAX_NODES] = { 0 };
  for (size_t i = 0; i < num_nodes; ++i) {
    CommandNode& command_node = command_nodes[i];

    const TreeConstructorNode& node = nodes[i];
    const uint8_t num_dependencies = node.mNumDependencies;
    if (num_dependencies) {
      for (uint8_t n = 0; n < num_dependencies; ++n) {
        const uint32_t dep_index = node.mDependencies[n];
        int& children_nodes = current_children_nodes_array[dep_index];
        command_nodes[dep_index].mChildren[children_nodes++] = &command_node;
      }
    } else {
      root_nodes[current_root_nodes++] = &command_node;
    }
  }

  mTreeData.SetNumNodes(num_nodes);
  mCommandTreeState = kCommandTreeState_ReadyToExecute;
}

// Returns 0 on success, failures stop execution and returns error code.
uintptr_t CommandTree::ExecuteCommands(size_t milliseconds, bool* timed_out) {
  YPlatform::Timer timer;
  timer.Start();

  bool ret = false;

  mTreeData.PrepareStart();
  YASSERT(mCommandTreeState == kCommandTreeState_ReadyToExecute,
          "Command Tree is not ready to execute yet.");
  mCommandTreeState = kCommandTreeState_Executing;

  ret = mTreeData.mThreadPool->Start();
  YASSERT(ret, "Thread Pool failed to start.");

  for (size_t i = 0; i < mNumRootNodes; ++i) {
    ret = mTreeData.mThreadPool->EnqueueRun(CommandTreeThread, mRootNodes[i]);
    YASSERT(ret, "Thread Pool failed to enqueue root node.");
  }

  uint64_t milli64 = static_cast<uint64_t>(milliseconds);
  do {
    // Update time used
    if (milli64 != static_cast<uint64_t>(-1)) {
      timer.Pulse();
      const uint64_t waited = timer.GetPulsedTimeMilli();
      if (waited >= milli64)
        break;
      milli64 -= waited;
    }

    if (!mSemaphore.Wait(static_cast<size_t>(milli64))) {
      break;
    }

    // Update time used
    if (milli64 != static_cast<uint64_t>(-1)) {
      timer.Pulse();
      const uint64_t waited = timer.GetPulsedTimeMilli();
      if (waited >= milli64)
        break;
      milli64 -= waited;
    }

    mTreeData.mThreadPool->Pause();
    if (!mTreeData.mThreadPool->Join(static_cast<size_t>(milli64)))
      break;

    if (mTreeData.mReturnValue == 0) {
      mCommandTreeState = kCommandTreeState_ReadyToExecute;
    } else {
      // Must be reinitialized.
      mCommandTreeState = kCommandTreeState_Initialized;
    }
    return mTreeData.mReturnValue;
  } while (false);

  // Timed out
  if (timed_out)
    *timed_out = true;
  return 1;
}

uintptr_t CommandTree::CommandTreeThread(void* arg) {
  CommandNode* node_data = static_cast<CommandNode*>(arg);
  CommandData* tree_data = node_data->mCmdTreeData;

  // Reinitialize node data.
  node_data->mNumDependenciesFinished = 0;

  // Execute the command.
  const uintptr_t ret_value = node_data->mCmdRoutine(node_data->mCmdArg);
  if (ret_value != 0) {
    tree_data->mReturnValue = ret_value;
    tree_data->mSemaphore->Release();
    return 0;
  }

  // If total number of nodes are done executing, finish.
  const uint32_t num_nodes = tree_data->mNumNodes;
  if (AtomicAdd32(&tree_data->mNumNodesFinished, 1) == num_nodes - 1) {
    tree_data->mSemaphore->Release();
    return 0;
  }

  // If return value is non-zero, do not enqueue more commands.
  if (tree_data->mReturnValue) {
    return 0;
  }

  // Everything was normal, enqueue any dependent children.
  const uint32_t num_children = node_data->mNumChildren;
  for (uint32_t i = 0; i < num_children; ++i) {
    CommandNode* dep_node = node_data->mChildren[i];
    const uint32_t num_deps = dep_node->mNumDependencies;
    if (AtomicAdd32(&dep_node->mNumDependenciesFinished, 1) == num_deps-1) {
      bool ret = tree_data->mThreadPool->EnqueueRun(
          CommandTreeThread, dep_node);
      YASSERT(ret, "Thread Pool failed to enqueue dependent node.");
    }
  }

  return 0;
}

}} // namespace YCommon { namespace YContainers {
