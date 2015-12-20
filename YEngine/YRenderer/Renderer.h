#ifndef YENGINE_YRENDERER_RENDERER_H
#define YENGINE_YRENDERER_RENDERER_H

#include <YCommon/YContainers/RefPointer.h>
#include <YEngine/YRenderDevice/PixelFormat.h>
#include <YEngine/YRenderDevice/RenderDevice.h>
#include <YEngine/YRenderDevice/VertexDeclElement.h>

#include "RendererCommon.h"

using YCommon::YContainers::ReadRefData;
using YCommon::YContainers::TypedReadRefData;

namespace YEngine { namespace YRenderDevice {
  struct RenderBlendState;
  struct SamplerState;
}} // namespace YEngine { namespace YRenderDevice {

namespace YEngine { namespace YRenderer {

struct RenderKeyField;

typedef uint8_t ShaderID;
typedef uint8_t RenderTypeID;

namespace Renderer {
  void Initialize(void* buffer, size_t buffer_size);
  void Terminate();

  void SetupRenderKey(const RenderKeyField* fields, size_t num_fields);

  // Register renderer options, default values do not overwrite settings.
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
                                uint8_t num_floats, uint8_t reg);
  void RegisterShaderTextureParam(const char* name, size_t name_size,
                                  uint8_t slot,
                                  const YRenderDevice::SamplerState& sampler);
  void RegisterShaderData(const char* shader_name, size_t shader_name_size,
                          const char* variant_name, size_t variant_name_size,
                          const char* vertex_decl, size_t vertex_decl_size,
                          size_t num_vertex_params,
                          const char** vertex_params,
                          size_t* vertex_param_sizes,
                          const void* vertex_shader_data,
                          size_t vertex_shader_size,
                          size_t num_pixel_params,
                          const char** pixel_params, size_t* pizel_param_sizes,
                          const void* pixel_shader_data,
                          size_t pixel_shader_size);
  void RegisterRenderPasses(const char* name, size_t name_size,
                            const char** render_passes,
                            const size_t* render_pass_sizes, size_t num_passes);
  void RegisterRenderType(const char* name, size_t name_size,
                          const char* shader, size_t shader_size);
  void RegisterVertexData(const char* name, size_t name_size);
  void RegisterShaderArg(const char* name, size_t name_size,
                         const char* param, size_t param_size);
  void RegisterGlobalArg(const char* param, size_t param_size,
                         const char* arg, size_t arg_size);
  void RegisterRenderObject(const char* name, size_t name_size,
                            const char* view_port, size_t view_port_size,
                            const char* render_type, size_t render_type_size,
                            const char* vertex_data, size_t vertex_data_size,
                            size_t num_shader_args,
                            const char** shader_args, size_t* shader_arg_sizes);

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
  bool ReleaseRenderPasses(const char* name, size_t name_size);
  bool ReleaseRenderType(const char* name, size_t name_size);
  bool ReleaseVertexData(const char* name, size_t name_size);
  bool ReleaseShaderArg(const char* name, size_t name_size);
  bool ReleaseGlobalArg(const char* param, size_t param_size);
  bool ReleaseRenderObject(const char* name, size_t name_size);

  // Activate/Deactivate Render Options
  void ActivateRenderPasses(const char* name, size_t name_size);
  void DeactivateRenderPasses();

  // Set Render Data Here, these store the pointers and uploads data.
  void SetVertexData(
      uint64_t vertex_data_hash,
      YRenderDevice::DrawPrimitive draw_primitive,
      YRenderDevice::UsageType usage_type,
      size_t num_vertex_elements,
      TypedReadRefData<YRenderDevice::VertexElementType> data_types,
      TypedReadRefData<YRenderDevice::VertexElementUsage> data_usages,
      TypedReadRefData<uint16_t> index_data,
      TypedReadRefData<float> vertex_datas,
      TypedReadRefData<uint32_t> vertex_counts);
  void AppendVertexData(
      uint64_t vertex_data_hash,
      YRenderDevice::DrawPrimitive draw_primitive,
      YRenderDevice::UsageType usage_type,
      size_t num_vertex_elements,
      TypedReadRefData<YRenderDevice::VertexElementType> data_types,
      TypedReadRefData<YRenderDevice::VertexElementUsage> data_usages,
      TypedReadRefData<size_t> data_sizes,
      TypedReadRefData<uint16_t> index_data,
      TypedReadRefData<float> vertex_datas);
  void SetShaderArg(
      uint64_t shader_arg_hash,
      YRenderDevice::UsageType usage_type,
      size_t shader_arg_size,
      ReadRefData shader_arg_data);

  // Enqueue Render Command
  void EnqueueRenderObject(uint64_t render_object_hash);

  // Execution Commands (These are meant to run on separate threads)
  void PrepareDraw();

  void ExecuteUploads();
  void ExecuteDraws();
}

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_RENDERER_H
