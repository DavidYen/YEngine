#include "yengine/data_loader/shader_load.h"

#include <schemas/shader_generated.h>

#include "ycommon/containers/mem_buffer.h"

namespace yengine { namespace data_loader {

void ShaderLoad::LoadData(const uint8_t* data, size_t size,
                          ycommon::containers::MemBuffer* buffer) {
  YASSERT(yengine_data::VerifyShaderBuffer(flatbuffers::Verifier(data, size)),
          "Invalid Shader Data.");

  shader_ids = nullptr;
  num_shader_ids = 0;

  const yengine_data::Shader* shader_data = yengine_data::GetShader(data);
  //const char* shader_name = shader_data->name()->c_str();

  const auto variants = shader_data->variants();
  num_shader_ids = variants->size();
  shader_ids = static_cast<renderer::ShaderID*>(
      buffer->Allocate(sizeof(*shader_ids) * num_shader_ids));
  YASSERT(shader_ids, "Out of memory - could not load shader data.");

  /*renderer::ShaderID* shader_id_iter = shader_ids;
  for (auto variant_iter = variants->begin();
       variant_iter != variants->end();
       ++variant_iter) {
    const char* variant_name = variant_iter->name()->c_str();
    *shader_id_iter++ = renderer::Renderer::CreateShader(
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
    renderer::Renderer::ReleaseShader(shader_ids[i]);
  }

  shader_ids = nullptr;
  num_shader_ids = 0;*/
}

}} // namespace yengine { namespace data_loader {
