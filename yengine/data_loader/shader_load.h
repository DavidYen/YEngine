#ifndef YENGINE_DATA_LOADER_SHADER_LOAD_H
#define YENGINE_DATA_LOADER_SHADER_LOAD_H

#include "yengine/renderer/renderer.h"

namespace ycommon { namespace containers {
  class MemBuffer;
}} // namespace ycommon { namespace containers {

namespace yengine { namespace data_loader {

struct ShaderLoad {
  renderer::ShaderID* shader_ids;
  size_t num_shader_ids;

  // Returns amount of buffer used.
  void LoadData(const uint8_t* data, size_t size,
                ycommon::containers::MemBuffer* buffer);
  void Release();
};

}} // namespace yengine { namespace data_loader {

#endif // YENGINE_DATA_LOADER_SHADER_LOAD_H
