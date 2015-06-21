#ifndef YENGINE_YDATALOADER_DATALOADER_H
#define YENGINE_YDATALOADER_DATALOADER_H

namespace YEngine { namespace YDataLoader {

typedef uint32_t LoadID;

namespace DataLoader {
  void Initialize(void* buffer, size_t buffer_size, uint32_t max_data_loads);
  void Terminate();

  LoadID LoadShader(void* data, size_t data_size);

  void ReleaseLoadedData(LoadID load_id);
}

}} // namespace YEngine { namespace YDataLoader {

#endif // YENGINE_YDATALOADER_DATALOADER_H
