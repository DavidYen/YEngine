#ifndef YENGINE_DATA_LOADER_DATA_LOADER_H
#define YENGINE_DATA_LOADER_DATA_LOADER_H

#include <stdint.h>

namespace yengine { namespace data_loader {

typedef uint32_t LoadID;

namespace DataLoader {
  void Initialize(void* buffer, size_t buffer_size, uint32_t max_data_loads);
  void Terminate();

  LoadID LoadShader(void* data, size_t data_size);

  void ReleaseLoadedData(LoadID load_id);
}

}} // namespace yengine { namespace data_loader {

#endif // YENGINE_DATA_LOADER_DATA_LOADER_H
