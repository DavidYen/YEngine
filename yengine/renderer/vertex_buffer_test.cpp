#include "yengine/renderer/vertex_buffer.h"

#include <gtest/gtest.h>

#include "ycommon/headers/test_helpers.h"
#include "yengine/render_device/render_device_mock.h"
#include "yengine/renderer/basic_renderer_test.h"
#include "yengine/renderer/render_device_state.h"

namespace yengine { namespace renderer {

class VertexDeclTest : public BasicRendererTest {};
class IndexBufferTest : public BasicRendererTest {};
class VertexBufferTest : public BasicRendererTest {};

TEST_F(VertexDeclTest, VertexDeclEmptyRelease) {
  const render_device::VertexDeclElement kElements[] = {
    { 0, 1, 1, render_device::kVertexElementType_Float,
      render_device::kVertexElementUsage_Position },
  };
  VertexDecl vertex_decl(kElements, ARRAY_SIZE(kElements));
  vertex_decl.Release();
}

TEST_F(VertexDeclTest, VertexDeclActivationTest) {
  const render_device::VertexDeclElement kElements[] = {
    { 0, 1, 1, render_device::kVertexElementType_Float,
      render_device::kVertexElementUsage_Position },
  };
  VertexDecl vertex_decl(kElements, ARRAY_SIZE(kElements));
  RenderDeviceState device_state;

  const render_device::VertexDeclID kVertexDeclID = 123;
  render_device::RenderDeviceMock::ExpectCreateVertexDeclaration(
      kVertexDeclID,
      vertex_decl.GetVertexDeclElements(),
      vertex_decl.GetNumVertexElements());
  render_device::RenderDeviceMock::ExpectActivateVertexDeclaration(
      kVertexDeclID);
  device_state.Reset();
  vertex_decl.Activate(device_state);

  // Second activation should not create the declaration again
  render_device::RenderDeviceMock::ExpectActivateVertexDeclaration(
      kVertexDeclID);
  device_state.Reset();
  vertex_decl.Activate(device_state);

  render_device::RenderDeviceMock::ExpectReleaseVertexDeclaration(
      kVertexDeclID);
  vertex_decl.Release();
}

TEST_F(IndexBufferTest, IndexBufferEmptyRelease) {
  IndexBuffer index_buffer;
  index_buffer.Release();
}

TEST_FAILURE_F(IndexBufferTest, FillNoInitializeFails, "not been initialized") {
  IndexBuffer index_buffer;

  const render_device::IndexBufferID kIndexBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateIndexBuffer(
      kIndexBufferID,
      render_device::kUsageType_Invalid,
      0);
  render_device::RenderDeviceMock::ExpectFillIndexBuffer(
      kIndexBufferID,
      0,
      nullptr,
      0);
  index_buffer.Fill(nullptr, 0);
}

TEST_F(IndexBufferTest, BasicFillTest) {
  IndexBuffer index_buffer;
  uint16_t kIndexData[10] = { 0 };
  index_buffer.Initialize(render_device::kUsageType_Static,
                          ARRAY_SIZE(kIndexData));

  const render_device::IndexBufferID kIndexBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateIndexBuffer(
      kIndexBufferID,
      render_device::kUsageType_Static,
      ARRAY_SIZE(kIndexData));
  render_device::RenderDeviceMock::ExpectFillIndexBuffer(
      kIndexBufferID,
      ARRAY_SIZE(kIndexData),
      kIndexData,
      sizeof(kIndexData));
  index_buffer.Fill(kIndexData, ARRAY_SIZE(kIndexData));
}

TEST_F(IndexBufferTest, FillMultiTest) {
  uint16_t kIndexData1[4] = { 0 };
  uint16_t kIndexData2[8] = { 0 };
  uint16_t* kIndexDatas[2] = { kIndexData1, kIndexData2 };
  uint32_t kIndexDataSizes[2] = { ARRAY_SIZE(kIndexData1),
                                  ARRAY_SIZE(kIndexData2) };
  uint16_t kIndexDataOffsets[2] = { 0, 0 };
  uint32_t kTotalSize = ARRAY_SIZE(kIndexData1) + ARRAY_SIZE(kIndexData2);

  IndexBuffer index_buffer;
  index_buffer.Initialize(render_device::kUsageType_Static, kTotalSize);

  const render_device::IndexBufferID kIndexBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateIndexBuffer(
      kIndexBufferID,
      render_device::kUsageType_Static,
      kTotalSize);
  render_device::RenderDeviceMock::ExpectFillIndexBuffer(
      kIndexBufferID,
      ARRAY_SIZE(kIndexData1),
      kIndexData1,
      sizeof(kIndexData1),
      0);
  render_device::RenderDeviceMock::ExpectFillIndexBuffer(
      kIndexBufferID,
      ARRAY_SIZE(kIndexData2),
      kIndexData2,
      sizeof(kIndexData2),
      0,
      ARRAY_SIZE(kIndexData1));
  index_buffer.FillMulti(ARRAY_SIZE(kIndexDatas),
                         kIndexDatas,
                         kIndexDataSizes,
                         kIndexDataOffsets);
}

TEST_FAILURE_F(IndexBufferTest, FillMaxSizeFails, "exceeded") {
  uint16_t kIndexData1[4] = { 0 };
  uint16_t kIndexData2[8] = { 0 };
  uint16_t* kIndexDatas[2] = { kIndexData1, kIndexData2 };
  uint32_t kIndexDataSizes[2] = { ARRAY_SIZE(kIndexData1),
                                  ARRAY_SIZE(kIndexData2) };
  uint16_t kIndexDataOffsets[2] = { 0, 0 };
  uint32_t kTotalSize = ARRAY_SIZE(kIndexData1) + ARRAY_SIZE(kIndexData2);

  IndexBuffer index_buffer;
  index_buffer.Initialize(render_device::kUsageType_Static, kTotalSize-1);

  const render_device::IndexBufferID kIndexBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateIndexBuffer(
      kIndexBufferID,
      render_device::kUsageType_Static,
      kTotalSize-1);
  render_device::RenderDeviceMock::ExpectFillIndexBuffer(
      kIndexBufferID,
      ARRAY_SIZE(kIndexData1),
      kIndexData1,
      sizeof(kIndexData1),
      0);
  render_device::RenderDeviceMock::ExpectFillIndexBuffer(
      kIndexBufferID,
      ARRAY_SIZE(kIndexData2),
      kIndexData2,
      sizeof(kIndexData2),
      0,
      ARRAY_SIZE(kIndexData1));
  index_buffer.FillMulti(ARRAY_SIZE(kIndexDatas),
                         kIndexDatas,
                         kIndexDataSizes,
                         kIndexDataOffsets);
}

TEST_FAILURE_F(IndexBufferTest, ActivateNoFillFails, "not been filled") {
  IndexBuffer index_buffer;
  RenderDeviceState device_state;

  index_buffer.Activate(device_state);
}

TEST_F(IndexBufferTest, ActivationTest) {
  IndexBuffer index_buffer;
  uint16_t kIndexData[10] = { 0 };
  index_buffer.Initialize(render_device::kUsageType_Static,
                          ARRAY_SIZE(kIndexData));

  const render_device::IndexBufferID kIndexBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateIndexBuffer(
      kIndexBufferID,
      render_device::kUsageType_Static,
      ARRAY_SIZE(kIndexData));
  render_device::RenderDeviceMock::ExpectFillIndexBuffer(
      kIndexBufferID,
      ARRAY_SIZE(kIndexData),
      kIndexData,
      sizeof(kIndexData));
  index_buffer.Fill(kIndexData, ARRAY_SIZE(kIndexData));

  render_device::RenderDeviceMock::ExpectActivateIndexStream(kIndexBufferID);
  RenderDeviceState device_state;
  index_buffer.Activate(device_state);
}

TEST_F(VertexBufferTest, VertexBufferEmptyRelease) {
  VertexBuffer vertex_buffer;
  vertex_buffer.Release();
}

TEST_FAILURE_F(VertexBufferTest, FillNoInitializeFails,
               "not been initialized") {
  VertexBuffer vertex_buffer;

  const render_device::VertexBufferID kVertexBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateVertexBuffer(
      kVertexBufferID,
      render_device::kUsageType_Invalid,
      1,
      0);
  render_device::RenderDeviceMock::ExpectFillVertexBuffer(
      kVertexBufferID,
      0,
      nullptr,
      0);
  vertex_buffer.Fill(nullptr, 0);
}

TEST_F(VertexBufferTest, BasicFillTest) {
  VertexBuffer vertex_buffer;
  float kVertexData[10] = { 0.0f };
  static_cast<void>(kVertexData);
  vertex_buffer.Initialize(render_device::kUsageType_Static,
                           sizeof(kVertexData[0]),
                           ARRAY_SIZE(kVertexData));

  const render_device::VertexBufferID kVertexBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateVertexBuffer(
      kVertexBufferID,
      render_device::kUsageType_Static,
      sizeof(kVertexData[0]),
      ARRAY_SIZE(kVertexData));
  render_device::RenderDeviceMock::ExpectFillVertexBuffer(
      kVertexBufferID,
      ARRAY_SIZE(kVertexData),
      kVertexData,
      sizeof(kVertexData));
  vertex_buffer.Fill(kVertexData, sizeof(kVertexData));
}

TEST_F(VertexBufferTest, FillMultiTest) {
  float kVertexData1[4] = { 0 };
  float kVertexData2[8] = { 0 };
  float* kVertexDatas[2] = { kVertexData1, kVertexData2 };
  uint32_t kVertexDataSizes[2] = { ARRAY_SIZE(kVertexData1),
                                   ARRAY_SIZE(kVertexData2) };
  uint32_t kTotalSize = ARRAY_SIZE(kVertexData1) + ARRAY_SIZE(kVertexData2);

  VertexBuffer vertex_buffer;
  vertex_buffer.Initialize(render_device::kUsageType_Static,
                           sizeof(kVertexData1[0]),
                           kTotalSize);

  const render_device::VertexBufferID kVertexBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateVertexBuffer(
      kVertexBufferID,
      render_device::kUsageType_Static,
      sizeof(kVertexData1[0]),
      kTotalSize);
  render_device::RenderDeviceMock::ExpectFillVertexBuffer(
      kVertexBufferID,
      ARRAY_SIZE(kVertexData1),
      kVertexData1,
      sizeof(kVertexData1),
      0);
  render_device::RenderDeviceMock::ExpectFillVertexBuffer(
      kVertexBufferID,
      ARRAY_SIZE(kVertexData2),
      kVertexData2,
      sizeof(kVertexData2),
      ARRAY_SIZE(kVertexData1));
  vertex_buffer.FillMulti(ARRAY_SIZE(kVertexDatas),
                          kVertexDatas,
                          kVertexDataSizes);
}

TEST_FAILURE_F(VertexBufferTest, FillMaxSizeFails, "exceeded") {
  float kVertexData1[4] = { 0 };
  float kVertexData2[8] = { 0 };
  float* kVertexDatas[2] = { kVertexData1, kVertexData2 };
  uint32_t kVertexDataSizes[2] = { ARRAY_SIZE(kVertexData1),
                                   ARRAY_SIZE(kVertexData2) };
  uint32_t kTotalSize = ARRAY_SIZE(kVertexData1) + ARRAY_SIZE(kVertexData2);

  VertexBuffer vertex_buffer;
  vertex_buffer.Initialize(render_device::kUsageType_Static,
                           sizeof(kVertexData1[0]),
                           kTotalSize - 1);

  const render_device::VertexBufferID kVertexBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateVertexBuffer(
      kVertexBufferID,
      render_device::kUsageType_Static,
      sizeof(kVertexData1[0]),
      kTotalSize - 1);
  render_device::RenderDeviceMock::ExpectFillVertexBuffer(
      kVertexBufferID,
      ARRAY_SIZE(kVertexData1),
      kVertexData1,
      sizeof(kVertexData1),
      0);
  render_device::RenderDeviceMock::ExpectFillVertexBuffer(
      kVertexBufferID,
      ARRAY_SIZE(kVertexData2),
      kVertexData2,
      sizeof(kVertexData2),
      ARRAY_SIZE(kVertexData1));
  vertex_buffer.FillMulti(ARRAY_SIZE(kVertexDatas),
                          kVertexDatas,
                          kVertexDataSizes);
}

TEST_FAILURE_F(VertexBufferTest, FillWrongStrideFails,
               "not a multiple of the stride") {
  float kVertexData1[4] = { 0 };
  float kVertexData2[3] = { 0 };
  float* kVertexDatas[2] = { kVertexData1, kVertexData2 };
  uint32_t kVertexDataSizes[2] = { ARRAY_SIZE(kVertexData1),
                                   ARRAY_SIZE(kVertexData2) };

  VertexBuffer vertex_buffer;
  vertex_buffer.Initialize(render_device::kUsageType_Static,
                           sizeof(kVertexData1),
                           2);

  const render_device::VertexBufferID kVertexBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateVertexBuffer(
      kVertexBufferID,
      render_device::kUsageType_Static,
      sizeof(kVertexData1),
      2);
  render_device::RenderDeviceMock::ExpectFillVertexBuffer(
      kVertexBufferID,
      1,
      kVertexData1,
      sizeof(kVertexData1),
      0);
  render_device::RenderDeviceMock::ExpectFillVertexBuffer(
      kVertexBufferID,
      0,
      kVertexData2,
      sizeof(kVertexData2),
      1);
  vertex_buffer.FillMulti(ARRAY_SIZE(kVertexDatas),
                          kVertexDatas,
                          kVertexDataSizes);
}

TEST_FAILURE_F(VertexBufferTest, ActivateNoFillFails, "not been filled") {
  VertexBuffer vertex_buffer;
  RenderDeviceState device_state;

  vertex_buffer.Activate(device_state);
}

TEST_F(VertexBufferTest, ActivationTest) {
  VertexBuffer vertex_buffer;
  float kVertexData[10] = { 0.0f };
  static_cast<void>(kVertexData);
  vertex_buffer.Initialize(render_device::kUsageType_Static,
                           sizeof(kVertexData[0]),
                           ARRAY_SIZE(kVertexData));

  const render_device::VertexBufferID kVertexBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateVertexBuffer(
      kVertexBufferID,
      render_device::kUsageType_Static,
      sizeof(kVertexData[0]),
      ARRAY_SIZE(kVertexData));
  render_device::RenderDeviceMock::ExpectFillVertexBuffer(
      kVertexBufferID,
      ARRAY_SIZE(kVertexData),
      kVertexData,
      sizeof(kVertexData));
  vertex_buffer.Fill(kVertexData, sizeof(kVertexData));

  render_device::RenderDeviceMock::ExpectActivateVertexStream(0,
                                                              kVertexBufferID);
  RenderDeviceState device_state;
  vertex_buffer.Activate(device_state);
}

}} // namespace yengine { namespace renderer {
