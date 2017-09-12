#include <YCommon/Headers/stdincludes.h>
#include "ShaderLoad.h"

#include <schemas/shader_generated.h>
#include <YCommon/YContainers/MemBuffer.h>

namespace YEngine { namespace YDataLoader {

void ShaderLoad::LoadData(const uint8_t* data, size_t size,
                          YCommon::YContainers::MemBuffer* buffer) {
  YASSERT(YEngineData::VerifyShaderBuffer(flatbuffers::Verifier(data, size)),
          "Invalid Shader Data.");

  shader_ids = nullptr;
  num_shader_ids = 0;

  const YEngineData::Shader* shader_data = YEngineData::GetShader(data);
  //const char* shader_name = shader_data->name()->c_str();

  const auto variants = shader_data->variants();
  num_shader_ids = variants->size();
  shader_ids = static_cast<YRenderer::ShaderID*>(
      buffer->Allocate(sizeof(*shader_ids) * num_shader_ids));
  YASSERT(shader_ids, "Out of memory - could not load shader data.");

  /*YRenderer::ShaderID* shader_id_iter = shader_ids;
  for (auto variant_iter = variants->begin();
       variant_iter != variants->end();
       ++variant_iter) {
    const char* variant_name = variant_iter->name()->c_str();
    *shader_id_iter++ = YRenderer::Renderer::CreateShader(
        shader_name, variant_name,
        variant_iter->vertex_shader()->Data(),
        variant_iter->vertex_shader()->size(),
        variant_iter->pixel_shader()->Data(),
        variant_iter->pixel_shader()->size());
  }*/
}

void ShaderLoad::Release() {
  /*const size_t num_shaders = num_shader_ids;
  for (size_t i = 0; i < num_shaders; ++i) {
    YRenderer::Renderer::ReleaseShader(shader_ids[i]);
  }

  shader_ids = nullptr;
  num_shader_ids = 0;*/
}

}} // namespace YEngine { namespace YDataLoader {
