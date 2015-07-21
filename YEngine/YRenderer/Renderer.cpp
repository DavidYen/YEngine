#include <YCommon/Headers/stdincludes.h>
#include "Renderer.h"

#include <algorithm>

#include <YCommon/YContainers/HashTable.h>
#include <YCommon/YContainers/MemBuffer.h>
#include <YCommon/YUtils/Hash.h>
#include <YEngine/YCore/StringTable.h>
#include <YEngine/YRenderDevice/RenderBlendState.h>
#include <Yengine/YRenderDevice/SamplerState.h>

// Hash Table Sizes (~2x maximum)
#define VIEWPORTS_SIZE 16
#define RENDERTARGETS_SIZE 16
#define BACKBUFFERNAMES_SIZE 12
#define BLENDSTATES_SIZE 32
#define RENDERPASSES_SIZE 64
#define VERTEXDECLS_SIZE 32
#define FLOATPARAMS_SIZE 256
#define TEXTUREPARAMS_SIZE 256
#define VERTEX_SHADERS_SIZE 256
#define PIXEL_SHADERS_SIZE 256
#define SHADER_COMBINATIONS_SIZE 512
#define ACTIVE_PASSES_SIZE 16
#define RENDER_TYPES_SIZE 128

// Maximum active
#define MAX_RENDERTARGETS_PER_PASS 4
#define MAX_SHADER_BASE_NAME 32
#define MAX_SHADER_VARIANT_NAME 32
#define MAX_VERTEX_ELEMENTS 8
#define MAX_FLOAT_PARAMS_PER_SHADER 16
#define MAX_TEX_PARAMS_PER_SHADER 8
#define MAX_ACTIVE_RENDERPASSES 8

#define INVALID_VIEWPORT static_cast<YRenderDevice::ViewPortID>(-1)
#define INVALID_RENDER_TARGET static_cast<YRenderDevice::RenderTargetID>(-1)
#define INVALID_BLEND_STATE static_cast<YRenderDevice::RenderBlendStateID>(-1)
#define INVALID_VERTEX_DECL static_cast<YRenderDevice::VertexDeclID>(-1)

namespace YEngine { namespace YRenderer {

namespace {
  YCommon::YContainers::MemBuffer gMemBuffer;

  struct RefCountBase {
    RefCountBase() : mRefCount(0) {}
    void IncRef() { mRefCount++; }
    bool DecRef() { return --mRefCount == 0; }

   protected:
    int32_t mRefCount;
  };

  struct ViewPortInternal : public RefCountBase {
    ViewPortInternal(Renderer::DimensionType top_type, float top,
                     Renderer::DimensionType left_type, float left,
                     Renderer::DimensionType width_type, float width,
                     Renderer::DimensionType height_type, float height,
                     float min_z, float max_z)
      : RefCountBase(),
        mTopType(top_type),
        mTop(top),
        mLeftType(left_type),
        mLeft(left),
        mWidthType(width_type),
        mWidth(width),
        mHeightType(height_type),
        mHeight(height),
        mMinZ(min_z),
        mMaxZ(max_z),
        mViewPortID(INVALID_VIEWPORT) {
    }

    Renderer::DimensionType mTopType, mLeftType, mWidthType, mHeightType;
    float mTop, mLeft, mWidth, mHeight, mMinZ, mMaxZ;
    YRenderDevice::ViewPortID mViewPortID;
  };
  YCommon::YContainers::TypedHashTable<ViewPortInternal> gViewPorts;

  struct RenderTargetInternal : public RefCountBase {
    RenderTargetInternal(YRenderDevice::PixelFormat format,
                         Renderer::DimensionType width_type, float width,
                         Renderer::DimensionType height_type, float height)
      : mWidthType(width_type),
        mWidth(width),
        mHeightType(height_type),
        mHeight(height),
        mFormat(format),
        mRenderTargetID(INVALID_RENDER_TARGET) {
    }

    Renderer::DimensionType mWidthType, mHeightType;
    float mWidth, mHeight;
    YRenderDevice::PixelFormat mFormat;
    YRenderDevice::RenderTargetID mRenderTargetID;
  };
  YCommon::YContainers::TypedHashTable<RenderTargetInternal> gRenderTargets;

  struct BackBufferInternal : public RefCountBase {
    BackBufferInternal() : RefCountBase() {}
  };
  YCommon::YContainers::TypedHashTable<BackBufferInternal> gBackBufferNames;

  struct BlendStateInternal : public RefCountBase {
    BlendStateInternal(const YRenderDevice::RenderBlendState& blend_state)
      : mBlendState(blend_state),
        mBlendStateID(INVALID_BLEND_STATE) {
    }

    YRenderDevice::RenderBlendState mBlendState;
    YRenderDevice::RenderBlendStateID mBlendStateID;
  };
  YCommon::YContainers::FullTypedHashTable<YRenderDevice::RenderBlendState,
                                           BlendStateInternal> gBlendStates;

  struct RenderPassInternal : public RefCountBase {
    RenderPassInternal(BlendStateInternal* blend_state,
                       const char* shader_variant,
                       uint8_t shader_variant_size,
                       RenderTargetInternal** render_targets,
                       uint8_t num_render_targets)
      : RefCountBase(),
        mBlendState(blend_state),
        mNumRenderTargets(num_render_targets),
        mShaderVariantSize(shader_variant_size) {
      YASSERT(num_render_targets < ARRAY_SIZE(mRenderTargets),
              "Maximum render targets exceeded");
      YASSERT(shader_variant_size < ARRAY_SIZE(mShaderVariant),
              "Maximum render targets exceeded");
      memset(mRenderTargets, 0, sizeof(mRenderTargets));
      memcpy(mRenderTargets, render_targets,
             num_render_targets * sizeof(RenderTargetInternal*));
      memset(mShaderVariant, 0, sizeof(mShaderVariant));
      memcpy(mShaderVariant, shader_variant, shader_variant_size);
    }

    RenderTargetInternal* mRenderTargets[MAX_RENDERTARGETS_PER_PASS];
    BlendStateInternal* mBlendState;
    char mShaderVariant[MAX_SHADER_VARIANT_NAME];
    uint8_t mNumRenderTargets;
    uint8_t mShaderVariantSize;
  };
  YCommon::YContainers::TypedHashTable<RenderPassInternal> gRenderPasses;

  struct VertexDeclInternal : public RefCountBase {
    VertexDeclInternal(const YRenderDevice::VertexDeclElement* elements,
                       uint8_t num_elements)
      : RefCountBase(),
        mNumVertexElements(num_elements),
        mVertexDeclID(INVALID_VERTEX_DECL) {
      YASSERT(num_elements < MAX_VERTEX_ELEMENTS,
              "Maximum vertex elements (%d) exceeded: %d",
              static_cast<int>(MAX_VERTEX_ELEMENTS),
              static_cast<int>(num_elements));
      memset(mVertexElements, 0, sizeof(mVertexElements));
      memcpy(mVertexElements, elements, num_elements * sizeof(elements[0]));
    }

    YRenderDevice::VertexDeclElement mVertexElements[MAX_VERTEX_ELEMENTS];
    uint8_t mNumVertexElements;
    YRenderDevice::VertexDeclID mVertexDeclID;
  };
  YCommon::YContainers::TypedHashTable<VertexDeclInternal> gVertexDecls;

  struct ShdrFloatParamInternal : public RefCountBase {
    ShdrFloatParamInternal(uint8_t num_floats,
                           uint8_t reg, uint8_t reg_offset)
      : RefCountBase(),
        mNumFloats(num_floats),
        mReg(reg),
        mRegOffset(reg_offset) {
    }

    uint8_t mNumFloats;
    uint8_t mReg;
    uint8_t mRegOffset;
  };
  YCommon::YContainers::TypedHashTable<ShdrFloatParamInternal> gShdrFloatParams;

  struct ShdrTexParamInternal : public RefCountBase {
    ShdrTexParamInternal(uint8_t slot,
                         const YRenderDevice::SamplerState& sampler_state)
      : RefCountBase(),
        mSamplerState(sampler_state),
        mSlot(slot) {
    }

    YRenderDevice::SamplerState mSamplerState;
    uint8_t mSlot;
  };
  YCommon::YContainers::TypedHashTable<ShdrTexParamInternal> gShdrTexParams;

  struct VertexShaderInternal : public RefCountBase {
    VertexShaderInternal(uint64_t shader_hash,
                         YRenderDevice::VertexShaderID shader_id)
      : RefCountBase(),
        mShaderHash(shader_hash),
        mVertexShaderID(shader_id) {}
    uint64_t mShaderHash;
    YRenderDevice::VertexShaderID mVertexShaderID;
  };
  YCommon::YContainers::TypedHashTable<VertexShaderInternal> gVertexShaders;

  struct PixelShaderInternal : public RefCountBase {
    PixelShaderInternal(uint64_t shader_hash,
                        YRenderDevice::PixelShaderID shader_id)
      : RefCountBase(),
        mShaderHash(shader_hash),
        mPixelShaderID(shader_id) {}
    uint64_t mShaderHash;
    YRenderDevice::PixelShaderID mPixelShaderID;
  };
  YCommon::YContainers::TypedHashTable<PixelShaderInternal> gPixelShaders;

  struct ShaderDataInternal : public RefCountBase {
    ShaderDataInternal(VertexShaderInternal* vertex_shader,
                       ShdrFloatParamInternal** vertex_shader_float_params,
                       uint8_t num_vertex_shdr_float_params,
                       ShdrTexParamInternal** vertex_shader_texture_params,
                       uint8_t num_vertex_shdr_texture_params,
                       PixelShaderInternal* pixel_shader,
                       ShdrFloatParamInternal** pixel_shader_float_params,
                       uint8_t num_pixel_shdr_float_params,
                       ShdrTexParamInternal** pixel_shader_texture_params,
                       uint8_t num_pixel_shdr_texture_params)
      : RefCountBase(),
        mNumVertexShdrFloatParams(num_vertex_shdr_float_params),
        mNumPixelShdrFloatParams(num_pixel_shdr_float_params),
        mNumVertexShdrTexParams(num_vertex_shdr_texture_params),
        mNumPixelShdrTexParams(num_pixel_shdr_texture_params),
        mVertexShader(vertex_shader),
        mPixelShader(pixel_shader) {
      memcpy(mVertexShdrFloatParams, vertex_shader_float_params,
             num_vertex_shdr_float_params * sizeof(mVertexShdrFloatParams[0]));
      memcpy(mPixelShdrFloatParams, pixel_shader_float_params,
             num_pixel_shdr_float_params * sizeof(mPixelShdrFloatParams[0]));
      memcpy(mVertexShdrTexParams, vertex_shader_texture_params,
             num_vertex_shdr_texture_params * sizeof(mVertexShdrTexParams[0]));
      memcpy(mPixelShdrTexParams, pixel_shader_texture_params,
             num_pixel_shdr_texture_params * sizeof(mPixelShdrTexParams[0]));
    }
    ShdrFloatParamInternal* mVertexShdrFloatParams[MAX_FLOAT_PARAMS_PER_SHADER];
    ShdrFloatParamInternal* mPixelShdrFloatParams[MAX_FLOAT_PARAMS_PER_SHADER];
    ShdrTexParamInternal* mVertexShdrTexParams[MAX_TEX_PARAMS_PER_SHADER];
    ShdrTexParamInternal* mPixelShdrTexParams[MAX_TEX_PARAMS_PER_SHADER];
    uint8_t mNumVertexShdrFloatParams, mNumPixelShdrFloatParams;
    uint8_t mNumVertexShdrTexParams, mNumPixelShdrTexParams;
    VertexShaderInternal* mVertexShader;
    PixelShaderInternal* mPixelShader;
  };
  YCommon::YContainers::TypedHashTable<ShaderDataInternal> gShaderDatas;

  struct ActivePassesInternal : public RefCountBase {
    ActivePassesInternal(RenderPassInternal** render_passes,
                         uint8_t num_render_passes)
      : RefCountBase(),
        mNumRenderPasses(num_render_passes) {
      memset(mRenderPasses, 0, sizeof(mRenderPasses));
      memcpy(mRenderPasses, render_passes,
             num_render_passes * sizeof(render_passes[0]));
    }
    uint8_t mNumRenderPasses;
    RenderPassInternal* mRenderPasses[MAX_ACTIVE_RENDERPASSES];
  };
  YCommon::YContainers::TypedHashTable<ActivePassesInternal> gActivePasses;

  struct RenderTypeInternal : public RefCountBase {
    RenderTypeInternal(const char* shader, size_t shader_size)
      : RefCountBase(),
        mShaderBaseSize(shader_size) {
      memset(mShaderBase, 0, sizeof(mShaderBase));
      YASSERT(shader_size < MAX_SHADER_BASE_NAME,
              "Maximum Shader Size %u exceeded: %u",
              static_cast<uint32_t>(MAX_SHADER_BASE_NAME),
              static_cast<uint32_t>(shader_size));
      memcpy(mShaderBase, shader, shader_size);
    }
    char mShaderBase[MAX_SHADER_BASE_NAME];
    size_t mShaderBaseSize;
  };
  YCommon::YContainers::TypedHashTable<RenderTypeInternal> gRenderTypes;

  ActivePassesInternal* gActiveRenderPasses = nullptr;
}

uint32_t GetDimensionValue(uint32_t frame_value,
                           Renderer::DimensionType type, float value) {
  return type == Renderer::kDimensionType_Percentage ?
         static_cast<uint32_t>(frame_value * value + 0.5f) :
         std::min(frame_value, static_cast<uint32_t>(value));
}

#define INITIALIZE_TABLE(HASHTABLE, TABLESIZE, NAME) \
  do { \
    const size_t table_buffer_size = \
        HASHTABLE.GetAllocationSize(TABLESIZE); \
    void* table_buffer = gMemBuffer.Allocate(table_buffer_size); \
    YASSERT(table_buffer, \
            "Not enough space to allocate " NAME " table."); \
    HASHTABLE.Init(table_buffer, table_buffer_size, TABLESIZE); \
  } while(0)

void Renderer::Initialize(void* buffer, size_t buffer_size) {
  gMemBuffer.Init(buffer, buffer_size);

  INITIALIZE_TABLE(gViewPorts, VIEWPORTS_SIZE, "ViewPorts");
  INITIALIZE_TABLE(gRenderTargets, RENDERTARGETS_SIZE, "Render Targets");
  INITIALIZE_TABLE(gBackBufferNames, BACKBUFFERNAMES_SIZE, "Back Buffer Names");
  INITIALIZE_TABLE(gBlendStates, BLENDSTATES_SIZE, "Blend States");
  INITIALIZE_TABLE(gRenderPasses, RENDERPASSES_SIZE, "Render Pass");
  INITIALIZE_TABLE(gVertexDecls, VERTEXDECLS_SIZE, "Vertex Declaration");
  INITIALIZE_TABLE(gShdrFloatParams, FLOATPARAMS_SIZE, "Shader Float Params");
  INITIALIZE_TABLE(gShdrTexParams, TEXTUREPARAMS_SIZE, "Shader Texture Params");
  INITIALIZE_TABLE(gVertexShaders, VERTEX_SHADERS_SIZE, "Vertex Shader");
  INITIALIZE_TABLE(gPixelShaders, PIXEL_SHADERS_SIZE, "Pixel Shader");
  INITIALIZE_TABLE(gShaderDatas, SHADER_COMBINATIONS_SIZE, "Shader Data");
  INITIALIZE_TABLE(gActivePasses, ACTIVE_PASSES_SIZE, "Active Render Passes");
  INITIALIZE_TABLE(gRenderTypes, RENDER_TYPES_SIZE, "Render Types");

  gActiveRenderPasses = nullptr;
}

void Renderer::Terminate() {
  gActiveRenderPasses = nullptr;

  gRenderTypes.Reset();
  gActivePasses.Reset();
  gShaderDatas.Reset();
  gPixelShaders.Reset();
  gVertexShaders.Reset();
  gShdrTexParams.Reset();
  gShdrFloatParams.Reset();
  gVertexDecls.Reset();
  gRenderPasses.Reset();
  gBlendStates.Reset();
  gBackBufferNames.Reset();
  gRenderTargets.Reset();
  gViewPorts.Reset();
}

void Renderer::RegisterViewPort(const char* name, size_t name_size,
                                DimensionType top_type, float top,
                                DimensionType left_type, float left,
                                DimensionType width_type, float width,
                                DimensionType height_type, float height,
                                float min_z, float max_z) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  ViewPortInternal* viewport = gViewPorts.GetValue(name_hash);
  if (viewport == nullptr) {
    ViewPortInternal new_viewport(top_type, top, left_type, left,
                                  width_type, width, height_type, height,
                                  min_z, max_z);
    viewport = gViewPorts.Insert(name_hash, new_viewport);
  }
  viewport->IncRef();
}

void Renderer::RegisterRenderTarget(const char* name, size_t name_size,
                                    YRenderDevice::PixelFormat format,
                                    DimensionType width_type, float width,
                                    DimensionType height_type, float height) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  RenderTargetInternal* render_target = gRenderTargets.GetValue(name_hash);
  if (render_target == nullptr) {
    YASSERT(gBackBufferNames.GetValue(name_hash) == nullptr,
            "Render target name cannot be the same as a back buffer.");
    RenderTargetInternal new_render_target(format,
                                           width_type, width,
                                           height_type, height);
    render_target = gRenderTargets.Insert(name_hash, new_render_target);
  }
  render_target->IncRef();
}

void Renderer::RegisterBackBufferName(const char* name, size_t name_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  BackBufferInternal* back_buffer = gBackBufferNames.GetValue(name_hash);
  if (back_buffer == nullptr) {
    YASSERT(gRenderTargets.GetValue(name_hash) == nullptr,
            "Back buffer name cannot be the same as a render target.");
    BackBufferInternal new_back_buffer;
    back_buffer = gBackBufferNames.Insert(name_hash, new_back_buffer);
  }
  back_buffer->IncRef();
}

void Renderer::RegisterRenderPass(
    const char* name, size_t name_size,
    const char* shader_variant, size_t variant_size,
    const YRenderDevice::RenderBlendState& blend_state,
    const char** render_targets, size_t* target_sizes,
    size_t num_targets) {
  const uint64_t blend_hash =
      YCommon::YUtils::Hash::Hash64(&blend_state, sizeof(blend_state));
  BlendStateInternal* blend_state_internal = gBlendStates.GetValue(blend_hash);
  if (nullptr == blend_state_internal) {
    BlendStateInternal new_blend_state(blend_state);
    blend_state_internal = gBlendStates.Insert(blend_hash, new_blend_state);
  }
  blend_state_internal->IncRef();

  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  RenderPassInternal* render_pass = gRenderPasses.GetValue(name_hash);
  if (nullptr == render_pass) {
    YASSERT(num_targets < MAX_RENDERTARGETS_PER_PASS,
            "Maximum render targets (%d) exceeded: %d",
            static_cast<int>(MAX_RENDERTARGETS_PER_PASS),
            static_cast<int>(num_targets));
    YASSERT(variant_size < MAX_SHADER_VARIANT_NAME,
            "Maximum shader variant name size (%d) exceeded: %d",
            static_cast<int>(MAX_SHADER_VARIANT_NAME),
            static_cast<int>(variant_size));

    RenderTargetInternal* targets[MAX_RENDERTARGETS_PER_PASS] = { nullptr };
    for (size_t i = 0; i < num_targets; ++i) {
      RenderTargetInternal* target =
      target = gRenderTargets.GetValue(render_targets[i], target_sizes[i]);
      YASSERT(target != nullptr,
              "Render Target (%s) could not be found for render pass (%s).",
              render_targets[i], name);
      target->IncRef();
      targets[i] = target;
    }
    RenderPassInternal new_render_pass(blend_state_internal, shader_variant,
                                       static_cast<uint8_t>(variant_size),
                                       targets,
                                       static_cast<uint8_t>(num_targets));
    render_pass = gRenderPasses.Insert(name_hash, new_render_pass);
  }
  render_pass->IncRef();
}

void Renderer::RegisterVertexDecl(
    const char* name, size_t name_size,
    const YRenderDevice::VertexDeclElement* elements,
    size_t num_elements) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  VertexDeclInternal* vertex_decl = gVertexDecls.GetValue(name_hash);
  if (nullptr == vertex_decl) {
    VertexDeclInternal new_vertex_decl(elements,
                                       static_cast<uint8_t>(num_elements));
    vertex_decl = gVertexDecls.Insert(name_hash, new_vertex_decl);
  }
  vertex_decl->IncRef();
}

void Renderer::RegisterShaderFloatParam(const char* name, size_t name_size,
                                        uint8_t num_floats,
                                        uint8_t reg, uint8_t reg_offset) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  ShdrFloatParamInternal* float_param = gShdrFloatParams.GetValue(name_hash);
  if (nullptr == float_param) {
    ShdrFloatParamInternal new_float_param(num_floats, reg, reg_offset);
    float_param = gShdrFloatParams.Insert(name_hash, new_float_param);
  }
  float_param->IncRef();
}

void Renderer::RegisterShaderTextureParam(
    const char* name, size_t name_size, uint8_t slot,
    const YRenderDevice::SamplerState& sampler) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  ShdrTexParamInternal* tex_param = gShdrTexParams.GetValue(name_hash);
  if (nullptr == tex_param) {
    ShdrTexParamInternal new_tex_param(slot, sampler);
    tex_param = gShdrTexParams.Insert(name_hash, new_tex_param);
  }
  tex_param->IncRef();
}

void Renderer::RegisterShaderData(
    const char* shader_name, size_t shader_name_size,
    const char* variant_name, size_t variant_name_size,
    size_t num_vertex_params,
    const char** vertex_params, size_t* vertex_param_sizes,
    const void* vertex_shader_data, size_t vertex_shader_size,
    size_t num_pixel_params,
    const char** pixel_params, size_t* pizel_param_sizes,
    const void* pixel_shader_data, size_t pixel_shader_size) {
  YASSERT(shader_name_size < MAX_SHADER_BASE_NAME,
          "Shader base name (%s) maximum length (%u) exceeded: %u",
          shader_name, static_cast<uint32_t>(MAX_SHADER_BASE_NAME),
          static_cast<uint32_t>(shader_name_size));
  YASSERT(variant_name_size < MAX_SHADER_VARIANT_NAME,
          "Shader variant name (%s) maximum length (%u) exceeded: %u",
          variant_name, static_cast<uint32_t>(MAX_SHADER_VARIANT_NAME),
          static_cast<uint32_t>(variant_name_size));
  char full_shader_name[MAX_SHADER_BASE_NAME + MAX_SHADER_VARIANT_NAME];
  memcpy(full_shader_name, shader_name, shader_name_size);
  full_shader_name[shader_name_size] = ':';
  memcpy(&full_shader_name[shader_name_size+1],
         variant_name, variant_name_size);

  const size_t full_shader_size = shader_name_size + variant_name_size;
  const uint64_t full_shader_hash =
      YCore::StringTable::AddString(full_shader_name, full_shader_size);

  ShaderDataInternal* shader_data = gShaderDatas.GetValue(full_shader_hash);
  if (nullptr == shader_data) {
    ShdrFloatParamInternal* vertex_float_params[MAX_FLOAT_PARAMS_PER_SHADER];
    ShdrTexParamInternal* vertex_tex_params[MAX_TEX_PARAMS_PER_SHADER];
    uint8_t num_vertex_float_params = 0;
    uint8_t num_vertex_tex_params = 0;
    for (size_t i = 0; i < num_vertex_params; ++i) {
      // Vertex Shader Parameters
      ShdrFloatParamInternal* float_param =
          gShdrFloatParams.GetValue(vertex_params[i], vertex_param_sizes[i]);
      if (float_param) {
        YASSERT(num_vertex_float_params + 1 < MAX_FLOAT_PARAMS_PER_SHADER,
                "Maximum number of float parameters (%u) exceeded: %s",
                static_cast<uint8_t>(MAX_FLOAT_PARAMS_PER_SHADER),
                full_shader_name);
        float_param->IncRef();
        vertex_float_params[num_vertex_float_params++] = float_param;
        continue;
      }

      ShdrTexParamInternal* tex_param =
          gShdrTexParams.GetValue(vertex_params[i], vertex_param_sizes[i]);
      if (tex_param) {
        YASSERT(num_vertex_tex_params + 1 < MAX_TEX_PARAMS_PER_SHADER,
                "Maximum number of texture parameters (%u) exceeded: %s",
                static_cast<uint8_t>(MAX_TEX_PARAMS_PER_SHADER),
                full_shader_name);
        tex_param->IncRef();
        vertex_tex_params[num_vertex_tex_params++] = tex_param;
        continue;
      }

      YASSERT(false, "Unknown Vertex Shader Parameter: %s", vertex_params[i]);
    }

    // Pixel Shader Parameters
    ShdrFloatParamInternal* pixel_float_params[MAX_FLOAT_PARAMS_PER_SHADER];
    ShdrTexParamInternal* pixel_tex_params[MAX_TEX_PARAMS_PER_SHADER];
    uint8_t num_pixel_float_params = 0;
    uint8_t num_pixel_tex_params = 0;
    for (size_t i = 0; i < num_pixel_params; ++i) {
      ShdrFloatParamInternal* float_param =
          gShdrFloatParams.GetValue(pixel_params[i], pizel_param_sizes[i]);
      if (float_param) {
        YASSERT(num_pixel_float_params + 1 < MAX_FLOAT_PARAMS_PER_SHADER,
                "Maximum number of float parameters (%u) exceeded: %s",
                static_cast<uint8_t>(MAX_FLOAT_PARAMS_PER_SHADER),
                full_shader_name);
        float_param->IncRef();
        pixel_float_params[num_pixel_float_params++] = float_param;
        continue;
      }

      ShdrTexParamInternal* tex_param =
          gShdrTexParams.GetValue(pixel_params[i], pizel_param_sizes[i]);
      if (tex_param) {
        YASSERT(num_pixel_tex_params + 1 < MAX_TEX_PARAMS_PER_SHADER,
                "Maximum number of texture parameters (%u) exceeded: %s",
                static_cast<uint8_t>(MAX_TEX_PARAMS_PER_SHADER),
                full_shader_name);
        tex_param->IncRef();
        pixel_tex_params[num_pixel_tex_params++] = tex_param;
        continue;
      }

      YASSERT(false, "Unknown Vertex Shader Parameter: %s", vertex_params[i]);
    }

    // Vertex Shader
    const uint64_t vertex_shader_hash =
        YCommon::YUtils::Hash::Hash64(vertex_shader_data, vertex_shader_size);
    VertexShaderInternal* vertex_shader =
        gVertexShaders.GetValue(vertex_shader_hash);
    if (nullptr == vertex_shader) {
      VertexShaderInternal new_vertex_shader(
          vertex_shader_hash,
          YRenderDevice::RenderDevice::CreateVertexShader(vertex_shader_data,
                                                          vertex_shader_size));
      vertex_shader = gVertexShaders.Insert(vertex_shader_hash,
                                            new_vertex_shader);
    }
    vertex_shader->IncRef();

    // Pixel Shader
    const uint64_t pixel_shader_hash =
        YCommon::YUtils::Hash::Hash64(pixel_shader_data, pixel_shader_size);
    PixelShaderInternal* pixel_shader =
        gPixelShaders.GetValue(pixel_shader_hash);
    if (nullptr == pixel_shader) {
      PixelShaderInternal new_pixel_shader(
          pixel_shader_hash,
          YRenderDevice::RenderDevice::CreatePixelShader(pixel_shader_data,
                                                         pixel_shader_size));
      pixel_shader = gPixelShaders.Insert(pixel_shader_hash, new_pixel_shader);
    }
    pixel_shader->IncRef();

    // Full Shader Data
    ShaderDataInternal new_shader_data(
        vertex_shader, vertex_float_params, num_vertex_float_params,
        vertex_tex_params, num_vertex_tex_params,
        pixel_shader, pixel_float_params, num_pixel_float_params,
        pixel_tex_params, num_pixel_tex_params);
    shader_data = gShaderDatas.Insert(full_shader_hash, new_shader_data);
  }
  shader_data->IncRef();
}

void Renderer::RegisterRenderPasses(const char* name, size_t name_size,
                                    const char** render_passes,
                                    const size_t* render_pass_sizes,
                                    size_t num_passes) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  ActivePassesInternal* active_passes = gActivePasses.GetValue(name_hash);
  if (nullptr == active_passes) {
    YASSERT(num_passes < MAX_ACTIVE_RENDERPASSES,
            "Maximum number of active render passes (%u) exceeded (%u): %s",
            static_cast<uint32_t>(MAX_ACTIVE_RENDERPASSES),
            static_cast<uint32_t>(num_passes), name);

    RenderPassInternal* internal_render_passes[MAX_ACTIVE_RENDERPASSES];
    for (size_t i = 0; i < num_passes; ++i) {
      internal_render_passes[i] = gRenderPasses.GetValue(render_passes[i],
                                                         render_pass_sizes[i]);
      YASSERT(internal_render_passes[i],
              "Render pass (%s) could not be found for \"%s\"",
              render_passes[i], name);
      internal_render_passes[i]->IncRef();
    }

    ActivePassesInternal new_active_pass(internal_render_passes,
                                         static_cast<uint8_t>(num_passes));
    active_passes = gActivePasses.Insert(name_hash, new_active_pass);
  }
  active_passes->IncRef();
}

void Renderer::RegisterRenderType(const char* name, size_t name_size,
                                  const char* shader, size_t shader_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  RenderTypeInternal* render_type = gRenderTypes.GetValue(name_hash);
  if (nullptr == render_type) {
    RenderTypeInternal new_render_type(shader, shader_size);
    render_type = gRenderTypes.Insert(name_hash, new_render_type);
  }
  render_type->IncRef();
}

bool Renderer::ReleaseViewPort(const char* name, size_t name_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  ViewPortInternal* viewport = gViewPorts.GetValue(name_hash);
  YASSERT(viewport, "Releasing an invalid viewport: %s", name);
  if (viewport->DecRef()) {
    if (viewport->mViewPortID != INVALID_VIEWPORT) {
      YRenderDevice::RenderDevice::ReleaseViewPort(viewport->mViewPortID);
    }
    return gViewPorts.Remove(name_hash);
  }
  return false;
}

bool Renderer::ReleaseRenderTarget(const char* name, size_t name_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  RenderTargetInternal* render_target = gRenderTargets.GetValue(name_hash);
  YASSERT(render_target, "Releasing an invalid render target: %s", name);
  if (render_target->DecRef()) {
    if (render_target->mRenderTargetID != INVALID_RENDER_TARGET) {
      YRenderDevice::RenderDevice::ReleaseRenderTarget(
          render_target->mRenderTargetID);
    }
    return gRenderTargets.Remove(name_hash);
  }
  return false;
}

bool Renderer::ReleaseBackBufferName(const char* name, size_t name_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  BackBufferInternal* back_buffer = gBackBufferNames.GetValue(name_hash);
  YASSERT(back_buffer, "Releasing an invalid back buffer name: %s", name);
  if (back_buffer->DecRef()) {
    return gBackBufferNames.Remove(name_hash);
  }
  return false;
}

bool Renderer::ReleaseRenderPass(const char* name, size_t name_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  RenderPassInternal* render_pass = gRenderPasses.GetValue(name_hash);
  YASSERT(render_pass, "Releasing an invalid render pass: %s", name);
  if (render_pass->DecRef()) {
    // Release Render Targets as well
    const uint8_t num_targets = render_pass->mNumRenderTargets;
    for (uint8_t i = 0; i < num_targets; ++i) {
      const bool released = render_pass->mRenderTargets[i]->DecRef();
      YASSERT(!released,
              "Render target released before referenced render pass (%s).",
              name);
    }

    // Decrement Blend State Reference.
    BlendStateInternal* blend_state = render_pass->mBlendState;
    if (blend_state->DecRef()) {
      if (blend_state->mBlendStateID != INVALID_BLEND_STATE) {
        YRenderDevice::RenderDevice::ReleaseRenderBlendState(
            blend_state->mBlendStateID);
      }
      const bool released = gBlendStates.Remove(blend_state->mBlendState);
      YDEBUG_CHECK(released, "Sanity blend state removal check failed.");
    }

    return gRenderPasses.Remove(name_hash);
  }
  return false;
}

bool Renderer::ReleaseVertexDecl(const char* name, size_t name_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  VertexDeclInternal* vertex_decl = gVertexDecls.GetValue(name_hash);
  YASSERT(vertex_decl, "Releasing an invalid Vertex Declaration: %s", name);
  if (vertex_decl->DecRef()) {
    if (vertex_decl->mVertexDeclID != INVALID_VERTEX_DECL) {
      YRenderDevice::RenderDevice::ReleaseVertexDeclaration(
          vertex_decl->mVertexDeclID);
    }
    return gVertexDecls.Remove(name_hash);
  }
  return false;
}

bool Renderer::ReleaseShaderFloatParam(const char* name, size_t name_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  ShdrFloatParamInternal* float_param = gShdrFloatParams.GetValue(name_hash);
  YASSERT(float_param, "Releasing an invalid Shader Float Param: %s", name);
  if (float_param->DecRef()) {
    return gShdrFloatParams.Remove(name_hash);
  }
  return false;
}

bool Renderer::ReleaseShaderTextureParam(const char* name, size_t name_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  ShdrTexParamInternal* tex_param = gShdrTexParams.GetValue(name_hash);
  YASSERT(tex_param, "Releasing an invalid Shader Texture Param: %s", name);
  if (tex_param->DecRef()) {
    return gShdrTexParams.Remove(name_hash);
  }
  return false;
}

bool Renderer::ReleaseShaderData(const char* shader_name,
                                 size_t shader_name_size,
                                 const char* variant_name,
                                 size_t variant_name_size) {
  char full_shader_name[MAX_SHADER_BASE_NAME + MAX_SHADER_VARIANT_NAME];
  memcpy(full_shader_name, shader_name, shader_name_size);
  full_shader_name[shader_name_size] = ':';
  memcpy(&full_shader_name[shader_name_size+1],
         variant_name, variant_name_size);

  const size_t full_shader_size = shader_name_size + variant_name_size;
  const uint64_t full_shader_hash =
      YCore::StringTable::AddString(full_shader_name, full_shader_size);

  ShaderDataInternal* shader_data = gShaderDatas.GetValue(full_shader_hash);
  YASSERT(shader_data, "Releasing an invalid Shader: %s", full_shader_name);
  if (shader_data->DecRef()) {
    PixelShaderInternal* pixel_shader = shader_data->mPixelShader;
    if (pixel_shader->DecRef()) {
      YRenderDevice::RenderDevice::ReleasePixelShader(
          pixel_shader->mPixelShaderID);
      const bool removed = gPixelShaders.Remove(pixel_shader->mShaderHash);
      YDEBUG_CHECK(removed, "Pixel shader removal sanity checked failed.");
    }

    VertexShaderInternal* vertex_shader = shader_data->mVertexShader;
    if (vertex_shader->DecRef()) {
      YRenderDevice::RenderDevice::ReleaseVertexShader(
          vertex_shader->mVertexShaderID);
      const bool removed = gVertexShaders.Remove(vertex_shader->mShaderHash);
      YASSERT(removed, "Vertex shader removal sanity checked failed.");
    }

    const uint8_t pixel_texture_params = shader_data->mNumPixelShdrTexParams;
    for (uint8_t i = 0; i < pixel_texture_params; ++i) {
      bool empty = shader_data->mPixelShdrTexParams[i]->DecRef();
      YASSERT(!empty, "Shader parameter released before shader data: %s",
                   full_shader_name);
    }
    const uint8_t pixel_float_params = shader_data->mNumPixelShdrFloatParams;
    for (uint8_t i = 0; i < pixel_float_params; ++i) {
      bool empty = shader_data->mPixelShdrFloatParams[i]->DecRef();
      YASSERT(!empty, "Shader parameter released before shader data: %s",
              full_shader_name);
    }

    const uint8_t vertex_tex_params = shader_data->mNumVertexShdrFloatParams;
    for (uint8_t i = 0; i < vertex_tex_params; ++i) {
      bool empty = shader_data->mVertexShdrTexParams[i]->DecRef();
      YASSERT(!empty, "Shader parameter released before shader data: %s",
              full_shader_name);
    }
    const uint8_t vertex_float_params = shader_data->mNumVertexShdrTexParams;
    for (uint8_t i = 0; i < vertex_float_params; ++i) {
      bool empty = shader_data->mVertexShdrFloatParams[i]->DecRef();
      YASSERT(!empty, "Shader parameter released before shader data: %s",
              full_shader_name);
    }
    return gShaderDatas.Remove(full_shader_hash);
  }
  return false;
}

bool Renderer::ReleaseRenderPasses(const char* name, size_t name_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  ActivePassesInternal* active_passes = gActivePasses.GetValue(name_hash);
  YASSERT(active_passes, "Releasing an invalid Render Passes Name: %s", name);
  if (active_passes->DecRef()) {
    const uint8_t num_passes = active_passes->mNumRenderPasses;
    for (uint8_t i = 0; i < num_passes; ++i) {
      const bool empty = active_passes->mRenderPasses[i]->DecRef();
      YASSERT(!empty, "Render pass %u released before render passes name: %s",
              static_cast<uint32_t>(i), name);
    }
    return gActivePasses.Remove(name_hash);
  }
  return false;
}

bool Renderer::ReleaseRenderType(const char* name, size_t name_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  RenderTypeInternal* render_type = gRenderTypes.GetValue(name_hash);
  YASSERT(render_type, "Releasing an invalid Render Type Name: %s", name);
  if (render_type->DecRef()) {
    return gRenderTypes.Remove(name_hash);
  }
  return false;
}

void Renderer::ActivateRenderPasses(const char* name, size_t name_size) {
  DeactivateRenderPasses();
  uint32_t frame_width, frame_height;
  YRenderDevice::RenderDevice::GetFrameBufferDimensions(frame_width,
                                                        frame_height);

  ActivePassesInternal* active_passes = gActivePasses.GetValue(name, name_size);
  YASSERT(active_passes, "Unknown Render Passes Name: %s", name);
  const uint8_t num_passes = active_passes->mNumRenderPasses;
  for (uint8_t i = 0; i < num_passes; ++i) {
    RenderPassInternal* render_pass = active_passes->mRenderPasses[i];

    BlendStateInternal* blend_state = render_pass->mBlendState;
    if (blend_state->mBlendStateID == INVALID_BLEND_STATE) {
      YDEBUG_CHECK(blend_state = gBlendStates.GetValue(blend_state->mBlendState),
                   "Sanity blend state existance check failed.");
      blend_state->mBlendStateID =
          YRenderDevice::RenderDevice::CreateRenderBlendState(
              blend_state->mBlendState);
    }

    const uint8_t num_render_targets = render_pass->mNumRenderTargets;
    for (uint8_t n = 0; n < num_render_targets; ++n) {
      RenderTargetInternal* render_target = render_pass->mRenderTargets[n];
      if (render_target->mRenderTargetID == INVALID_RENDER_TARGET) {
        render_target->mRenderTargetID =
            YRenderDevice::RenderDevice::CreateRenderTarget(
                GetDimensionValue(frame_width,
                                  render_target->mWidthType,
                                  render_target->mWidth),
                GetDimensionValue(frame_width,
                                  render_target->mHeightType,
                                  render_target->mHeight),
                render_target->mFormat);
      }
    }
  }
  active_passes->IncRef();
  gActiveRenderPasses = active_passes;

  // Setup Render Objects
  ViewPortInternal* viewport = gViewPorts.GetValue(name, name_size);
  if (viewport->mViewPortID == INVALID_VIEWPORT) {
    viewport->mViewPortID =
        YRenderDevice::RenderDevice::CreateViewPort(
            GetDimensionValue(frame_height,
                              viewport->mTopType,
                              viewport->mTop),
            GetDimensionValue(frame_width,
                              viewport->mLeftType,
                              viewport->mLeft),
            GetDimensionValue(frame_width,
                              viewport->mWidthType,
                              viewport->mWidth),
            GetDimensionValue(frame_height,
                              viewport->mHeightType,
                              viewport->mHeight),
            viewport->mMinZ, viewport->mMaxZ);
  }
}

void Renderer::DeactivateRenderPasses() {
  if (gActiveRenderPasses) {
    const bool empty = gActiveRenderPasses->DecRef();
    YASSERT(!empty, "Active render passes was released before deactivated.");
    gActiveRenderPasses = nullptr;
  }
}

}} // namespace YEngine { namespace YRenderer {
