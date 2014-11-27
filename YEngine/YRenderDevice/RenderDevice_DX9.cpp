#include <YCommon/Headers/stdincludes.h>
#include "RenderDevice.h"

#include <algorithm>
#include <d3dx9.h>
#include <string>

#include <YCommon/YContainers/AtomicArray.h>
#include <YCommon/YPlatform/PlatformHandle_Win.h>
#include <YCommon/Headers/Atomics.h>
#include <YCommon/Headers/Macros.h>

#include "VertexDeclElement.h"

#define NULL_RENDER_TARGET static_cast<RenderTargetID>(-1)
#define MAX_ELEMENTS_PER_VERTEX 24
#define MAX_STREAM_SOURCES 2
#define MAX_NUM_SURFACES 12
#define MAX_NUM_VERT_DECLS 32
#define MAX_NUM_TEXTURES 512
#define MAX_NUM_VERT_BUFFS 512
#define MAX_NUM_INDEX_BUFFS 512
#define MAX_NUM_CONST_BUFFS 512

namespace YEngine { namespace YRenderDevice {

/*************
* Helper Classes
**************/
template <typename T, typename T_ID, size_t size>
struct DataArray {
  T data[size];
  bool used[size];
  YCommon::YContainers::TypedAtomicArray<T> atomic_array;

  void Init() {
    memset(used, 0, sizeof(used));
    atomic_array.Init(data, sizeof(data), size);
  }

  void Release() {
    const uint32_t max_index = atomic_array.GetMaxUsedIndex();
    for (uint32_t i = 0; i < max_index; ++i) {
      if (used[i]) {
        data[i].Release();
      }
    }
  }

  T_ID AllocateID() {
    uint32_t index = atomic_array.Allocate();
    if (index != static_cast<uint32_t>(-1)) {
      used[index] = true;
      data[index].Init();
    }
    return static_cast<T_ID>(index);
  }

  bool ReleaseID(T_ID id) {
    if (id >= size || !used[id])
      return false;

    used[id] = false;
    data[id].Release();
    atomic_array.Remove(static_cast<uint32_t>(id));
    return true;
  }
};

/*************
* Global Variables
**************/
namespace {
  LPDIRECT3D9 gD3D = NULL;
  LPDIRECT3DDEVICE9 gD3DDevice = NULL;
  void* gMemory = NULL;
  uint64_t gMemorySize = 0;
  volatile uint64_t gUsedMemory = 0;

  const D3DFORMAT kTextureFormats[] = {
    D3DFMT_A8R8G8B8, // kPixelFormat_A8R8G8B8,
    D3DFMT_R32F,     // kPixelFormat_F32,
  };
  static_assert(ARRAY_SIZE(kTextureFormats) == NUM_PIXEL_FORMATS,
                "Texture Format translation must match.");

  const D3DPRIMITIVETYPE kPrimitiveDrawTypes[] = {
    D3DPT_POINTLIST,     // kDrawPrimitive_PointList,
    D3DPT_LINELIST,      // kDrawPrimitive_LineList,
    D3DPT_LINESTRIP,     // kDrawPrimitive_LineStrip,
    D3DPT_TRIANGLELIST,  // kDrawPrimitive_TriangleList,
    D3DPT_TRIANGLESTRIP, // kDrawPrimitive_TriangleStrip,
    D3DPT_TRIANGLEFAN,   // kDrawPrimitive_TriangleFan,
  };
  static_assert(ARRAY_SIZE(kPrimitiveDrawTypes) == NUM_DRAW_PRIMITIVES,
                "Primitive Draw Type translations must match.");

  // Negative numbers mean subtract from list of vertices.
  // Positive numbers mean divide from list of vertices.
  const int kPrimitiveDrawPoints[] = {
    1,  // kDrawPrimitive_PointList,
    2,  // kDrawPrimitive_LineList,
    -1, // kDrawPrimitive_LineStrip,
    3,  // kDrawPrimitive_TriangleList,
    -2, // kDrawPrimitive_TriangleStrip,
    -2, // kDrawPrimitive_TriangleFan,
  };
  static_assert(ARRAY_SIZE(kPrimitiveDrawTypes) == NUM_DRAW_PRIMITIVES,
                "Primitive Draw Type translations must match.");

  const BYTE kVertexElementDeclTypes[] = {
    D3DDECLTYPE_FLOAT1, // kVertexElementType_Float,
    D3DDECLTYPE_FLOAT2, // kVertexElementType_Float2,
    D3DDECLTYPE_FLOAT3, // kVertexElementType_Float3,
    D3DDECLTYPE_FLOAT4, // kVertexElementType_Float4,
  };
  static_assert(ARRAY_SIZE(kVertexElementDeclTypes) == NUM_VERTEX_ELEMENT_TYPES,
                "Number of decl types must match vertex element types.");

  struct ElementUsage {
    D3DDECLUSAGE usage;
    BYTE usage_index;
  };
  const ElementUsage kVertexElementUsages[] = {
    { D3DDECLUSAGE_POSITION, 0 },  // kVertexElementUsage_Position,
    { D3DDECLUSAGE_POSITION, 1 },  // kVertexElementUsage_Position1,
    { D3DDECLUSAGE_NORMAL, 0 },    // kVertexElementUsage_Normal,
    { D3DDECLUSAGE_NORMAL, 1 },    // kVertexElementUsage_Normal1,
    { D3DDECLUSAGE_BINORMAL, 0 },  // kVertexElementUsage_Binormal,
    { D3DDECLUSAGE_BINORMAL, 1 },  // kVertexElementUsage_BiNormal1,
    { D3DDECLUSAGE_TANGENT, 0 },   // kVertexElementUsage_Tanget,
    { D3DDECLUSAGE_TANGENT, 1 },   // kVertexElementUsage_Tanget1,
    { D3DDECLUSAGE_TEXCOORD, 0 },  // kVertexElementUsage_TexCoord,
    { D3DDECLUSAGE_TEXCOORD, 1 },  // kVertexElementUsage_TexCoord1,
  };
  static_assert(ARRAY_SIZE(kVertexElementUsages) == NUM_VERTEX_ELEMENT_USAGES,
                "Number of vertex element usages must match.");

  // Texture Data
  struct TextureData {
    IDirect3DTexture9* texture;
    UsageType usage;
    D3DFORMAT format;
    uint32_t width;
    uint32_t height;
    uint32_t mips;

    void Init() {
      texture = NULL;
      usage = kUsageType_Invalid;
      format = D3DFMT_UNKNOWN;
      width = 0;
      height = 0;
      mips = 0;
    }
    void Release() {
      if (texture) {
        texture->Release();
        Init();
      }
    }
  };
  DataArray<TextureData, TextureID, MAX_NUM_TEXTURES> gTextures;

  // Surface Data
  struct SurfaceData {
    IDirect3DSurface9* surface;
    TextureID texture_id;
    void Init() {
      surface = NULL;
      texture_id = static_cast<TextureID>(-1);
    }
    void Release() {
      if (surface) {
        if (texture_id != static_cast<TextureID>(-1)) {
          gTextures.ReleaseID(texture_id);
        }
        surface->Release();
        Init();
      }
    }
  };
  DataArray<SurfaceData, RenderTargetID, MAX_NUM_SURFACES> gSurfaces;

  // Vertex Declaration Data
  struct VertexDeclData {
    IDirect3DVertexDeclaration9* decl;
    uint8_t num_streams;
    uint8_t stream_divisors[MAX_STREAM_SOURCES];
    void Init() {
      decl = NULL;
      num_streams = 0;
      memset(stream_divisors, 0, sizeof(stream_divisors));
    }
    void Release() {
      if (decl) {
        decl->Release();
        Init();
      }
    }
  };
  DataArray<VertexDeclData, VertexDeclID, MAX_NUM_VERT_DECLS> gVertDecls;

  // Vertex Buffer Data
  struct VertBuffData {
    IDirect3DVertexBuffer9* vertex_buffer;
    UsageType usage;
    uint32_t stride;
    uint32_t total_count;
    volatile uint32_t count;

    void Init() {
      vertex_buffer = NULL;
      usage = kUsageType_Invalid;
      stride = 0;
      total_count = 0;
      count = 0;
    }
    void Release() {
      if (vertex_buffer) {
        vertex_buffer->Release();
        Init();
      }
    }
  };
  DataArray<VertBuffData, VertexBufferID, MAX_NUM_VERT_BUFFS> gVertexBuffers;

  // Index Buffer Data
  struct IndexBuffData {
    IDirect3DIndexBuffer9* index_buffer;
    UsageType usage;
    uint32_t total_count;
    volatile uint32_t count;

    void Init() {
      index_buffer = NULL;
      usage = kUsageType_Invalid;
      total_count = 0;
      count = 0;
    }
    void Release() {
      if (index_buffer) {
        index_buffer->Release();
        Init();
      }
    }
  };
  DataArray<IndexBuffData, IndexBufferID, MAX_NUM_INDEX_BUFFS> gIndexBuffers;

  // Constant Buffer Data
  struct ConstBuffData {
    void* buffer_data;
    UsageType usage;
    size_t size;

    void Init() {
      buffer_data = NULL;
      usage = kUsageType_Invalid;
      size = 0;
    }
    void Release() {
      Init();
    }
  };
  DataArray<ConstBuffData, ConstantBufferID, MAX_NUM_CONST_BUFFS> gConstBuffers;

  RenderTargetID gBackBufferRenderTarget = 0;
  D3DPRIMITIVETYPE gActivatedDrawPrimitiveType = D3DPT_FORCE_DWORD;
  int gDrawPoints = 0;
  VertexBufferID gActivatedStreams[MAX_STREAM_SOURCES];
}

void* AllocateMemory(size_t size) {
  const uint64_t memory_location = YCommon::AtomicAdd64(
      &gUsedMemory, static_cast<uint64_t>(size));
  YASSERT(memory_location + size < gMemorySize,
          "Render Device has run out of memory! Maximum size: %u",
          static_cast<uint32_t>(gMemorySize));

  return static_cast<uint8_t*>(gMemory) + memory_location;
}

void RenderDevice::Initialize(const YCommon::YPlatform::PlatformHandle& handle,
                              uint32_t width, uint32_t height,
                              void* buffer, size_t buffer_size) {
  YDEBUG_CHECK(gD3D == NULL, "DirectX9 Object already initialized!");
  YDEBUG_CHECK(gD3DDevice == NULL, "DirectX9 Device already initialized!");

  gMemory = buffer;
  gMemorySize = static_cast<uint64_t>(buffer_size);
  gUsedMemory = 0;

  const YCommon::YPlatform::WinPlatformHandle& win_handle =
      reinterpret_cast<const YCommon::YPlatform::WinPlatformHandle&>(handle);

  gD3D = Direct3DCreate9(D3D_SDK_VERSION);
  YASSERT(gD3D, "Direct3DCreate9() Failed.");

  // Fill Out Parameters Structure
  D3DPRESENT_PARAMETERS d3dpp;
  memset(&d3dpp, 0, sizeof(d3dpp));
  d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
  d3dpp.BackBufferCount = 1;
  d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp.hDeviceWindow = win_handle.mHwnd;
  d3dpp.EnableAutoDepthStencil = FALSE;
  d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
  d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

  // Windowed Mode?
  d3dpp.Windowed = win_handle.mWindowedMode ? TRUE : FALSE;

  // Back Buffer Size
  d3dpp.BackBufferWidth = width;
  d3dpp.BackBufferHeight = height;

  // Create D3D Device Interface
  HRESULT hr = gD3D->CreateDevice(
    D3DADAPTER_DEFAULT,   // Primary Adapter
    D3DDEVTYPE_HAL,       // Device Type
    d3dpp.hDeviceWindow,  // hFocusWindow
    D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING, // BehaviorFlags
    &d3dpp,               // Present Parameters
    &gD3DDevice);
  YASSERT(hr == D3D_OK, "DirectX9 CreateDevice() failed.");

  // Initialize Various Data Handlers
  gTextures.Init();
  gSurfaces.Init();
  gVertexBuffers.Init();
  gIndexBuffers.Init();

  // Store back buffer surface
  const RenderTargetID back_buffer_id = gSurfaces.AllocateID();
  hr = gD3DDevice->GetBackBuffer(0, 0,  D3DBACKBUFFER_TYPE_MONO,
                                 &gSurfaces.data[back_buffer_id].surface);
  YASSERT(hr == D3D_OK, "Could not get default back buffer.");
  gBackBufferRenderTarget = back_buffer_id;
  gActivatedDrawPrimitiveType = D3DPT_FORCE_DWORD;
  gDrawPoints = 0;
  memset(gActivatedStreams, -1, sizeof(gActivatedStreams));
}

void RenderDevice::Terminate() {
  gIndexBuffers.Release();
  gVertexBuffers.Release();
  gSurfaces.Release();
  gTextures.Release();

  gD3DDevice->Release();
  gD3D->Release();
}

// System Values
RenderTargetID RenderDevice::GetNullRenderTarget() {
  return NULL_RENDER_TARGET;
}

RenderTargetID RenderDevice::GetBackBufferRenderTarget() {
  return gBackBufferRenderTarget;
}

TextureID RenderDevice::GetRenderTargetTexture(RenderTargetID render_target) {
  YASSERT(render_target < ARRAY_SIZE(gSurfaces.data) &&
          gSurfaces.used[render_target],
          "Getting Texture for Invalid Render Target: %d",
          static_cast<int>(render_target));
  return gSurfaces.data[render_target].texture_id;
}

// Creates
RenderTargetID RenderDevice::CreateRenderTarget(uint32_t width, uint32_t height,
                                                PixelFormat format) {
  YASSERT(format > 0 && format < ARRAY_SIZE(kTextureFormats),
          "Invalid Pixel Format: %d", static_cast<int>(format));
  const D3DFORMAT d3d_format = kTextureFormats[format];

  const TextureID texture_id = gTextures.AllocateID();
  YASSERT(texture_id != static_cast<TextureID>(-1),
          "Maximum number of textures reached.");

  const RenderTargetID surface_id = gSurfaces.AllocateID();
  YASSERT(surface_id != static_cast<RenderTargetID>(-1),
          "Maximum number of surfaces reached.");

  TextureData& texture_data = gTextures.data[texture_id];
  texture_data.usage = kUsageType_System;
  texture_data.format = d3d_format;
  texture_data.width = width;
  texture_data.height = height;
  texture_data.mips = 1;
  HRESULT hr = gD3DDevice->CreateTexture(width, height, 1,
                                         D3DUSAGE_RENDERTARGET,
                                         d3d_format,
                                         D3DPOOL_DEFAULT,
                                         &texture_data.texture,
                                         NULL);
  YASSERT(hr == D3D_OK, "Could not create render target texture.");

  SurfaceData& surface_data = gSurfaces.data[texture_id];
  surface_data.texture_id = texture_id;
  hr = texture_data.texture->GetSurfaceLevel(0, &surface_data.surface);
  YASSERT(hr == D3D_OK, "Surface Target could not be extracted from texture.");

  (void) hr;
  return surface_id;
}

VertexDeclID RenderDevice::CreateVertexDeclaration(VertexDeclElement* elements,
                                                   size_t num_elements) {
  YASSERT(num_elements < MAX_ELEMENTS_PER_VERTEX,
          "Maximum number of elements per vertex declaration is %u, %u given.",
          static_cast<uint32_t>(MAX_ELEMENTS_PER_VERTEX),
          static_cast<uint32_t>(num_elements));

  const VertexDeclID vert_decl_id = gVertDecls.AllocateID();
  YASSERT(vert_decl_id != static_cast<VertexDeclID>(-1),
          "Maximum number of vertex declarations reached.");

  D3DVERTEXELEMENT9 vert_elements[MAX_ELEMENTS_PER_VERTEX+1];
  VertexDeclData& vertex_decl_data = gVertDecls.data[vert_decl_id];
  uint8_t max_streams = 0;
  bool stream_divisor_set[MAX_STREAM_SOURCES] = { false };

  size_t element_iter = 0;
  while (element_iter < num_elements) {
    const VertexDeclElement& decl_element = elements[element_iter];
    const ElementUsage& element_usage =
        kVertexElementUsages[decl_element.mElementUsage];

    const D3DVERTEXELEMENT9 vert_element = {
        static_cast<WORD>(decl_element.mStreamNum),         // WORD Stream;
        static_cast<WORD>(decl_element.mElementOffset),     // WORD Offset;
        kVertexElementDeclTypes[decl_element.mElementType], // BYTE Type;
        D3DDECLMETHOD_DEFAULT,                              // BYTE Method;
        static_cast<BYTE>(element_usage.usage),             // BYTE Usage;
        element_usage.usage_index                           // BYTE UsageIndex;
    };

    vert_elements[element_iter++] = vert_element;

    YASSERT(decl_element.mStreamNum < MAX_STREAM_SOURCES,
            "Maximum stream source is %u, %u supplied as vertex element.",
            static_cast<uint32_t>(MAX_STREAM_SOURCES),
            decl_element.mStreamNum);

    max_streams = max(decl_element.mStreamNum, max_streams);
    if (stream_divisor_set[decl_element.mStreamNum]) {
      YASSERT(decl_element.mInstanceDivisor ==
              vertex_decl_data.stream_divisors[decl_element.mStreamNum],
              "Instance divisor must all match.");
    } else {
      stream_divisor_set[decl_element.mStreamNum] = true;
      vertex_decl_data.stream_divisors[decl_element.mStreamNum] =
          decl_element.mInstanceDivisor;
    }
  }

  const D3DVERTEXELEMENT9 end_element = D3DDECL_END();
  vert_elements[element_iter++] = end_element;

  vertex_decl_data.num_streams = max_streams + 1;
  HRESULT hr = gD3DDevice->CreateVertexDeclaration(
      vert_elements,          // const D3DVERTEXELEMENT9 *pVertexElements,
      &vertex_decl_data.decl  // IDirect3DVertexDeclaration9 **ppDecl
  );

  YASSERT(hr == D3D_OK, "Could not create vertex declaration.");
  (void) hr;

  return vert_decl_id;
}

TextureID RenderDevice::CreateTexture(UsageType type, uint32_t width,
                                      uint32_t height, uint32_t mips,
                                      PixelFormat format,
                                      void* buffer, size_t buffer_size) {
  YASSERT(format > 0 && format < ARRAY_SIZE(kTextureFormats),
          "Invalid Pixel Format: %d", static_cast<int>(format));
  const D3DFORMAT d3d_format = kTextureFormats[format];

  YASSERT(IS_POWER_OF_2(width),
          "Texture width must be power of 2: %u", width);
  YASSERT(IS_POWER_OF_2(height),
          "Texture height must be power of 2: %u", height);

  const TextureID texture_id = gTextures.AllocateID();
  YASSERT(texture_id != static_cast<TextureID>(-1),
          "Maximum number of textures reached.");

  TextureData& texture_data = gTextures.data[texture_id];
  const bool is_dynamic = (type == kUsageType_Dynamic);
  const DWORD usage = is_dynamic ? D3DUSAGE_DYNAMIC : 0;
  const D3DPOOL pool = is_dynamic ? D3DPOOL_DEFAULT : D3DPOOL_SYSTEMMEM;
  HRESULT hr = gD3DDevice->CreateTexture(
      width,                 // [in]           UINT Width,
      height,                // [in]           UINT Height,
      mips,                  // [in]           UINT Levels,
      usage,                 // [in]           DWORD Usage,
      d3d_format,            // [in]           D3DFORMAT Format,
      pool,                  // [in]           D3DPOOL Pool,
      &texture_data.texture, // [out, retval]  IDirect3DTexture9 **ppTexture,
      NULL                   // [in]           HANDLE *pSharedHandle
  );
  YASSERT(hr == D3D_OK, "Could not create render target texture.");

  texture_data.format = d3d_format;
  texture_data.width = width;
  texture_data.height = height;
  texture_data.mips = mips;

  switch (type) {
    case kUsageType_Immutable:
      YASSERT(buffer && buffer_size > 0,
              "Immutable texture must be initialized upon creation.");

      // DX9 does not support immutable resources, simply set it to static and
      // fill it before setting it to immutable.
      texture_data.usage = kUsageType_Static;
      FillTexture(texture_id, buffer, buffer_size);
      texture_data.usage = kUsageType_Immutable;
      break;

    case kUsageType_Static:
    case kUsageType_Dynamic:
      texture_data.usage = type;
      if (buffer_size > 0)
        FillTexture(texture_id, buffer, buffer_size);
      break;

    default:
      YFATAL("Invalid Usage Type used in Texture Creation: %d",
             static_cast<int>(type));
  }

  (void) hr;
  return texture_id;
}

VertexBufferID RenderDevice::CreateVertexBuffer(UsageType type, uint32_t stride,
                                                uint32_t count, void* buffer,
                                                size_t buffer_size) {
  const VertexBufferID vert_id = gVertexBuffers.AllocateID();
  YASSERT(vert_id != static_cast<VertexBufferID>(-1),
          "Maximum number of vertex buffers reached.");

  VertBuffData& vertex_buffer_data = gVertexBuffers.data[vert_id];
  const bool is_dynamic = (type == kUsageType_Dynamic);
  const DWORD usage = (is_dynamic ? D3DUSAGE_DYNAMIC : 0) | D3DUSAGE_WRITEONLY;
  const D3DPOOL pool = is_dynamic ? D3DPOOL_DEFAULT : D3DPOOL_SYSTEMMEM;

  HRESULT hr = gD3DDevice->CreateVertexBuffer(
      static_cast<UINT>(stride * count), // [in]           UINT Length,
      usage,                             // [in]           DWORD Usage,
      0,                                 // [in]           DWORD FVF,
      pool,                              // [in]           D3DPOOL Pool,
      &vertex_buffer_data.vertex_buffer, // [out, retval]  ppVertexBuffer,
      NULL                               // [in]           HANDLE *pSharedHandle
  );

  vertex_buffer_data.stride = stride;
  vertex_buffer_data.total_count = count;
  vertex_buffer_data.count = 0;

  switch (type) {
    case kUsageType_Immutable:
      YASSERT(buffer && buffer_size > 0,
              "Immutable vertex buffer must be initialized upon creation.");

      // DX9 does not support immutable resources, simply set it to static and
      // fill it before setting it to immutable.
      vertex_buffer_data.usage = kUsageType_Static;
      FillVertexBuffer(vert_id, count, buffer, buffer_size);
      vertex_buffer_data.usage = kUsageType_Immutable;
      break;

    case kUsageType_Static:
    case kUsageType_Dynamic:
      vertex_buffer_data.usage = type;
      if (buffer_size > 0)
        FillVertexBuffer(vert_id, count, buffer, buffer_size);
      break;

    default:
      YFATAL("Invalid Usage Type used in Vertex Buffer Creation: %d",
             static_cast<int>(type));
  }

  (void) hr;
  return vert_id;
}

IndexBufferID RenderDevice::CreateIndexBuffer(UsageType type, uint32_t count,
                                              void* buffer,
                                              size_t buffer_size) {
  const IndexBufferID indx_id = gIndexBuffers.AllocateID();
  YASSERT(indx_id != static_cast<IndexBufferID>(-1),
          "Maximum number of index buffers reached.");

  IndexBuffData& index_buffer_data = gIndexBuffers.data[indx_id];
  const bool is_dynamic = (type == kUsageType_Dynamic);
  const DWORD usage = (is_dynamic ? D3DUSAGE_DYNAMIC : 0) | D3DUSAGE_WRITEONLY;
  const D3DPOOL pool = is_dynamic ? D3DPOOL_DEFAULT : D3DPOOL_SYSTEMMEM;

  HRESULT hr = gD3DDevice->CreateIndexBuffer(
      count * sizeof(uint16_t),        // [in]           UINT Length,
      usage,                           // [in]           DWORD Usage,
      D3DFMT_INDEX16,                  // [in]           D3DFORMAT Format,
      pool,                            // [in]           D3DPOOL Pool,
      &index_buffer_data.index_buffer, // [out, retval]  **ppIndexBuffer,
      NULL                             // [in]           HANDLE *pSharedHandle
  );

  index_buffer_data.total_count = count;
  index_buffer_data.count = 0;

  switch (type) {
    case kUsageType_Immutable:
      YASSERT(buffer && buffer_size > 0,
              "Immutable index buffer must be initialized upon creation.");

      // DX9 does not support immutable resources, simply set it to static and
      // fill it before setting it to immutable.
      index_buffer_data.usage = kUsageType_Static;
      FillIndexBuffer(indx_id, count, buffer, buffer_size);
      index_buffer_data.usage = kUsageType_Immutable;
      break;

    case kUsageType_Static:
    case kUsageType_Dynamic:
      index_buffer_data.usage = type;
      if (buffer_size > 0)
        FillIndexBuffer(indx_id, count, buffer, buffer_size);
      break;

    default:
      YFATAL("Invalid Usage Type used in Index Buffer Creation: %d",
             static_cast<int>(type));
  }

  (void) hr;
  return indx_id;
}

ConstantBufferID RenderDevice::CreateConstantBuffer(UsageType type, size_t size,
                                                    void* buffer,
                                                    size_t buffer_size) {
  const ConstantBufferID buff_id = gConstBuffers.AllocateID();
  YASSERT(buff_id != static_cast<ConstantBufferID>(-1),
          "Maximum number of constant buffers reached.");
  YASSERT(size % (sizeof(float) * 4),
          "Constant buffer size must be a multiple of %u bytes: %u",
          static_cast<uint32_t>(sizeof(float) * 4),
          static_cast<uint32_t>(size));

  void* allocated_data = AllocateMemory(size);
  memset(allocated_data, 0, size);

  ConstBuffData& const_buffer_data = gConstBuffers.data[buff_id];
  const_buffer_data.buffer_data = allocated_data;
  const_buffer_data.size = size;

  switch (type) {
    case kUsageType_Immutable:
      YASSERT(buffer && buffer_size > 0,
              "Immutable constant buffer must be initialized upon creation.");

      // DX9 does not support immutable resources, simply set it to static and
      // fill it before setting it to immutable.
      const_buffer_data.usage = kUsageType_Static;
      FillConstantBuffer(buff_id, buffer, buffer_size);
      const_buffer_data.usage = kUsageType_Immutable;
      break;

    case kUsageType_Static:
    case kUsageType_Dynamic:
      const_buffer_data.usage = type;
      if (buffer_size > 0)
        FillConstantBuffer(buff_id, buffer, buffer_size);
      break;

    default:
      YFATAL("Invalid Usage Type used in constant Buffer Creation: %d",
              static_cast<int>(type));
  }

  return buff_id;
}

// Modifiers
void RenderDevice::FillTexture(TextureID texture, void* buffer, size_t size) {
  YASSERT(texture < ARRAY_SIZE(gTextures.data) && gTextures.used[texture],
          "Filling Invalid Texture ID: %d.", static_cast<int>(texture));

  TextureData& texture_data = gTextures.data[texture];
  uint32_t width = texture_data.width;
  uint32_t height = texture_data.height;
  uint32_t used_size = 0;

  const uint32_t num_mips = texture_data.mips;
  for (uint32_t i = 0; i < num_mips; ++i) {
    uint32_t current_size = width * height;
    YASSERT(used_size + current_size <= size,
            "Texture fill buffer size (%u) too small for texture (%ux%ux%u).",
            static_cast<uint32_t>(size), texture_data.width,
            texture_data.height, texture_data.mips);

    FillTextureMip(texture, i,
                                 static_cast<uint8_t*>(buffer) + used_size,
                                 current_size);

    width = (width <= 1) ? 1 : (width >> 1);
    height = (height <= 1) ? 1 : (height >> 1);
    used_size += current_size;
  }

  YASSERT(used_size == size,
          "Texture fill buffer size (%u) did not match texture (%ux%ux%u).",
          static_cast<uint32_t>(size), texture_data.width,
          texture_data.height, texture_data.mips);
}

void RenderDevice::FillTextureMip(TextureID texture, uint32_t mip,
                                  void* buffer, size_t size) {
  YASSERT(texture < ARRAY_SIZE(gTextures.data) && gTextures.used[texture],
          "Filling Invalid Texture ID: %d.", static_cast<int>(texture));

  TextureData& texture_data = gTextures.data[texture];
  HRESULT hr = E_FAIL;
  (void) hr;

  YASSERT(texture_data.usage == kUsageType_Static ||
          texture_data.usage == kUsageType_Dynamic,
          "Cannot fill texture with usage type: %d.",
          static_cast<int>(texture_data.usage));

  uint32_t width = texture_data.width >> mip;
  if (0 == width)
    width = 1;

  uint32_t height = texture_data.height >> mip;
  if (0 == height)
    height = 1;

  YASSERT(size == width * height,
          "Invalid texture fill size (%u) for mip (%u) for texture %ux%u.",
          static_cast<uint32_t>(size), mip,
          texture_data.width, texture_data.height);

  D3DLOCKED_RECT locked_rect;
  hr = texture_data.texture->LockRect(mip, &locked_rect, NULL, D3DLOCK_DISCARD);
  YASSERT(hr == D3D_OK,
          "Could not lock texture (%u): 0x%08X.",
          static_cast<uint32_t>(texture),
          static_cast<uint32_t>(hr));

  memcpy(locked_rect.pBits, buffer, size);

  hr = texture_data.texture->UnlockRect(mip);
  YASSERT(hr == D3D_OK,
          "Could not unlock texture (%u): 0x%08X.",
          static_cast<uint32_t>(texture),
          static_cast<uint32_t>(hr));
}

void RenderDevice::ResetVertexBuffer(VertexBufferID vertex_buffer) {
  YASSERT(vertex_buffer < ARRAY_SIZE(gVertexBuffers.data) &&
          gVertexBuffers.used[vertex_buffer],
          "Resetting Invalid Vertex Buffer ID: %d.",
          static_cast<int>(vertex_buffer));

  gVertexBuffers.data[vertex_buffer].count = 0;
}

void RenderDevice::AppendVertexBuffer(VertexBufferID vertex_buffer,
                                      uint32_t count,
                                      void* buffer, size_t buffer_size,
                                      uint32_t* starting_offset) {
  YASSERT(vertex_buffer < ARRAY_SIZE(gVertexBuffers.data) &&
          gVertexBuffers.used[vertex_buffer],
          "Appending to Invalid Vertex Buffer ID: %d.",
          static_cast<int>(vertex_buffer));

  VertBuffData& vertex_buffer_data = gVertexBuffers.data[vertex_buffer];
  uint32_t prev_count = YCommon::AtomicAdd32(&vertex_buffer_data.count,
                                             count);
  YASSERT(prev_count + count < vertex_buffer_data.total_count,
          "Cannot append to Vertex Buffer ID (%d), maximum count reached.",
          static_cast<int>(vertex_buffer));

  FillVertexBuffer(vertex_buffer, count, buffer, buffer_size, prev_count);
  if (starting_offset)
    *starting_offset = prev_count;
}

void RenderDevice::FillVertexBuffer(VertexBufferID vertex_buffer,
                                    uint32_t count,
                                    void* buffer, size_t buffer_size,
                                    uint32_t index_offset) {
  HRESULT hr = E_FAIL;
  (void) hr;

  YASSERT(vertex_buffer < ARRAY_SIZE(gVertexBuffers.data) &&
          gVertexBuffers.used[vertex_buffer],
          "Filling Invalid Vertex Buffer ID: %d.",
          static_cast<int>(vertex_buffer));

  VertBuffData& vertex_buffer_data = gVertexBuffers.data[vertex_buffer];
  YASSERT(vertex_buffer_data.usage == kUsageType_Static ||
          vertex_buffer_data.usage == kUsageType_Dynamic,
          "Cannot fill vertex buffer with usage type: %d.",
          static_cast<int>(vertex_buffer_data.usage));

  const uint32_t index_end = index_offset + count;
  YASSERT(index_end > vertex_buffer_data.total_count,
          "Could not fill vertex buffer ID %d, maximum count exceeded.",
          static_cast<int>(vertex_buffer));

  for (uint32_t current_count = vertex_buffer_data.count;
       index_end < current_count;
       current_count = vertex_buffer_data.count) {
    if (YCommon::AtomicCmpSet32(&vertex_buffer_data.count, current_count,
                                index_end)) {
      break;
    }
  }

  const uint32_t stride = vertex_buffer_data.stride;
  const uint32_t fill_index = index_offset * stride;
  const size_t fill_size = count * stride;
  YASSERT(fill_size == buffer_size,
          "Vertex Fill size (%u) did not match buffer size (%u).",
          static_cast<uint32_t>(fill_size),
          static_cast<uint32_t>(buffer_size));

  const DWORD flags = (vertex_buffer_data.usage == kUsageType_Dynamic) ?
                      D3DLOCK_NOOVERWRITE :
                      0;
  void* data = NULL;
  hr = vertex_buffer_data.vertex_buffer->Lock(
      static_cast<UINT>(fill_index),  // [in]   UINT OffsetToLock,
      static_cast<UINT>(buffer_size), // [in]   UINT SizeToLock,
      &data,                          // [out]  VOID **ppbData,
      flags                           // [in]   DWORD Flags
  );
  YASSERT(D3D_OK == hr,
          "Could not lock vertex buffer ID (%u): 0x%08X.",
          static_cast<uint32_t>(vertex_buffer),
          static_cast<uint32_t>(hr));

  memcpy(data, buffer, buffer_size);

  hr = vertex_buffer_data.vertex_buffer->Unlock();
  YASSERT(D3D_OK == hr,
          "Could not unlock vertex buffer ID (%u): 0x%08X.",
          static_cast<uint32_t>(vertex_buffer),
          static_cast<uint32_t>(hr));
}

void RenderDevice::ResetIndexBuffer(IndexBufferID index_buffer) {
  YASSERT(index_buffer < ARRAY_SIZE(gIndexBuffers.data) &&
          gIndexBuffers.used[index_buffer],
          "Resetting Invalid Index Buffer ID: %d.",
          static_cast<int>(index_buffer));

  gIndexBuffers.data[index_buffer].count = 0;
}

void RenderDevice::AppendIndexBuffer(IndexBufferID index_buffer, uint32_t count,
                                     void* buffer, size_t buffer_size,
                                     uint32_t* starting_offset) {
  YASSERT(index_buffer < ARRAY_SIZE(gIndexBuffers.data) &&
          gIndexBuffers.used[index_buffer],
          "Appending to Invalid Index Buffer ID: %d.",
          static_cast<int>(index_buffer));

  IndexBuffData& index_buffer_data = gIndexBuffers.data[index_buffer];
  uint32_t prev_count = YCommon::AtomicAdd32(&index_buffer_data.count, count);
  YASSERT(prev_count + count < index_buffer_data.total_count,
          "Cannot append to Index Buffer ID (%d), maximum count reached.",
          static_cast<int>(index_buffer));

  FillIndexBuffer(index_buffer, count, buffer, buffer_size, prev_count);
  if (starting_offset)
    *starting_offset = prev_count;
}

void RenderDevice::FillIndexBuffer(IndexBufferID index_buffer, uint32_t count,
                                   void* buffer, size_t buffer_size,
                                   uint32_t index_offset) {
  HRESULT hr = E_FAIL;
  (void) hr;

  YASSERT(index_buffer < ARRAY_SIZE(gIndexBuffers.data) &&
          gIndexBuffers.used[index_buffer],
          "Filling Invalid Index Buffer ID: %d.",
          static_cast<int>(index_buffer));

  IndexBuffData& index_buffer_data = gIndexBuffers.data[index_buffer];
  YASSERT(index_buffer_data.usage == kUsageType_Static ||
          index_buffer_data.usage == kUsageType_Dynamic,
          "Cannot fill index buffer with usage type: %d.",
          static_cast<int>(index_buffer_data.usage));

  const uint32_t index_end = index_offset + count;
  YASSERT(index_end > index_buffer_data.total_count,
          "Could not fill index buffer ID %d, maximum count exceeded.",
          static_cast<int>(index_buffer));

  for (uint32_t current_count = index_buffer_data.count;
       index_end < current_count;
       current_count = index_buffer_data.count) {
    if (YCommon::AtomicCmpSet32(&index_buffer_data.count, current_count,
                                index_end)) {
      break;
    }
  }

  const size_t fill_size = count * sizeof(uint16_t);
  YASSERT(fill_size == buffer_size,
          "Index Fill size (%u) did not match buffer size (%u).",
          static_cast<uint32_t>(fill_size),
          static_cast<uint32_t>(buffer_size));

  const DWORD flags = (index_buffer_data.usage == kUsageType_Dynamic) ?
                      D3DLOCK_NOOVERWRITE :
                      0;
  void* data = NULL;
  hr = index_buffer_data.index_buffer->Lock(
      static_cast<UINT>(index_offset), // [in]   UINT OffsetToLock,
      static_cast<UINT>(buffer_size),  // [in]   UINT SizeToLock,
      &data,                           // [out]  VOID **ppbData,
      flags                            // [in]   DWORD Flags
  );
  YASSERT(D3D_OK == hr,
          "Could not lock vertex buffer ID (%u): 0x%08X.",
          static_cast<uint32_t>(index_buffer),
          static_cast<uint32_t>(hr));

  memcpy(data, buffer, buffer_size);

  hr = index_buffer_data.index_buffer->Unlock();
  YASSERT(D3D_OK == hr,
          "Could not unlock vertex buffer ID (%u): 0x%08X.",
          static_cast<uint32_t>(index_buffer),
          static_cast<uint32_t>(hr));
}

void RenderDevice::FillConstantBuffer(ConstantBufferID constant_buffer,
                                      void* buffer, size_t size) {
   YASSERT(constant_buffer < ARRAY_SIZE(gConstBuffers.data) &&
          gConstBuffers.used[constant_buffer],
          "Filling Invalid Constant Buffer ID: %d.",
          static_cast<int>(constant_buffer));

  ConstBuffData& const_buffer_data = gConstBuffers.data[constant_buffer];
  YASSERT(const_buffer_data.usage == kUsageType_Static ||
          const_buffer_data.usage == kUsageType_Dynamic,
          "Cannot fill constant buffer with usage type: %d.",
          static_cast<int>(const_buffer_data.usage));
  YASSERT(const_buffer_data.size == size,
          "Cannot fill constant buffer of size %u with size %u.",
          static_cast<uint32_t>(const_buffer_data.size),
          static_cast<uint32_t>(size));

  memcpy(const_buffer_data.buffer_data, buffer, size);
}

// Accessors
uint32_t RenderDevice::GetVertexBufferCount(VertexBufferID vertex_buffer) {
  YASSERT(vertex_buffer < ARRAY_SIZE(gVertexBuffers.data) &&
          gVertexBuffers.used[vertex_buffer],
          "Accessing Invalid Vertex Buffer ID: %d.",
          static_cast<int>(vertex_buffer));

  return gVertexBuffers.data[vertex_buffer].count;
}

uint32_t RenderDevice::GetIndexBufferCount(IndexBufferID index_buffer) {
  YASSERT(index_buffer < ARRAY_SIZE(gIndexBuffers.data) &&
          gIndexBuffers.used[index_buffer],
          "Accessing Invalid Vertex Buffer ID: %d.",
          static_cast<int>(index_buffer));

  return gIndexBuffers.data[index_buffer].count;
}

// Releases
void RenderDevice::ReleaseRenderTarget(RenderTargetID render_target) {
  YASSERT(render_target < ARRAY_SIZE(gSurfaces.data) &&
          gSurfaces.used[render_target],
          "Releasing Invalid Render Target ID: %d",
          static_cast<int>(render_target));
  YASSERT(render_target != GetBackBufferRenderTarget(),
          "Cannot release back buffer render target!");
  gSurfaces.ReleaseID(render_target);
}

void RenderDevice::ReleaseVertexDeclaration(VertexDeclID vertex_decl) {
  YASSERT(vertex_decl < ARRAY_SIZE(gVertDecls.data) &&
          gVertDecls.used[vertex_decl],
          "Releasing Invalid Vertex Declaration ID: %d",
          static_cast<int>(vertex_decl));
  gVertDecls.ReleaseID(vertex_decl);
}

void RenderDevice::ReleaseTexture(TextureID texture) {
  YASSERT(texture < ARRAY_SIZE(gTextures.data) && gTextures.used[texture],
          "Releasing Invalid Texture ID: %d", static_cast<int>(texture));
  YASSERT(gTextures.data[texture].usage != kUsageType_System,
          "Cannot release system texture: %d",
          static_cast<int>(texture));
  gTextures.ReleaseID(texture);
}

void RenderDevice::ReleaseVertexBuffer(VertexBufferID vertex_buffer) {
  YASSERT(vertex_buffer < ARRAY_SIZE(gVertexBuffers.data) &&
          gVertexBuffers.used[vertex_buffer],
          "Releasing Invalid Vertex Buffer ID: %d",
          static_cast<int>(vertex_buffer));
  YASSERT(gVertexBuffers.data[vertex_buffer].usage != kUsageType_System,
          "Cannot release system vertex buffer ID: %d",
          static_cast<int>(vertex_buffer));
  gVertexBuffers.ReleaseID(vertex_buffer);
}

void RenderDevice::ReleaseIndexBuffer(IndexBufferID index_buffer) {
  YASSERT(index_buffer < ARRAY_SIZE(gIndexBuffers.data) &&
          gIndexBuffers.used[index_buffer],
          "Releasing Invalid Index Buffer ID: %d",
          static_cast<int>(index_buffer));
  YASSERT(gIndexBuffers.data[index_buffer].usage != kUsageType_System,
          "Cannot release system index buffer ID: %d",
          static_cast<int>(index_buffer));
  gIndexBuffers.ReleaseID(index_buffer);
}

void RenderDevice::ReleaseConstantBuffer(ConstantBufferID constant_buffer) {
  YASSERT(constant_buffer < ARRAY_SIZE(gConstBuffers.data) &&
          gConstBuffers.used[constant_buffer],
          "Releasing Invalid Constant Buffer ID: %d",
          static_cast<int>(constant_buffer));
  YASSERT(gConstBuffers.data[constant_buffer].usage != kUsageType_System,
          "Cannot release system constant buffer ID: %d",
          static_cast<int>(constant_buffer));
  gConstBuffers.ReleaseID(constant_buffer);
}

// Activations
void RenderDevice::ActivateViewPort(uint32_t top, uint32_t left,
                                    uint32_t width, uint32_t height,
                                    float min_z, float max_z) {
  D3DVIEWPORT9 d3dVP;
  d3dVP.X = left;
  d3dVP.Y = top;
  d3dVP.Width = width;
  d3dVP.Height = height;
  d3dVP.MinZ = min_z;
  d3dVP.MaxZ = max_z;

  HRESULT hr = gD3DDevice->SetViewport(&d3dVP);
  YASSERT(hr == D3D_OK, "Could not set viewport (0x%08x).",
          static_cast<uint32_t>(hr));
  (void) hr;
}

void RenderDevice::ActivateRenderTarget(int target,
                                        RenderTargetID render_target) {
  HRESULT hr = E_FAIL;
  (void) hr;

  if (render_target == GetNullRenderTarget()) {
    hr = gD3DDevice->SetRenderTarget(static_cast<DWORD>(target), NULL);
    YASSERT(hr == D3D_OK, "Could not clear render target index: %d", target);
  } else {
    YASSERT(render_target < ARRAY_SIZE(gSurfaces.data) &&
            gSurfaces.used[render_target],
            "Activating Invalid Render Target ID: %d",
            static_cast<int>(render_target));

    hr = gD3DDevice->SetRenderTarget(static_cast<DWORD>(target),
                                     gSurfaces.data[render_target].surface);
    YASSERT(hr == D3D_OK,
            "Could not set render target index (%d) to Render Target ID: %d.",
            target, static_cast<int>(render_target));
  }
}

void RenderDevice::ActivateVertexDeclaration(VertexDeclID vertex_decl) {
  YASSERT(vertex_decl < ARRAY_SIZE(gVertDecls.data) &&
          gVertDecls.used[vertex_decl],
          "Activating Invalid Vertex Declaration ID: %d",
          static_cast<int>(vertex_decl));

  HRESULT hr = E_FAIL;
  (void) hr;

  const VertexDeclData& vertex_decl_data = gVertDecls.data[vertex_decl];

  hr = gD3DDevice->SetVertexDeclaration(vertex_decl_data.decl);
  YASSERT(hr == D3D_OK,
          "Could not activate vertex declaration ID (%d).",
          static_cast<int>(vertex_decl));

  for (uint8_t i = 0; i < vertex_decl_data.num_streams; ++i) {
    hr = gD3DDevice->SetStreamSourceFreq(i,
                                         vertex_decl_data.stream_divisors[i]);
    YASSERT(hr == D3D_OK,
            "Could not set stream %u source frequency to %u.",
            static_cast<uint32_t>(i),
            static_cast<uint32_t>(vertex_decl_data.stream_divisors[i]));
  }
}

void RenderDevice::ActivateTexture(int sampler, TextureID texture) {
  YASSERT(texture < ARRAY_SIZE(gTextures.data) && gTextures.used[texture],
          "Activating Invalid Texture ID: %d", static_cast<int>(texture));

  HRESULT hr = gD3DDevice->SetTexture(static_cast<DWORD>(sampler),
                                      gTextures.data[texture].texture);
  YASSERT(hr == D3D_OK,
          "Could not set texture ID (%d) to sampler %d.",
          static_cast<int>(texture), sampler);
}

void RenderDevice::ActivateVertexStream(uint32_t stream,
                                        VertexBufferID vertex_buffer) {
  YASSERT(stream < ARRAY_SIZE(gActivatedStreams),
          "Activating Invalid Vertex Stream: %u.",
          stream);
  YASSERT(vertex_buffer < ARRAY_SIZE(gVertexBuffers.data) &&
          gVertexBuffers.used[vertex_buffer],
          "Activating Invalid Vertex Buffer ID: %d",
          static_cast<int>(vertex_buffer));

  gActivatedStreams[stream] = vertex_buffer;

  const VertBuffData& vert_buffer = gVertexBuffers.data[vertex_buffer];
  HRESULT hr = gD3DDevice->SetStreamSource(
      static_cast<UINT>(stream), // [in]  UINT StreamNumber,
      vert_buffer.vertex_buffer, // [in]  IDirect3DVertexBuffer9 *pStreamData,
      0,                         // [in]  UINT OffsetInBytes,
      vert_buffer.stride         // [in]  UINT Stride
  );
  YASSERT(hr == D3D_OK,
          "Could not set stream source (%u) to Vertex buffer ID: %d.",
          stream, static_cast<int>(vertex_buffer));
  (void) hr;
}

void RenderDevice::ActivateIndexStream(IndexBufferID index_buffer) {
  YASSERT(index_buffer < ARRAY_SIZE(gIndexBuffers.data) &&
          gIndexBuffers.used[index_buffer],
          "Activating Invalid Index Buffer ID: %d",
          static_cast<int>(index_buffer));

  const IndexBuffData& indx_buffer = gIndexBuffers.data[index_buffer];
  HRESULT hr = gD3DDevice->SetIndices(indx_buffer.index_buffer);
  YASSERT(hr == D3D_OK,
          "Could not activate index buffer ID: %d.",
          static_cast<int>(index_buffer));
  (void) hr;
}

void RenderDevice::ActivateConstantBuffer(int start_reg,
                                          ConstantBufferID constant_buffer) {
  YASSERT(constant_buffer < ARRAY_SIZE(gConstBuffers.data) &&
          gConstBuffers.used[constant_buffer],
          "Activating Invalid Constant Buffer ID: %d",
          static_cast<int>(constant_buffer));

  const ConstBuffData& const_buffer = gConstBuffers.data[constant_buffer];
  const int regs = static_cast<int>(const_buffer.size / (sizeof(float) * 4));

  HRESULT hr = E_FAIL;
  (void) hr;
  for (int i = start_reg; i < regs; ++i) {
    hr = gD3DDevice->SetVertexShaderConstantF(
        i,
        static_cast<float*>(const_buffer.buffer_data),
        4);
    YASSERT(hr == D3D_OK,
            "Could not activate constant buffer %d",
            static_cast<int>(constant_buffer));
  }
}

void RenderDevice::ActivateDrawPrimitive(DrawPrimitive draw_primitive) {
  YASSERT(draw_primitive < 0 || draw_primitive >= NUM_DRAW_PRIMITIVES,
          "Invalid Draw Primitive: %d", static_cast<int>(draw_primitive));
  gActivatedDrawPrimitiveType = kPrimitiveDrawTypes[draw_primitive];
  gDrawPoints = kPrimitiveDrawPoints[draw_primitive];
}

// Draw
void RenderDevice::Draw(uint32_t start_vertex, uint32_t num_verts) {
  YASSERT(gDrawPoints == 0, "Draw Primitive not activated.");
  YASSERT(gActivatedStreams[0] != -1, "Vertex Stream not activated.");

  int num_primitives = 0;
  if (gDrawPoints < 0) {
    num_primitives = static_cast<int>(num_verts) + gDrawPoints;
    YASSERT(num_primitives > 0,
            "Not enough vertices supplied: %d.",
            static_cast<int>(num_verts));
  } else {
    YASSERT(num_verts % gDrawPoints == 0,
            "Number of vertices (%d) is not a multiple of %d.",
            static_cast<int>(num_verts), gDrawPoints);
    num_primitives = static_cast<int>(num_verts) / gDrawPoints;
  }

  HRESULT hr = gD3DDevice->DrawPrimitive(gActivatedDrawPrimitiveType,
                                         start_vertex,
                                         num_primitives);
  YASSERT(hr == D3D_OK,
          "Could not draw primitives: 0x%08x.",
          static_cast<uint32_t>(hr));

  (void) hr;
}

void RenderDevice::DrawInstanced(uint32_t start_vertex,
                                 uint32_t verts_per_instance,
                                 uint32_t start_instance,
                                 uint32_t num_instances) {
  const uint32_t instance_vertex_begin = start_vertex +
                                         (start_instance * verts_per_instance);
  const uint32_t instance_verts = num_instances * verts_per_instance;
  Draw(instance_vertex_begin, instance_verts);
}

void RenderDevice::DrawIndexed(uint32_t start_index, uint32_t num_indexes,
                               uint32_t vertex_offset) {
  YASSERT(gDrawPoints == 0, "Draw Primitive not activated.");
  YASSERT(gActivatedStreams[0] != static_cast<VertexBufferID>(-1),
          "Vertex Stream not activated.");

  int num_primitives = 0;
  if (gDrawPoints < 0) {
    num_primitives = static_cast<int>(num_indexes) + gDrawPoints;
    YASSERT(num_primitives > 0,
            "Not enough indices supplied: %d.",
            static_cast<int>(num_indexes));
  } else {
    YASSERT(num_indexes % gDrawPoints == 0,
            "Number of indices (%d) is not a multiple of %d.",
            static_cast<int>(num_indexes), gDrawPoints);
    num_primitives = static_cast<int>(num_indexes) / gDrawPoints;
  }

  VertexBufferID vert_id = gActivatedStreams[0];
  const VertBuffData& vertex_buffer_data = gVertexBuffers.data[vert_id];

  HRESULT hr = gD3DDevice->DrawIndexedPrimitive(
      gActivatedDrawPrimitiveType,     // [in]  D3DPRIMITIVETYPE Type,
      static_cast<int>(vertex_offset), // [in]  INT BaseVertexIndex,
      0,                               // [in]  UINT MinIndex,
      vertex_buffer_data.total_count,  // [in]  UINT NumVertices,
      start_index,                     // [in]  UINT StartIndex,
      num_primitives                   // [in]  UINT PrimitiveCount
  );
    
                                                
                                                
                                                
                                                
  YASSERT(hr == D3D_OK,
          "Could not draw primitives: 0x%08x.",
          static_cast<uint32_t>(hr));

  (void) hr;
}

void RenderDevice::DrawIndexedInstanced(uint32_t start_index,
                                        uint32_t index_per_instance,
                                        uint32_t start_instance,
                                        uint32_t num_instances,
                                        uint32_t vertex_offset) {
  const uint32_t instance_index_begin = start_index +
                                        (start_instance * index_per_instance);
  const uint32_t instance_indexes = num_instances * index_per_instance;
  DrawIndexed(instance_index_begin, instance_indexes, vertex_offset);
}

// Render
void RenderDevice::Begin() {
  HRESULT hr = gD3DDevice->BeginScene();
  YASSERT(hr == D3D_OK,
          "BeginScene() was not successful.");
  (void) hr;
}

void RenderDevice::End() {
  HRESULT hr = gD3DDevice->EndScene();
  YASSERT(hr == D3D_OK,
          "EndScene() was not successful.");
  (void) hr;
}

void RenderDevice::Present() {
  HRESULT hr = gD3DDevice->Present(NULL, NULL, NULL, NULL);
  YASSERT(hr == D3D_OK,
          "Present() was not successful.");
  (void) hr;
}

}} // namespace YEngine { namespace YRenderDevice {
