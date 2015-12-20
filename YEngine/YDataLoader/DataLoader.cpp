#include <YCommon/Headers/stdincludes.h>
#include "DataLoader.h"

#include <YCommon/YContainers/MemBuffer.h>

#include "ShaderLoad.h"

namespace YEngine { namespace YDataLoader {

namespace {
  YCommon::YContainers::MemBuffer gBuffer;

  struct LoadedData {
    enum DataType {
      kDataType_Shader,
    } mDataType;
    void* mDataItem;
    size_t mUsedBufferSize;

    void Initialize(DataType data_type, const uint8_t* data, size_t data_size) {
      mDataType = data_type;
      mDataItem = nullptr;
      mUsedBufferSize = 0;

      const size_t original_used_space = gBuffer.AllocatedBufferSpace();

      switch (data_type) {
        case kDataType_Shader:
          mDataItem = gBuffer.Allocate(sizeof(ShaderLoad));
          YASSERT(mDataItem != nullptr,
                  "Out of Memory - Could not allocate memory for loaded data.");
          static_cast<ShaderLoad*>(mDataItem)->LoadData(data, data_size,
                                                        &gBuffer);
          break;
      }

      mUsedBufferSize = gBuffer.AllocatedBufferSpace() - original_used_space;
    }

    void Release() {
      switch (mDataType) {
        case kDataType_Shader:
          static_cast<ShaderLoad*>(mDataItem)->Release();
          break;
      }
    }
  };

  bool* gUsedIndexes = nullptr;
  LoadedData* gLoadedData = nullptr;
  uint32_t gNumUsedIndexes = 0;
  uint32_t gMaxDataLoads = 0;
}

void DataLoader::Initialize(void* buffer, size_t buffer_size,
                            uint32_t max_data_loads) {
  gBuffer.Init(buffer, buffer_size);
  gUsedIndexes = static_cast<bool*>(
      gBuffer.Allocate(sizeof(bool) * max_data_loads));
  gLoadedData = static_cast<LoadedData*>(
      gBuffer.Allocate(sizeof(LoadedData) * max_data_loads));
  gNumUsedIndexes = 0;
  gMaxDataLoads = 0;
}

void DataLoader::Terminate() {
  YASSERT(gNumUsedIndexes == 0, "All loaded datas have not been released.");
  gBuffer.Reset();
}

LoadID DataLoader::LoadShader(void* data, size_t data_size) {
  YASSERT(gNumUsedIndexes < gMaxDataLoads,
          "Cannot load more than maximum number (%u).", gMaxDataLoads);
  const uint32_t index = gNumUsedIndexes++;
  gUsedIndexes[index] = true;
  gLoadedData[index].Initialize(LoadedData::kDataType_Shader,
                                static_cast<const uint8_t*>(data),
                                data_size);
  return static_cast<LoadID>(index);
}

void DataLoader::ReleaseLoadedData(LoadID load_id) {
  YASSERT(load_id >= 0 && load_id < gMaxDataLoads,
          "Invalid Load ID: %d", static_cast<int>(load_id));
  YASSERT(gUsedIndexes[load_id],
          "Cannot release load ID: %d", static_cast<int>(load_id));

  gLoadedData[load_id].Release();
  gUsedIndexes[load_id] = false;

  while (gNumUsedIndexes > 0 && !gUsedIndexes[gNumUsedIndexes-1]) {
    gBuffer.Free(gLoadedData[--gNumUsedIndexes].mUsedBufferSize);
  }
}

}} // namespace YEngine { namespace YDataLoader {
