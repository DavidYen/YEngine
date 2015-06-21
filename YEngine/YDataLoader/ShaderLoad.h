#ifndef YENGINE_YDATALOADER_SHADERLOAD_H
#define YENGINE_YDATALOADER_SHADERLOAD_H

#include <YEngine/YRenderer/Renderer.h>

namespace YCommon { namespace YContainers {
  class MemBuffer;
}} // namespace YCommon { namespace YContainers {

namespace YEngine { namespace YDataLoader {

struct ShaderLoad {
  YRenderer::ShaderID* shader_ids;
  size_t num_shader_ids;

  // Returns amount of buffer used.
  void LoadData(const uint8_t* data, size_t size,
                YCommon::YContainers::MemBuffer* buffer);
  void Release();
};

}} // namespace YEngine { namespace YDataLoader {

#endif // YENGINE_YDATALOADER_SHADERLOAD_H
