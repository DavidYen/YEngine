#include <YCommon/Headers/stdincludes.h>
#include "RenderDevice_Mock.h"

#include <gmock/gmock.h>
#include <YCommon/YPlatform/PlatformHandle.h>

#include "RenderBlendState.h"
#include "SamplerState.h"
#include "VertexDeclElement.h"

using ::testing::Return;
using ::testing::_;

namespace YEngine { namespace YRenderDevice {

namespace {
  class PureRenderDevice {
   public:
    PureRenderDevice() {}
    virtual ~PureRenderDevice() {}

    // System Values
    virtual RenderTargetID GetNullRenderTarget() = 0;
    virtual RenderTargetID GetBackBufferRenderTarget() = 0;
    virtual TextureID GetRenderTargetTexture(RenderTargetID render_target) = 0;

    // Creates
    virtual ViewPortID CreateViewPort(uint32_t top, uint32_t left,
                                      uint32_t width, uint32_t height,
                                      float min_z, float max_z) = 0;
    virtual RenderBlendStateID CreateRenderBlendState(
        const RenderBlendState& state) = 0;
    virtual RenderTargetID CreateRenderTarget(uint32_t width, uint32_t height,
                                              PixelFormat format) = 0;
    virtual VertexDeclID CreateVertexDeclaration(VertexDeclElement* elements,
                                                 size_t num_elements) = 0;
    virtual VertexShaderID CreateVertexShader(const void* shader_data,
                                              size_t shader_size) = 0;
    virtual PixelShaderID CreatePixelShader(const void* shader_data,
                                            size_t shader_size) = 0;
    virtual SamplerStateID CreateSamplerState(const SamplerState& state) = 0;
    virtual TextureID CreateTexture(UsageType type,
                                    uint32_t width, uint32_t height,
                                    uint32_t mips, PixelFormat format,
                                    void* buffer, size_t buffer_size) = 0;
    virtual VertexBufferID CreateVertexBuffer(UsageType type, uint32_t stride,
                                              uint32_t count, void* buffer,
                                              size_t buffer_size) = 0;
    virtual IndexBufferID CreateIndexBuffer(UsageType type, uint32_t count,
                                            void* buffer,
                                            size_t buffer_size) = 0;
    virtual ConstantBufferID CreateConstantBuffer(UsageType type, size_t size,
                                                  void* buffer,
                                                  size_t buffer_size) = 0;

    // Command List
    virtual void BeginRecord() = 0;
    virtual CommandListID EndRecord() = 0;

    // Modifiers
    virtual void SetViewPort(ViewPortID viewport,
                             uint32_t top, uint32_t left,
                             uint32_t width, uint32_t height,
                             float min_z, float max_z) = 0;
    virtual void FillTexture(TextureID texture,
                             void* buffer, size_t size) = 0;
    virtual void FillTextureMip(TextureID texture, uint32_t mip,
                                void* buffer, size_t size) = 0;
    virtual void ResetVertexBuffer(VertexBufferID vertex_buffer) = 0;
    virtual void AppendVertexBuffer(VertexBufferID vertex_buffer,
                                    uint32_t count,
                                    void* buffer, size_t buffer_size,
                                    uint32_t* starting_offset) = 0;
    virtual void FillVertexBuffer(VertexBufferID vertex_buffer, uint32_t count,
                                  void* buffer, size_t buffer_size,
                                  uint32_t index_offset) = 0;
    virtual void ResetIndexBuffer(IndexBufferID index_buffer) = 0;
    virtual void AppendIndexBuffer(IndexBufferID index_buffer, uint32_t count,
                                   void* buffer, size_t buffer_size,
                                   uint32_t* starting_offset) = 0;
    virtual void FillIndexBuffer(IndexBufferID index_buffer, uint32_t count,
                                 void* buffer, size_t buffer_size,
                                 uint32_t index_offset) = 0;
    virtual void FillConstantBuffer(ConstantBufferID constant_buffer,
                                    void* buffer, size_t size) = 0;

    // Accessors
    virtual void GetViewPort(ViewPortID viewport,
                             uint32_t& top, uint32_t& left,
                             uint32_t& width, uint32_t& height,
                             float& min_z, float& max_z) = 0;
    virtual uint32_t GetVertexBufferCount(VertexBufferID vertex_buffer) = 0;
    virtual uint32_t GetIndexBufferCount(IndexBufferID index_buffer) = 0;

    // Releases
    virtual void ReleaseViewPort(ViewPortID viewport) = 0;
    virtual void ReleaseRenderBlendState(RenderBlendStateID state) = 0;
    virtual void ReleaseRenderTarget(RenderTargetID render_target) = 0;
    virtual void ReleaseVertexDeclaration(VertexDeclID vertex_decl) = 0;
    virtual void ReleaseVertexShader(VertexShaderID shader) = 0;
    virtual void ReleasePixelShader(PixelShaderID shader) = 0;
    virtual void ReleaseSamplerState(SamplerStateID state) = 0;
    virtual void ReleaseTexture(TextureID texture) = 0;
    virtual void ReleaseVertexBuffer(VertexBufferID vertex_buffer) = 0;
    virtual void ReleaseIndexBuffer(IndexBufferID index_buffer) = 0;
    virtual void ReleaseConstantBuffer(ConstantBufferID constant_buffer) = 0;
    virtual void ReleaseCommandList(CommandListID command_list) = 0;

    // Activations
    virtual void ActivateViewPort(ViewPortID viewport) = 0;
    virtual void ActivateRenderBlendState(RenderBlendStateID blend_state) = 0;
    virtual void ActivateRenderTarget(int target,
                                      RenderTargetID render_target) = 0;
    virtual void ActivateVertexDeclaration(VertexDeclID vertex_decl) = 0;
    virtual void ActivateVertexShader(VertexShaderID shader) = 0;
    virtual void ActivatePixelShader(PixelShaderID shader) = 0;
    virtual void ActivateSamplerState(int sampler, SamplerStateID sampler_state) = 0;
    virtual void ActivateTexture(int sampler, TextureID texture) = 0;
    virtual void ActivateVertexStream(uint32_t stream,
                                      VertexBufferID vertex_buffer) = 0;
    virtual void ActivateIndexStream(IndexBufferID index_buffer) = 0;
    virtual void ActivateConstantBuffer(int start_reg,
                                        ConstantBufferID constant_buffer) = 0;
    virtual void ActivateDrawPrimitive(DrawPrimitive draw_primitive) = 0;

    // Draw
    virtual void Draw(uint32_t start_vertex, uint32_t num_verts) = 0;
    virtual void DrawInstanced(uint32_t start_vertex,
                               uint32_t verts_per_instance,
                               uint32_t start_instance,
                               uint32_t num_instances) = 0;
    virtual void DrawIndexed(uint32_t start_index, uint32_t num_indexes,
                             uint32_t vertex_offset) = 0;
    virtual void DrawIndexedInstanced(uint32_t start_index,
                                      uint32_t index_per_instance,
                                      uint32_t start_instance,
                                      uint32_t num_instances,
                                      uint32_t vertex_offset) = 0;

    // Render
    virtual void ExecuteCommandList(CommandListID commands) = 0;
    virtual void Present() = 0;
  };

  class MockRenderDevice : public PureRenderDevice {
   public:
    MockRenderDevice() : PureRenderDevice() {}
    ~MockRenderDevice() override {}

    MOCK_METHOD0(GetNullRenderTarget, RenderTargetID());
    MOCK_METHOD0(GetBackBufferRenderTarget, RenderTargetID());
    MOCK_METHOD1(GetRenderTargetTexture,
                 TextureID(RenderTargetID render_target));

    // Creates
    MOCK_METHOD6(CreateViewPort, ViewPortID(uint32_t top, uint32_t left,
                                            uint32_t width, uint32_t height,
                                            float min_z, float max_z));
    MOCK_METHOD1(CreateRenderBlendState,
                 RenderBlendStateID(const RenderBlendState& state));
    MOCK_METHOD3(CreateRenderTarget,
                 RenderTargetID(uint32_t width, uint32_t height,
                                PixelFormat format));
    MOCK_METHOD2(CreateVertexDeclaration,
                 VertexDeclID(VertexDeclElement* elements,
                              size_t num_elements));
    MOCK_METHOD2(CreateVertexShader, VertexShaderID(const void* shader_data,
                                                    size_t shader_size));
    MOCK_METHOD2(CreatePixelShader, PixelShaderID(const void* shader_data,
                                                  size_t shader_size));
    MOCK_METHOD1(CreateSamplerState, SamplerStateID(const SamplerState& state));
    MOCK_METHOD7(CreateTexture, TextureID(UsageType type,
                                          uint32_t width, uint32_t height,
                                          uint32_t mips, PixelFormat format,
                                          void* buffer, size_t buffer_size));
    MOCK_METHOD5(CreateVertexBuffer,
                 VertexBufferID(UsageType type, uint32_t stride,
                                uint32_t count, void* buffer,
                                size_t buffer_size));
    MOCK_METHOD4(CreateIndexBuffer,
                 IndexBufferID(UsageType type, uint32_t count,
                               void* buffer, size_t buffer_size));
    MOCK_METHOD4(CreateConstantBuffer,
                 ConstantBufferID(UsageType type, size_t size,
                                  void* buffer, size_t buffer_size));

    // Command List
    MOCK_METHOD0(BeginRecord, void());
    MOCK_METHOD0(EndRecord, CommandListID());

    // Modifiers
    MOCK_METHOD7(SetViewPort, void(ViewPortID viewport,
                                   uint32_t top, uint32_t left,
                                   uint32_t width, uint32_t height,
                                   float min_z, float max_z));
    MOCK_METHOD3(FillTexture, void(TextureID texture,
                                   void* buffer, size_t size));
    MOCK_METHOD4(FillTextureMip, void(TextureID texture, uint32_t mip,
                                      void* buffer, size_t size));
    MOCK_METHOD1(ResetVertexBuffer, void(VertexBufferID vertex_buffer));
    MOCK_METHOD5(AppendVertexBuffer,
                 void(VertexBufferID vertex_buffer, uint32_t count,
                      void* buffer, size_t buffer_size,
                      uint32_t* starting_offset));
    MOCK_METHOD5(FillVertexBuffer,
                 void(VertexBufferID vertex_buffer, uint32_t count,
                      void* buffer, size_t buffer_size, uint32_t index_offset));
    MOCK_METHOD1(ResetIndexBuffer, void(IndexBufferID index_buffer));
    MOCK_METHOD5(AppendIndexBuffer,
                 void(IndexBufferID index_buffer, uint32_t count,
                      void* buffer, size_t buffer_size,
                      uint32_t* starting_offset));
    MOCK_METHOD5(FillIndexBuffer,
                 void(IndexBufferID index_buffer, uint32_t count,
                      void* buffer, size_t buffer_size,
                      uint32_t index_offset));
    MOCK_METHOD3(FillConstantBuffer, void(ConstantBufferID constant_buffer,
                                          void* buffer, size_t size));

    // Accessors
    MOCK_METHOD7(GetViewPort, void(ViewPortID viewport,
                                   uint32_t& top, uint32_t& left,
                                   uint32_t& width, uint32_t& height,
                                   float& min_z, float& max_z));
    MOCK_METHOD1(GetVertexBufferCount, uint32_t(VertexBufferID vertex_buffer));
    MOCK_METHOD1(GetIndexBufferCount, uint32_t(IndexBufferID index_buffer));

    // Releases
    MOCK_METHOD1(ReleaseViewPort, void(ViewPortID viewport));
    MOCK_METHOD1(ReleaseRenderBlendState, void(RenderBlendStateID state));
    MOCK_METHOD1(ReleaseRenderTarget, void(RenderTargetID render_target));
    MOCK_METHOD1(ReleaseVertexDeclaration, void(VertexDeclID vertex_decl));
    MOCK_METHOD1(ReleaseVertexShader, void(VertexShaderID shader));
    MOCK_METHOD1(ReleasePixelShader, void(PixelShaderID shader));
    MOCK_METHOD1(ReleaseSamplerState, void(SamplerStateID state));
    MOCK_METHOD1(ReleaseTexture, void(TextureID texture));
    MOCK_METHOD1(ReleaseVertexBuffer, void(VertexBufferID vertex_buffer));
    MOCK_METHOD1(ReleaseIndexBuffer, void(IndexBufferID index_buffer));
    MOCK_METHOD1(ReleaseConstantBuffer, void(ConstantBufferID constant_buffer));
    MOCK_METHOD1(ReleaseCommandList, void(CommandListID command_list));

    // Activations
    MOCK_METHOD1(ActivateViewPort, void(ViewPortID viewport));
    MOCK_METHOD1(ActivateRenderBlendState, void(RenderBlendStateID blend_state));
    MOCK_METHOD2(ActivateRenderTarget,
                 void(int target, RenderTargetID render_target));
    MOCK_METHOD1(ActivateVertexDeclaration, void(VertexDeclID vertex_decl));
    MOCK_METHOD1(ActivateVertexShader, void(VertexShaderID shader));
    MOCK_METHOD1(ActivatePixelShader, void(PixelShaderID shader));
    MOCK_METHOD2(ActivateSamplerState, void(int sampler,
                                            SamplerStateID sampler_state));
    MOCK_METHOD2(ActivateTexture, void(int sampler, TextureID texture));
    MOCK_METHOD2(ActivateVertexStream, void(uint32_t stream,
                                            VertexBufferID vertex_buffer));
    MOCK_METHOD1(ActivateIndexStream, void(IndexBufferID index_buffer));
    MOCK_METHOD2(ActivateConstantBuffer,
                 void(int start_reg, ConstantBufferID constant_buffer));
    MOCK_METHOD1(ActivateDrawPrimitive, void(DrawPrimitive draw_primitive));

    // Draw
    MOCK_METHOD2(Draw, void(uint32_t start_vertex, uint32_t num_verts));
    MOCK_METHOD4(DrawInstanced,
                 void(uint32_t start_vertex, uint32_t verts_per_instance,
                      uint32_t start_instance, uint32_t num_instances));
    MOCK_METHOD3(DrawIndexed, void(uint32_t start_index, uint32_t num_indexes,
                                   uint32_t vertex_offset));
    MOCK_METHOD5(DrawIndexedInstanced,
                 void(uint32_t start_index, uint32_t index_per_instance,
                     uint32_t start_instance, uint32_t num_instances,
                     uint32_t vertex_offset));

    // Render
    MOCK_METHOD1(ExecuteCommandList, void(CommandListID commands));
    MOCK_METHOD0(Present, void());
  };

  MockRenderDevice* gMockRenderDevice = nullptr;
  YCommon::YPlatform::PlatformHandle gPlatformHandle;
  uint32_t gRenderWidth = 0;
  uint32_t gRenderHeight = 0;
} // namespace {

void RenderDevice::Initialize(const YCommon::YPlatform::PlatformHandle& handle,
                              uint32_t width, uint32_t height,
                              void* buffer, size_t buffer_size) {
  gPlatformHandle = handle;
  gRenderWidth = width;
  gRenderHeight = height;

  ASSERT_LE(sizeof(MockRenderDevice), buffer_size);
  gMockRenderDevice = new (buffer) MockRenderDevice();

  // Disallow all mock calls.
  EXPECT_CALL(*gMockRenderDevice, GetNullRenderTarget()).Times(0);
  EXPECT_CALL(*gMockRenderDevice, GetBackBufferRenderTarget()).Times(0);
  EXPECT_CALL(*gMockRenderDevice, GetRenderTargetTexture(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, CreateViewPort(_, _, _, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, CreateRenderBlendState(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, CreateRenderTarget(_, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, CreateVertexDeclaration(_,_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, CreateVertexShader(_, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, CreatePixelShader(_, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, CreateSamplerState(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, CreateTexture(_, _, _, _, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, CreateVertexBuffer(_, _, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, CreateIndexBuffer(_, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, CreateConstantBuffer(_, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, BeginRecord()).Times(0);
  EXPECT_CALL(*gMockRenderDevice, EndRecord()).Times(0);
  EXPECT_CALL(*gMockRenderDevice, SetViewPort(_, _, _, _, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, FillTexture(_, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, FillTextureMip(_, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ResetVertexBuffer(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, AppendVertexBuffer(_, _, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, FillVertexBuffer(_, _, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ResetIndexBuffer(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, AppendIndexBuffer(_, _, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, FillIndexBuffer(_, _, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, FillConstantBuffer(_, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, GetViewPort(_, _, _, _, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, GetVertexBufferCount(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, GetIndexBufferCount(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ReleaseViewPort(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ReleaseRenderBlendState(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ReleaseRenderTarget(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ReleaseVertexDeclaration(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ReleaseVertexShader(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ReleasePixelShader(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ReleaseSamplerState(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ReleaseTexture(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ReleaseVertexBuffer(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ReleaseIndexBuffer(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ReleaseConstantBuffer(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ReleaseCommandList(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ActivateViewPort(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ActivateRenderBlendState(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ActivateRenderTarget(_, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ActivateVertexDeclaration(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ActivateVertexShader(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ActivatePixelShader(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ActivateSamplerState(_, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ActivateTexture(_, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ActivateVertexStream(_, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ActivateIndexStream(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ActivateConstantBuffer(_, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ActivateDrawPrimitive(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, Draw(_, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, DrawInstanced(_, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, DrawIndexed(_, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, DrawIndexedInstanced(_, _, _, _, _)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, ExecuteCommandList(_)).Times(0);
  EXPECT_CALL(*gMockRenderDevice, Present()).Times(0);
}

void RenderDevice::Terminate() {
  gMockRenderDevice->~MockRenderDevice();
  gMockRenderDevice = nullptr;
  memset(&gPlatformHandle, 0, sizeof(gPlatformHandle));
  gRenderWidth = 0;
  gRenderHeight = 0;
}

RenderTargetID RenderDevice::GetNullRenderTarget() {
  return gMockRenderDevice->GetNullRenderTarget();
}

RenderTargetID RenderDevice::GetBackBufferRenderTarget() {
  return gMockRenderDevice->GetBackBufferRenderTarget();
}

TextureID RenderDevice::GetRenderTargetTexture(RenderTargetID render_target) {
  return gMockRenderDevice->GetRenderTargetTexture(render_target);
}

// Creates
ViewPortID RenderDevice::CreateViewPort(uint32_t top, uint32_t left,
                          uint32_t width, uint32_t height,
                          float min_z, float max_z) {
  return gMockRenderDevice->CreateViewPort(top, left,
                                           width, height,
                                           min_z, max_z);
}

RenderBlendStateID RenderDevice::CreateRenderBlendState(
    const RenderBlendState& state) {
  return gMockRenderDevice->CreateRenderBlendState(state);
}

RenderTargetID RenderDevice::CreateRenderTarget(uint32_t width, uint32_t height,
                                                PixelFormat format) {
  return gMockRenderDevice->CreateRenderTarget(width, height, format);
}

VertexDeclID RenderDevice::CreateVertexDeclaration(VertexDeclElement* elements,
                                                   size_t num_elements) {
  return gMockRenderDevice->CreateVertexDeclaration(elements, num_elements);
}

VertexShaderID RenderDevice::CreateVertexShader(const void* shader_data,
                                                size_t shader_size) {
  return gMockRenderDevice->CreateVertexShader(shader_data, shader_size);
}

PixelShaderID RenderDevice::CreatePixelShader(const void* shader_data,
                                              size_t shader_size) {
  return gMockRenderDevice->CreatePixelShader(shader_data, shader_size);
}

SamplerStateID RenderDevice::CreateSamplerState(const SamplerState& state) {
  return gMockRenderDevice->CreateSamplerState(state);
}

TextureID RenderDevice::CreateTexture(UsageType type,
                                      uint32_t width, uint32_t height,
                                      uint32_t mips, PixelFormat format,
                                      void* buffer, size_t buffer_size) {
  return gMockRenderDevice->CreateTexture(type, width, height,
                                          mips, format,
                                          buffer, buffer_size);
}

VertexBufferID RenderDevice::CreateVertexBuffer(UsageType type, uint32_t stride,
                                                uint32_t count, void* buffer,
                                                size_t buffer_size) {
  return gMockRenderDevice->CreateVertexBuffer(type, stride, count,
                                               buffer, buffer_size);
}

IndexBufferID RenderDevice::CreateIndexBuffer(UsageType type, uint32_t count,
                                              void* buffer,
                                              size_t buffer_size) {
  return gMockRenderDevice->CreateIndexBuffer(type, count, buffer, buffer_size);
}

ConstantBufferID RenderDevice::CreateConstantBuffer(UsageType type, size_t size,
                                                    void* buffer,
                                                    size_t buffer_size) {
  return gMockRenderDevice->CreateConstantBuffer(type, size,
                                                 buffer, buffer_size);
}

void RenderDevice::BeginRecord() {
  gMockRenderDevice->BeginRecord();
}

CommandListID RenderDevice::EndRecord() {
  return gMockRenderDevice->EndRecord();
}

void RenderDevice::SetViewPort(ViewPortID viewport,
                               uint32_t top, uint32_t left,
                               uint32_t width, uint32_t height,
                               float min_z, float max_z) {
  gMockRenderDevice->SetViewPort(viewport,
                                 top, left, width, height, min_z, max_z);
}

void RenderDevice::FillTexture(TextureID texture,
                               void* buffer, size_t size) {
  gMockRenderDevice->FillTexture(texture, buffer, size);
}

void RenderDevice::FillTextureMip(TextureID texture, uint32_t mip,
                                  void* buffer, size_t size) {
  gMockRenderDevice->FillTextureMip(texture, mip, buffer, size);
}

void RenderDevice::ResetVertexBuffer(VertexBufferID vertex_buffer) {
  gMockRenderDevice->ResetVertexBuffer(vertex_buffer);
}

void RenderDevice::AppendVertexBuffer(VertexBufferID vertex_buffer,
                                      uint32_t count,
                                      void* buffer, size_t buffer_size,
                                      uint32_t* starting_offset) {
  gMockRenderDevice->AppendVertexBuffer(vertex_buffer, count,
                                        buffer, buffer_size,
                                        starting_offset);
}

void RenderDevice::FillVertexBuffer(VertexBufferID vertex_buffer,
                                    uint32_t count,
                                    void* buffer, size_t buffer_size,
                                    uint32_t index_offset) {
  gMockRenderDevice->FillVertexBuffer(vertex_buffer, count,
                                      buffer, buffer_size,
                                      index_offset);
}

void RenderDevice::ResetIndexBuffer(IndexBufferID index_buffer) {
  gMockRenderDevice->ResetIndexBuffer(index_buffer);
}

void RenderDevice::AppendIndexBuffer(IndexBufferID index_buffer, uint32_t count,
                                     void* buffer, size_t buffer_size,
                                     uint32_t* starting_offset) {
  gMockRenderDevice->AppendIndexBuffer(index_buffer, count, buffer, buffer_size,
                                       starting_offset);
}

void RenderDevice::FillIndexBuffer(IndexBufferID index_buffer, uint32_t count,
                                   void* buffer, size_t buffer_size,
                                   uint32_t index_offset) {
  gMockRenderDevice->FillIndexBuffer(index_buffer, count,
                                     buffer, buffer_size, index_offset);
}

void RenderDevice::FillConstantBuffer(ConstantBufferID constant_buffer,
                                      void* buffer, size_t size) {
  gMockRenderDevice->FillConstantBuffer(constant_buffer, buffer, size);
}

// Accessors
void RenderDevice::GetFrameBufferDimensions(uint32_t& width, uint32_t& height) {
  width = gRenderWidth;
  height = gRenderHeight;
}

void RenderDevice::GetViewPort(ViewPortID viewport,
                               uint32_t& top, uint32_t& left,
                               uint32_t& width, uint32_t& height,
                               float& min_z, float& max_z) {
  gMockRenderDevice->GetViewPort(viewport, top, left, width, height,
                                 min_z, max_z);
}

uint32_t RenderDevice::GetVertexBufferCount(VertexBufferID vertex_buffer) {
  return gMockRenderDevice->GetVertexBufferCount(vertex_buffer);
}

uint32_t RenderDevice::GetIndexBufferCount(IndexBufferID index_buffer) {
  return gMockRenderDevice->GetIndexBufferCount(index_buffer);
}

void RenderDevice::ReleaseViewPort(ViewPortID viewport) {
  gMockRenderDevice->ReleaseViewPort(viewport);
}

void RenderDevice::ReleaseRenderBlendState(RenderBlendStateID state) {
  gMockRenderDevice->ReleaseRenderBlendState(state);
}

void RenderDevice::ReleaseRenderTarget(RenderTargetID render_target) {
  gMockRenderDevice->ReleaseRenderTarget(render_target);
}

void RenderDevice::ReleaseVertexDeclaration(VertexDeclID vertex_decl) {
  gMockRenderDevice->ReleaseVertexDeclaration(vertex_decl);
}

void RenderDevice::ReleaseVertexShader(VertexShaderID shader) {
  gMockRenderDevice->ReleaseVertexShader(shader);
}

void RenderDevice::ReleasePixelShader(PixelShaderID shader) {
  gMockRenderDevice->ReleasePixelShader(shader);
}

void RenderDevice::ReleaseSamplerState(SamplerStateID state) {
  gMockRenderDevice->ReleaseSamplerState(state);
}

void RenderDevice::ReleaseTexture(TextureID texture) {
  gMockRenderDevice->ReleaseTexture(texture);
}

void RenderDevice::ReleaseVertexBuffer(VertexBufferID vertex_buffer) {
  gMockRenderDevice->ReleaseVertexBuffer(vertex_buffer);
}

void RenderDevice::ReleaseIndexBuffer(IndexBufferID index_buffer) {
  gMockRenderDevice->ReleaseIndexBuffer(index_buffer);
}

void RenderDevice::ReleaseConstantBuffer(ConstantBufferID constant_buffer) {
  gMockRenderDevice->ReleaseConstantBuffer(constant_buffer);
}

void RenderDevice::ReleaseCommandList(CommandListID command_list) {
  gMockRenderDevice->ReleaseCommandList(command_list);
}

void RenderDevice::ActivateViewPort(ViewPortID viewport) {
  gMockRenderDevice->ActivateViewPort(viewport);
}

void RenderDevice::ActivateRenderBlendState(RenderBlendStateID blend_state) {
  gMockRenderDevice->ActivateRenderBlendState(blend_state);
}

void RenderDevice::ActivateRenderTarget(int target, RenderTargetID render_target) {
  gMockRenderDevice->ActivateRenderTarget(target, render_target);
}

void RenderDevice::ActivateVertexDeclaration(VertexDeclID vertex_decl) {
  gMockRenderDevice->ActivateVertexDeclaration(vertex_decl);
}

void RenderDevice::ActivateVertexShader(VertexShaderID shader) {
  gMockRenderDevice->ActivateVertexShader(shader);
}

void RenderDevice::ActivatePixelShader(PixelShaderID shader) {
  gMockRenderDevice->ActivatePixelShader(shader);
}

void RenderDevice::ActivateSamplerState(int sampler, SamplerStateID sampler_state) {
  gMockRenderDevice->ActivateSamplerState(sampler, sampler_state);
}

void RenderDevice::ActivateTexture(int sampler, TextureID texture) {
  gMockRenderDevice->ActivateTexture(sampler, texture);
}

void RenderDevice::ActivateVertexStream(uint32_t stream,
                                        VertexBufferID vertex_buffer) {
  gMockRenderDevice->ActivateVertexStream(stream, vertex_buffer);
}

void RenderDevice::ActivateIndexStream(IndexBufferID index_buffer) {
  gMockRenderDevice->ActivateIndexStream(index_buffer);
}

void RenderDevice::ActivateConstantBuffer(int start_reg,
                                          ConstantBufferID constant_buffer) {
  gMockRenderDevice->ActivateConstantBuffer(start_reg, constant_buffer);
}

void RenderDevice::ActivateDrawPrimitive(DrawPrimitive draw_primitive) {
  gMockRenderDevice->ActivateDrawPrimitive(draw_primitive);
}


// Draw
void RenderDevice::Draw(uint32_t start_vertex, uint32_t num_verts) {
  gMockRenderDevice->Draw(start_vertex, num_verts);
}

void RenderDevice::DrawInstanced(uint32_t start_vertex,
                                 uint32_t verts_per_instance,
                                 uint32_t start_instance,
                                 uint32_t num_instances) {
  gMockRenderDevice->DrawInstanced(start_vertex, verts_per_instance,
                                   start_instance, num_instances);
}

void RenderDevice::DrawIndexed(uint32_t start_index, uint32_t num_indexes,
                               uint32_t vertex_offset) {
  gMockRenderDevice->DrawIndexed(start_index, num_indexes, vertex_offset);
}

void RenderDevice::DrawIndexedInstanced(uint32_t start_index,
                                        uint32_t index_per_instance,
                                        uint32_t start_instance,
                                        uint32_t num_instances,
                                        uint32_t vertex_offset) {
  gMockRenderDevice->DrawIndexedInstanced(start_index, index_per_instance,
                                          start_instance, num_instances,
                                          vertex_offset);
}

void RenderDevice::ExecuteCommandList(CommandListID commands) {
  gMockRenderDevice->ExecuteCommandList(commands);
}

void RenderDevice::Present() {
  gMockRenderDevice->Present();
}

void RenderDeviceMock::ExpectGetNullRenderTarget(RenderTargetID ret) {
  EXPECT_CALL(*gMockRenderDevice, GetNullRenderTarget())
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectGetBackBufferRenderTarget(RenderTargetID ret) {
  EXPECT_CALL(*gMockRenderDevice, GetBackBufferRenderTarget())
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectGetRenderTargetTexture(
    TextureID ret, RenderTargetID render_target) {
  EXPECT_CALL(*gMockRenderDevice, GetRenderTargetTexture(render_target))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectCreateViewPort(ViewPortID ret,
                                            uint32_t top, uint32_t left,
                                            uint32_t width, uint32_t height,
                                            float min_z, float max_z) {
  EXPECT_CALL(*gMockRenderDevice, CreateViewPort(top, left, width, height,
                                                min_z, max_z))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectCreateRenderBlendState(
    RenderBlendStateID ret, const RenderBlendState& state) {
  EXPECT_CALL(*gMockRenderDevice, CreateRenderBlendState(state))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectCreateRenderTarget(RenderTargetID ret,
                                                uint32_t width, uint32_t height,
                                                PixelFormat format) {
  EXPECT_CALL(*gMockRenderDevice, CreateRenderTarget(width, height, format))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectCreateVertexDeclaration(
    VertexDeclID ret, VertexDeclElement* elements, size_t num_elements) {
  EXPECT_CALL(*gMockRenderDevice, CreateVertexDeclaration(elements,
                                                         num_elements))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectCreateVertexShader(VertexShaderID ret,
                                                const void* shader_data,
                                                size_t shader_size) {
  EXPECT_CALL(*gMockRenderDevice, CreateVertexShader(shader_data, shader_size))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectCreatePixelShader(PixelShaderID ret,
                                               const void* shader_data,
                                               size_t shader_size) {
  EXPECT_CALL(*gMockRenderDevice, CreatePixelShader(shader_data, shader_size))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectCreateSamplerState(SamplerStateID ret,
                                                const SamplerState& state) {
  EXPECT_CALL(*gMockRenderDevice, CreateSamplerState(state))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectCreateTexture(TextureID ret,
                                           UsageType type,
                                           uint32_t width, uint32_t height,
                                           uint32_t mips, PixelFormat format,
                                           void* buffer, size_t buffer_size) {
  EXPECT_CALL(*gMockRenderDevice, CreateTexture(type, width, height, mips,
                                               format, buffer, buffer_size))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectCreateVertexBuffer(VertexBufferID ret,
                                                UsageType type, uint32_t stride,
                                                uint32_t count, void* buffer,
                                                size_t buffer_size) {
  EXPECT_CALL(*gMockRenderDevice, CreateVertexBuffer(type, stride, count,
                                                    buffer, buffer_size))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectCreateIndexBuffer(IndexBufferID ret,
                                               UsageType type, uint32_t count,
                                               void* buffer, size_t buffer_size) {
  EXPECT_CALL(*gMockRenderDevice, CreateIndexBuffer(type, count,
                                                   buffer, buffer_size))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectCreateConstantBuffer(ConstantBufferID ret,
                                                  UsageType type, size_t size,
                                                  void* buffer,
                                                  size_t buffer_size) {
  EXPECT_CALL(*gMockRenderDevice, CreateConstantBuffer(type, size,
                                                      buffer, buffer_size))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectBeginRecord() {
  EXPECT_CALL(*gMockRenderDevice, BeginRecord())
      .Times(1);
}

void RenderDeviceMock::ExpectEndRecord(CommandListID ret) {
  EXPECT_CALL(*gMockRenderDevice, EndRecord())
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectSetViewPort(ViewPortID viewport,
                                         uint32_t top, uint32_t left,
                                         uint32_t width, uint32_t height,
                                         float min_z, float max_z) {
  EXPECT_CALL(*gMockRenderDevice, SetViewPort(viewport,
                                              top, left, width, height,
                                              min_z, max_z))
      .Times(1);
}

void RenderDeviceMock::ExpectFillTexture(TextureID texture,
                                         void* buffer, size_t size) {
  EXPECT_CALL(*gMockRenderDevice, FillTexture(texture, buffer, size))
      .Times(1);
}

void RenderDeviceMock::ExpectFillTextureMip(TextureID texture, uint32_t mip,
                                            void* buffer, size_t size) {
  EXPECT_CALL(*gMockRenderDevice, FillTextureMip(texture, mip, buffer, size))
      .Times(1);
}

void RenderDeviceMock::ExpectResetVertexBuffer(VertexBufferID vertex_buffer) {
  EXPECT_CALL(*gMockRenderDevice, ResetVertexBuffer(vertex_buffer))
      .Times(1);
}

void RenderDeviceMock::ExpectAppendVertexBuffer(VertexBufferID vertex_buffer,
                                                uint32_t count,
                                                void* buffer,
                                                size_t buffer_size,
                                                uint32_t* starting_offset) {
  EXPECT_CALL(*gMockRenderDevice, AppendVertexBuffer(vertex_buffer, count,
                                                    buffer, buffer_size,
                                                    starting_offset))
      .Times(1);
}

void RenderDeviceMock::ExpectFillVertexBuffer(VertexBufferID vertex_buffer,
                                              uint32_t count,
                                              void* buffer, size_t buffer_size,
                                              uint32_t index_offset) {
  EXPECT_CALL(*gMockRenderDevice, FillVertexBuffer(vertex_buffer, count,
                                                  buffer, buffer_size,
                                                  index_offset))
      .Times(1);
}

void RenderDeviceMock::ExpectResetIndexBuffer(IndexBufferID index_buffer) {
  EXPECT_CALL(*gMockRenderDevice, ResetIndexBuffer(index_buffer))
      .Times(1);
}

void RenderDeviceMock::ExpectAppendIndexBuffer(IndexBufferID index_buffer,
                                               uint32_t count,
                                               void* buffer, size_t buffer_size,
                                               uint32_t* starting_offset) {
  EXPECT_CALL(*gMockRenderDevice, AppendIndexBuffer(index_buffer, count,
                                                   buffer, buffer_size,
                                                   starting_offset))
      .Times(1);
}

void RenderDeviceMock::ExpectFillIndexBuffer(IndexBufferID index_buffer,
                                             uint32_t count,
                                             void* buffer, size_t buffer_size,
                                             uint32_t index_offset) {
  EXPECT_CALL(*gMockRenderDevice, FillIndexBuffer(index_buffer, count,
                                                 buffer, buffer_size,
                                                 index_offset))
      .Times(1);
}

void RenderDeviceMock::ExpectFillConstantBuffer(
    ConstantBufferID constant_buffer, void* buffer, size_t size) {
  EXPECT_CALL(*gMockRenderDevice, FillConstantBuffer(constant_buffer,
                                                    buffer, size))
      .Times(1);
}

void RenderDeviceMock::ExpectGetViewPort(ViewPortID viewport,
                                         uint32_t& top, uint32_t& left,
                                         uint32_t& width, uint32_t& height,
                                         float& min_z, float& max_z) {
  EXPECT_CALL(*gMockRenderDevice, GetViewPort(viewport, top, left,
                                             width, height, min_z, max_z))
      .Times(1);
}

void RenderDeviceMock::ExpectGetVertexBufferCount(
    uint32_t ret, VertexBufferID vertex_buffer) {
  EXPECT_CALL(*gMockRenderDevice, GetVertexBufferCount(vertex_buffer))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectGetIndexBufferCount(uint32_t ret,
                                                 IndexBufferID index_buffer) {
  EXPECT_CALL(*gMockRenderDevice, GetIndexBufferCount(index_buffer))
      .Times(1)
      .WillOnce(Return(ret));
}

void RenderDeviceMock::ExpectReleaseViewPort(ViewPortID viewport) {
  EXPECT_CALL(*gMockRenderDevice, ReleaseViewPort(viewport))
      .Times(1);
}

void RenderDeviceMock::ExpectReleaseRenderBlendState(RenderBlendStateID state) {
  EXPECT_CALL(*gMockRenderDevice, ReleaseRenderBlendState(state))
      .Times(1);
}

void RenderDeviceMock::ExpectReleaseRenderTarget(RenderTargetID render_target) {
  EXPECT_CALL(*gMockRenderDevice, ReleaseRenderTarget(render_target))
      .Times(1);
}

void RenderDeviceMock::ExpectReleaseVertexDeclaration(
    VertexDeclID vertex_decl) {
  EXPECT_CALL(*gMockRenderDevice, ReleaseVertexDeclaration(vertex_decl))
      .Times(1);
}

void RenderDeviceMock::ExpectReleaseVertexShader(VertexShaderID shader) {
  EXPECT_CALL(*gMockRenderDevice, ReleaseVertexShader(shader))
      .Times(1);
}

void RenderDeviceMock::ExpectReleasePixelShader(PixelShaderID shader) {
  EXPECT_CALL(*gMockRenderDevice, ReleasePixelShader(shader))
      .Times(1);
}

void RenderDeviceMock::ExpectReleaseSamplerState(SamplerStateID state) {
  EXPECT_CALL(*gMockRenderDevice, ReleaseSamplerState(state))
      .Times(1);
}

void RenderDeviceMock::ExpectReleaseTexture(TextureID texture) {
  EXPECT_CALL(*gMockRenderDevice, ReleaseTexture(texture))
      .Times(1);
}

void RenderDeviceMock::ExpectReleaseVertexBuffer(VertexBufferID vertex_buffer) {
  EXPECT_CALL(*gMockRenderDevice, ReleaseVertexBuffer(vertex_buffer))
      .Times(1);
}

void RenderDeviceMock::ExpectReleaseIndexBuffer(IndexBufferID index_buffer) {
  EXPECT_CALL(*gMockRenderDevice, ReleaseIndexBuffer(index_buffer))
      .Times(1);
}

void RenderDeviceMock::ExpectReleaseConstantBuffer(
    ConstantBufferID constant_buffer) {
  EXPECT_CALL(*gMockRenderDevice, ReleaseConstantBuffer(constant_buffer))
      .Times(1);
}

void RenderDeviceMock::ExpectReleaseCommandList(CommandListID command_list) {
  EXPECT_CALL(*gMockRenderDevice, ReleaseCommandList(command_list))
      .Times(1);
}

void RenderDeviceMock::ExpectActivateViewPort(ViewPortID viewport) {
  EXPECT_CALL(*gMockRenderDevice, ActivateViewPort(viewport))
      .Times(1);
}

void RenderDeviceMock::ExpectActivateRenderBlendState(
    RenderBlendStateID blend_state) {
  EXPECT_CALL(*gMockRenderDevice, ActivateRenderBlendState(blend_state))
      .Times(1);
}

void RenderDeviceMock::ExpectActivateRenderTarget(
    int target, RenderTargetID render_target) {
  EXPECT_CALL(*gMockRenderDevice, ActivateRenderTarget(target, render_target))
      .Times(1);
}

void RenderDeviceMock::ExpectActivateVertexDeclaration(
    VertexDeclID vertex_decl) {
  EXPECT_CALL(*gMockRenderDevice, ActivateVertexDeclaration(vertex_decl))
      .Times(1);
}

void RenderDeviceMock::ExpectActivateVertexShader(VertexShaderID shader) {
  EXPECT_CALL(*gMockRenderDevice, ActivateVertexShader(shader))
      .Times(1);
}

void RenderDeviceMock::ExpectActivatePixelShader(PixelShaderID shader) {
  EXPECT_CALL(*gMockRenderDevice, ActivatePixelShader(shader))
      .Times(1);
}

void RenderDeviceMock::ExpectActivateSamplerState(
    int sampler, SamplerStateID sampler_state) {
  EXPECT_CALL(*gMockRenderDevice, ActivateSamplerState(sampler, sampler_state))
      .Times(1);
}

void RenderDeviceMock::ExpectActivateTexture(int sampler, TextureID texture) {
  EXPECT_CALL(*gMockRenderDevice, ActivateTexture(sampler, texture))
      .Times(1);
}

void RenderDeviceMock::ExpectActivateVertexStream(
    uint32_t stream, VertexBufferID vertex_buffer) {
  EXPECT_CALL(*gMockRenderDevice, ActivateVertexStream(stream, vertex_buffer))
      .Times(1);
}

void RenderDeviceMock::ExpectActivateIndexStream(IndexBufferID index_buffer) {
  EXPECT_CALL(*gMockRenderDevice, ActivateIndexStream(index_buffer))
      .Times(1);
}

void RenderDeviceMock::ExpectActivateConstantBuffer(
    int start_reg, ConstantBufferID constant_buffer) {
  EXPECT_CALL(*gMockRenderDevice, ActivateConstantBuffer(start_reg,
                                                         constant_buffer))
      .Times(1);
}

void RenderDeviceMock::ExpectActivateDrawPrimitive(
    DrawPrimitive draw_primitive) {
  EXPECT_CALL(*gMockRenderDevice, ActivateDrawPrimitive(draw_primitive))
      .Times(1);
}

void RenderDeviceMock::ExpectDraw(uint32_t start_vertex, uint32_t num_verts) {
  EXPECT_CALL(*gMockRenderDevice, Draw(start_vertex, num_verts))
      .Times(1);
}

void RenderDeviceMock::ExpectDrawInstanced(uint32_t start_vertex,
                                           uint32_t verts_per_instance,
                                           uint32_t start_instance,
                                           uint32_t num_instances) {
  EXPECT_CALL(*gMockRenderDevice, DrawInstanced(start_vertex, verts_per_instance,
                                               start_instance, num_instances))
      .Times(1);
}

void RenderDeviceMock::ExpectDrawIndexed(uint32_t start_index, uint32_t num_indexes,
                                         uint32_t vertex_offset) {
  EXPECT_CALL(*gMockRenderDevice, DrawIndexed(start_index, num_indexes,
                                             vertex_offset))
      .Times(1);
}

void RenderDeviceMock::ExpectDrawIndexedInstanced(uint32_t start_index,
                                                  uint32_t index_per_instance,
                                                  uint32_t start_instance,
                                                  uint32_t num_instances,
                                                  uint32_t vertex_offset) {
  EXPECT_CALL(*gMockRenderDevice, DrawIndexedInstanced(start_index,
                                                      index_per_instance,
                                                      start_instance,
                                                      num_instances,
                                                      vertex_offset))
      .Times(1);
}

void RenderDeviceMock::ExpectExecuteCommandList(CommandListID commands) {
  EXPECT_CALL(*gMockRenderDevice, ExecuteCommandList(commands)).Times(1);
}

void RenderDeviceMock::ExpectPresent() {
  EXPECT_CALL(*gMockRenderDevice, Present()).Times(1);
}

}} // namespace YEngine { namespace YRenderDevice {
