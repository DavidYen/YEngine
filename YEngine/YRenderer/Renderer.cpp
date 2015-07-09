#include <YCommon/Headers/stdincludes.h>
#include "Renderer.h"

#include <YCommon/YContainers/HashTable.h>
#include <YCommon/YContainers/MemBuffer.h>
#include <YEngine/YCore/StringTable.h>

#define VIEWPORTS_SIZE 16
#define RENDERTARGETS_SIZE 16
#define RENDERPASSES_SIZE 32
#define RENDERTYPES_SIZE 32

namespace YEngine { namespace YRenderer {

namespace {
  YCommon::YContainers::MemBuffer gMemBuffer;

  struct ViewPortInternal {
    ViewPortInternal(YRenderDevice::ViewPortID viewport_id)
      : mRefCount(0),
        mViewportID(viewport_id) {
    }

    uint32_t mRefCount;
    YRenderDevice::ViewPortID mViewportID;
  };
  YCommon::YContainers::TypedHashTable<ViewPortInternal> gViewPorts;

  struct RenderTargetInternal {
    RenderTargetInternal(YRenderDevice::RenderTargetID render_target_id)
      : mRefCount(0),
        mRenderTargetID(render_target_id) {
    }

    uint32_t mRefCount;
    YRenderDevice::RenderTargetID mRenderTargetID;
  };
  YCommon::YContainers::TypedHashTable<RenderTargetInternal> gRenderTargets;
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
}

void Renderer::Terminate() {
  gRenderTargets.Reset();
  gViewPorts.Reset();
}

void Renderer::RegisterViewPort(const char* name, size_t name_size,
                                uint32_t top, uint32_t left,
                                uint32_t width, uint32_t height,
                                float min_z, float max_z) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  ViewPortInternal* viewport = gViewPorts.GetValue(name_hash);
  if (viewport == nullptr) {
    YRenderDevice::ViewPortID viewport_id =
        YRenderDevice::RenderDevice::CreateViewPort(top, left,
                                                    width, height,
                                                    min_z, max_z);

    ViewPortInternal new_viewport(viewport_id);
    viewport = gViewPorts.Insert(name_hash, new_viewport);
  }
  viewport->mRefCount++;
}

void Renderer::RegisterRenderTarget(const char* name, size_t name_size,
                                    YRenderDevice::PixelFormat format,
                                    float width_percentage,
                                    float height_percentage) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  RenderTargetInternal* render_target = gRenderTargets.GetValue(name_hash);
  if (render_target == nullptr) {
    uint32_t frame_width, frame_height;
    YRenderDevice::RenderDevice::GetFrameBufferDimensions(frame_width,
                                                          frame_height);
    const uint32_t width =
        static_cast<uint32_t>(frame_width * width_percentage + 0.5f);
    const uint32_t height =
        static_cast<uint32_t>(frame_height * height_percentage + 0.5f);
        YRenderDevice::RenderTargetID target_id =
            YRenderDevice::RenderDevice::CreateRenderTarget(width, height,
                                                            format);

    RenderTargetInternal new_render_target(target_id);
    render_target = gRenderTargets.Insert(name_hash, new_render_target);
  }
  render_target->mRefCount++;
}

bool Renderer::ReleaseViewPort(const char* name, size_t name_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  ViewPortInternal* viewport = gViewPorts.GetValue(name_hash);
  YDEBUG_CHECK(viewport, "Releasing an invalid viewport: %s", name);

  if (0 == --viewport->mRefCount) {
    YRenderDevice::RenderDevice::ReleaseViewPort(viewport->mViewportID);
    return gViewPorts.Remove(name_hash);
  }
  return false;
}

bool Renderer::ReleaseRenderTarget(const char* name, size_t name_size) {
  const uint64_t name_hash = YCore::StringTable::AddString(name, name_size);
  RenderTargetInternal* render_target = gRenderTargets.GetValue(name_hash);
  YDEBUG_CHECK(render_target, "Releasing an invalid render target: %s", name);

  if (0 == --render_target->mRefCount) {
    YRenderDevice::RenderDevice::ReleaseRenderTarget(
        render_target->mRenderTargetID);
    return gRenderTargets.Remove(name_hash);
  }
  return false;
}

}} // namespace YEngine { namespace YRenderer {
