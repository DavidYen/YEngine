#include <YCommon/Headers/stdincludes.h>
#include "Renderer.h"

#include <algorithm>

#include <YCommon/YContainers/HashTable.h>
#include <YCommon/YContainers/MemBuffer.h>
#include <YCommon/YUtils/Hash.h>
#include <YEngine/YCore/StringTable.h>
#include <YEngine/YRenderDevice/RenderBlendState.h>

#define VIEWPORTS_SIZE 16
#define RENDERTARGETS_SIZE 16
#define BACKBUFFERNAMES_SIZE 12
#define BLENDSTATES_SIZE 32
#define RENDERPASSES_SIZE 32
#define RENDERTYPES_SIZE 32

#define MAX_RENDERTARGETS_PER_PASS 4
#define MAX_SHADER_VARIANT_NAME 16

#define INVALID_VIEWPORT static_cast<YRenderDevice::ViewPortID>(-1)
#define INVALID_RENDER_TARGET static_cast<YRenderDevice::RenderTargetID>(-1)
#define INVALID_BLEND_STATE static_cast<YRenderDevice::RenderBlendStateID>(-1)

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
}

uint32_t GetDimensionValue(uint32_t frame_value,
                           Renderer::DimensionType type, float value) {
  return type == Renderer::kDimensionType_Percentage ?
         static_cast<uint32_t>(frame_value * value + 0.5f) :
         std::min(frame_value, static_cast<uint32_t>(value));
}

void Renderer::Initialize(void* buffer, size_t buffer_size) {
  gMemBuffer.Init(buffer, buffer_size);

  // View Ports
  const size_t viewports_size =
      gViewPorts.GetAllocationSize(VIEWPORTS_SIZE);
  void* viewports_buffer = gMemBuffer.Allocate(viewports_size);
  YASSERT(viewports_buffer,
          "Not enough space to allocate viewports table.");
  gViewPorts.Init(viewports_buffer, viewports_size, VIEWPORTS_SIZE);

  // Render Targets
  const size_t targets_size =
      gRenderTargets.GetAllocationSize(RENDERTARGETS_SIZE);
  void* targets_buffer = gMemBuffer.Allocate(targets_size);
  YASSERT(targets_buffer,
          "Not enough space to allocate render targets table.");
  gRenderTargets.Init(targets_buffer, targets_size, RENDERTARGETS_SIZE);

  // Back Buffer Names
  const size_t backbuffers_size =
      gBackBufferNames.GetAllocationSize(BACKBUFFERNAMES_SIZE);
  void* backbuffers_buffer = gMemBuffer.Allocate(backbuffers_size);
  YASSERT(backbuffers_buffer,
          "Not enough space to allocate back buffer name table.");
  gBackBufferNames.Init(backbuffers_buffer, backbuffers_size,
                        BACKBUFFERNAMES_SIZE);

  const size_t blendstates_size =
      gBlendStates.GetAllocationSize(BLENDSTATES_SIZE);
  void* blendstates_buffer = gMemBuffer.Allocate(blendstates_size);
  YASSERT(blendstates_buffer,
          "Not enough space to allocate blend states table.");
  gBlendStates.Init(blendstates_buffer, blendstates_size,
                    BLENDSTATES_SIZE);

  // Render Passes
  const size_t renderpasses_size =
      gRenderPasses.GetAllocationSize(RENDERPASSES_SIZE);
  void* renderpasses_buffer = gMemBuffer.Allocate(renderpasses_size);
  YASSERT(renderpasses_buffer,
          "Not enough space to allocate back buffer name table.");
  gRenderPasses.Init(renderpasses_buffer, renderpasses_size,
                     RENDERPASSES_SIZE);
}

void Renderer::Terminate() {
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

void Renderer::ActivateRenderPasses(const char* name, size_t name_size) {
  uint32_t frame_width, frame_height;
  YRenderDevice::RenderDevice::GetFrameBufferDimensions(frame_width,
                                                        frame_height);

  // Setup Render Passes
  RenderPassInternal* render_pass = gRenderPasses.GetValue(name, name_size);

  BlendStateInternal* blend_state = render_pass->mBlendState;
  if (blend_state->mBlendStateID == INVALID_BLEND_STATE) {
    YDEBUG_CHECK(blend_state = gBlendStates.GetValue(blend_state->mBlendState),
                 "Sanity blend state existance check failed.");
    blend_state->mBlendStateID =
        YRenderDevice::RenderDevice::CreateRenderBlendState(
            blend_state->mBlendState);
  }

  const uint8_t num_render_targets = render_pass->mNumRenderTargets;
  for (uint8_t i = 0; i < num_render_targets; ++i) {
    RenderTargetInternal* render_target = render_pass->mRenderTargets[i];
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

}} // namespace YEngine { namespace YRenderer {
