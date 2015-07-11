#ifndef YENGINE_YRENDERER_RENDERCOMMANDS_H
#define YENGINE_YRENDERER_RENDERCOMMANDS_H

#include <YEngine/YRenderDevice/RenderDevice.h>
#include "Renderer.h"

namespace YEngine { namespace YRenderer {

class RenderCommands {
 public:
  RenderCommands();
  ~RenderCommands();

  struct RenderKeyFieldDescription {
    uint8_t field_bits;
    enum RenderKeyFieldType {
      kRenderKeyFieldType_RenderPass,
      kRenderKeyFieldType_ViewPort,
      kRenderKeyFieldType_VertexDecl,
      kRenderKeyFieldType_Shader,
      kRenderKeyFieldType_StreamSource,
      kRenderKeyFieldType_Depth,
      kRenderKeyFieldType_ArbitraryNumber,
    } field_type;
  };

  // Setup render key fields, must be called while command buffer is empty.
  void SetupRenderKey(const RenderKeyFieldDescription* fields,
                      size_t num_fields);

  void AddCommand(uint8_t render_pass, ViewPortID viewport, 

 private:
  
};

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_RENDERCOMMANDS_H
