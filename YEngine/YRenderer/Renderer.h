#ifndef YENGINE_YRENDERER_RENDERER_H
#define YENGINE_YRENDERER_RENDERER_H

#include <YEngine/YRenderDevice/PixelFormat.h>
#include <YEngine/YRenderDevice/RenderDevice.h>
#include <YEngine/YRenderDevice/VertexDeclElement.h>

namespace YEngine { namespace YRenderDevice {
  struct RenderBlendState;
  struct SamplerState;
}} // namespace YEngine { namespace YRenderDevice {

namespace YEngine { namespace YRenderer {

struct RenderPass;
struct RenderType;

typedef uint8_t ShaderID;
typedef uint8_t RenderTypeID;

namespace Renderer {
  void Initialize(void* buffer, size_t buffer_size);
  void Terminate();

  enum DimensionType {
    kDimensionType_Absolute, // Use absolute numbers.
    kDimensionType_Percentage, // Use percentage of frame buffer dimensions.
  };

  // Register renderer options
  void RegisterViewPort(const char* name, size_t name_size,
                        DimensionType top_type, float top,
                        DimensionType left_type, float left,
                        DimensionType width_type, float width,
                        DimensionType height_type, float height,
                        float min_z, float max_z);
  void RegisterRenderTarget(const char* name, size_t name_size,
                            YRenderDevice::PixelFormat format,
                            DimensionType width_type, float width,
                            DimensionType height_type, float height);
  void RegisterBackBufferName(const char* name, size_t name_size);
  void RegisterRenderPass(const char* name, size_t name_size,
                          const char* shader_variant, size_t variant_size,
                          const YRenderDevice::RenderBlendState& blend_state,
                          const char** render_targets, size_t* target_sizes,
                          size_t num_targets);
  void RegisterVertexDecl(const char* name, size_t name_size,
                          const YRenderDevice::VertexDeclElement* elements,
                          size_t num_elements);
  void RegisterShaderFloatParam(const char* name, size_t name_size,
                                uint8_t num_floats,
                                uint8_t reg, uint8_t reg_offset);
  void RegisterShaderTextureParam(const char* name, size_t name_size,
                                  uint8_t slot,
                                  const YRenderDevice::SamplerState& sampler);
  void RegisterShaderData(const char* shader_name, size_t shader_name_size,
                          const char* variant_name, size_t variant_name_size,
                          const char** vertex_params, size_t num_vertex_params,
                          const void* vertex_shader_data,
                          size_t vertex_shader_size,
                          const char** pixel_params, size_t num_pixel_params,
                          const void* pixel_shader_data,
                          size_t pixel_shader_size);
  void RegisterRenderPasses(const char* name, size_t name_size,
                            const char** render_passes, size_t num_passes);
  void RegisterRenderType(const char* name, size_t name_size,
                          const char* shader, size_t shader_size);
  void RegisterVertexData(const char* name, size_t name_size,
                          size_t num_vertex_elements,
                          const YRenderDevice::VertexElementType* data_types,
                          const YRenderDevice::VertexElementUsage* data_usages,
                          const size_t* vertex_data_sizes,
                          const void** vertex_datas);
  void RegisterShaderArgs(const char* name, size_t name_size, size_t num_args,
                          const char** param_names, size_t* param_name_sizes,
                          const char** arg_names, size_t* arg_name_sizes);
  void RegisterRenderObject(const char* name, size_t name_size,
                            const char* view_port, size_t view_port_size,
                            const char* render_type, size_t render_type_size,
                            const char* shader_args, size_t shader_args_size,
                            const char* vertex_data, size_t vertex_data_size);

  // Release
  bool ReleaseViewPort(const char* name, size_t name_size);
  bool ReleaseRenderTarget(const char* name, size_t name_size);
  bool ReleaseBackBufferName(const char* name, size_t name_size);
  bool ReleaseRenderPass(const char* name, size_t name_size);
  bool ReleaseVertexDecl(const char* name, size_t name_size);
  bool ReleaseShaderFloatParam(const char* name, size_t name_size);
  bool ReleaseShaderTextureParam(const char* name, size_t name_size);
  bool ReleaseShaderData(const char* shader_name, size_t shader_name_size,
                         const char* variant_name, size_t variant_name_size);
  bool ReleaseShaderPasses(const char* name, size_t name_size);
  bool ReleaseRenderType(const char* name, size_t name_size);
  bool ReleaseVertexData(const char* name, size_t name_size);
  bool ReleaseShaderArgs(const char* name, size_t name_size);
  bool ReleaseRenderObject(const char* name, size_t name_size);

  // Activate Render Options
  void ActivateRenderPasses(const char* name, size_t name_size);

  // Enqueue Render Command
  void EnqueueRenderObject(uint64_t render_object_hash);

  // 
}

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_RENDERER_H
