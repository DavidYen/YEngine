#ifndef YENGINE_YFRAMEWORK_COMMANDNODE_H
#define YENGINE_YFRAMEWORK_COMMANDNODE_H

namespace YEngine { namespace YFramework {

struct AllocationData;

// Returns 0 on success, failures returned stop command executions.
typedef uintptr_t (*CommandRoutine)(const AllocationData* in, size_t num_in,
                                    AllocationData* out, size_t num_out);

struct CommandData {
  uint64_t mNameHash;
  CommandRoutine mCommandRoutine;
  uint64_t* mInputDataHashes;
  size_t mNumInputDataHashes;
  uint64_t* mOutputDataHashes;
  size_t mNumOutputDataHashes;
};

}}

#endif // YENGINE_YFRAMEWORK_COMMANDNODE_H
