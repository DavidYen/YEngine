#include <YCommon/Headers/stdincludes.h>
#include "CommandTree.h"

#include <gtest/gtest.h>

namespace YCommon { namespace YContainers {

struct ThreadArg {
  uint32_t expected_num;
  volatile uint32_t* current_num;
};

static uintptr_t ExpectRoutine(void* arg) {
  const uint32_t expected = static_cast<ThreadArg*>(arg)->expected_num;
  volatile uint32_t* cur_num = static_cast<ThreadArg*>(arg)->current_num;

  if (expected == *cur_num) {
    *cur_num += 1;
    return 0;
  }

  return 1;
}

TEST(BasicCommandTreeTest, ConstructorTest) {
  char buffer[10240];
  CommandTree command_tree(2, buffer, sizeof(buffer));
}

TEST(BasicCommandTreeTest, TreeConstructionTest) {
  char buffer[10240];
  CommandTree command_tree(2, buffer, sizeof(buffer));

  // Thread Argument
  const uint32_t magic_num = 123;
  volatile uint32_t current_num = magic_num;
  ThreadArg arg = { magic_num, &current_num };

  // Try simple construction
  CommandTree::TreeConstructorNode tree_node;
  tree_node.mCommandRoutine = ExpectRoutine;
  tree_node.mCommandArg = &arg;
  tree_node.mNumDependencies = 0;
  command_tree.ConstructTree(1, &tree_node);
}

TEST(BasicCommandTreeTest, TreeExecutionTest) {
  char buffer[10240];
  CommandTree command_tree(2, buffer, sizeof(buffer));

  // Thread Argument
  const uint32_t magic_num = 123;
  volatile uint32_t current_num = magic_num;
  ThreadArg arg = { magic_num, &current_num };

  CommandTree::TreeConstructorNode tree_node;
  tree_node.mCommandRoutine = ExpectRoutine;
  tree_node.mCommandArg = &arg;
  tree_node.mNumDependencies = 0;

  // Try empty construction
  command_tree.ConstructTree(1, &tree_node);

  ASSERT_EQ(current_num, magic_num);

  bool timed_out = false;
  const uintptr_t ret = command_tree.ExecuteCommands(100, &timed_out);
  ASSERT_FALSE(timed_out) << "Execution has timed out.";
  ASSERT_EQ(ret, 0) << "Execution did not return 0.";

  ASSERT_EQ(current_num, magic_num + 1) << "Function did not execute.";
}

TEST(BasicCommandTreeTest, TreeExecutionOrder) {
  char buffer[10240];
  CommandTree command_tree(2, buffer, sizeof(buffer));

  // Thread Argument
  const uint32_t magic_num = 123;
  volatile uint32_t current_num = magic_num;

  ThreadArg arg1 = { magic_num, &current_num };
  ThreadArg arg2 = { magic_num + 1, &current_num };

  CommandTree::TreeConstructorNode tree_nodes[2];

  tree_nodes[0].Initialize(ExpectRoutine, &arg1);
  tree_nodes[1].InitializeWithDependency(ExpectRoutine, &arg2, 0);

  // Try empty construction
  command_tree.ConstructTree(ARRAY_SIZE(tree_nodes), tree_nodes);

  ASSERT_EQ(current_num, magic_num);

  bool timed_out = false;
  const uintptr_t ret = command_tree.ExecuteCommands(500, &timed_out);
  ASSERT_FALSE(timed_out) << "Execution has timed out.";
  ASSERT_EQ(ret, 0) << "Execution did not return 0.";

  EXPECT_EQ(current_num, magic_num + 2) << "Commands did not execute in order.";
}

TEST(BasicCommandTreeTest, TreeExecutionMultipleStreams) {
  char buffer[10240];
  CommandTree command_tree(2, buffer, sizeof(buffer));

  // Thread Argument
  const uint32_t magic_num1 = 123;
  const uint32_t magic_num2 = 234;
  volatile uint32_t current_num1 = magic_num1;
  volatile uint32_t current_num2 = magic_num2;

  ThreadArg arg1_1 = { magic_num1, &current_num1 };
  ThreadArg arg1_2 = { magic_num1 + 1, &current_num1 };
  ThreadArg arg1_3 = { magic_num1 + 2, &current_num1 };
  ThreadArg arg2_1 = { magic_num2, &current_num2 };
  ThreadArg arg2_2 = { magic_num2 + 1, &current_num2 };

  CommandTree::TreeConstructorNode tree_nodes[5];

  // Initialize out of order should keep execution order.
  tree_nodes[3].Initialize(ExpectRoutine, &arg2_1);
  tree_nodes[0].InitializeWithDependency(ExpectRoutine, &arg2_2, 3);
  tree_nodes[1].Initialize(ExpectRoutine, &arg1_1);
  tree_nodes[4].InitializeWithDependency(ExpectRoutine, &arg1_2, 1);
  tree_nodes[2].InitializeWithDependency(ExpectRoutine, &arg1_3, 4);

  // Try empty construction
  command_tree.ConstructTree(ARRAY_SIZE(tree_nodes), tree_nodes);

  ASSERT_EQ(current_num1, magic_num1);
  ASSERT_EQ(current_num2, magic_num2);

  bool timed_out = false;
  const uintptr_t ret = command_tree.ExecuteCommands(500, &timed_out);
  ASSERT_FALSE(timed_out) << "Execution has timed out.";
  ASSERT_EQ(ret, 0) << "Execution did not return 0.";

  EXPECT_EQ(current_num1, magic_num1 + 3) << "Commands 1 did not execute.";
  EXPECT_EQ(current_num2, magic_num2 + 2) << "Commands 2 did not execute.";
}

TEST(BasicCommandTreeTest, FailingCommandStopsExecution) {
  char buffer[10240];
  CommandTree command_tree(2, buffer, sizeof(buffer));

  // Thread Argument
  const uint32_t magic_num = 123;
  const uint32_t wrong_num = 234;
  volatile uint32_t current_num = magic_num;

  ThreadArg arg1 = { magic_num, &current_num };
  ThreadArg arg2 = { wrong_num, &current_num };
  ThreadArg arg3 = { magic_num + 1, &current_num };

  CommandTree::TreeConstructorNode tree_nodes[3];

  tree_nodes[0].Initialize(ExpectRoutine, &arg1);
  tree_nodes[1].InitializeWithDependency(ExpectRoutine, &arg2, 0);
  tree_nodes[2].InitializeWithDependency(ExpectRoutine, &arg3, 1);

  // Try empty construction
  command_tree.ConstructTree(ARRAY_SIZE(tree_nodes), tree_nodes);

  ASSERT_EQ(current_num, magic_num);

  bool timed_out = false;
  const uintptr_t ret = command_tree.ExecuteCommands(500, &timed_out);
  ASSERT_FALSE(timed_out) << "Execution has timed out.";
  ASSERT_EQ(ret, 1) << "Execution did not return 1.";

  EXPECT_EQ(current_num, magic_num + 1) << "Commands did not execute at all.";
}

}} // namespace YCommon { namespace YContainers {
