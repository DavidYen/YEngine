
/**************
* Clear functions
***************/
float4 ClearGBufferVertexShader(float4 pos : POSITION) : SV_POSITION {
  return pos;
}

struct ClearGBufferPSOut {
  float4 Color    : SV_TARGET0; // Also contains specular hightlight
  float4 Normal   : SV_TARGET1; // Also contains specular power
  float4 Depth    : SV_TARGET2;
  float4 LightMap : SV_TARGET3;
};

ClearGBufferPSOut ClearGBufferPixelShader(float4 input : TEXCOORD) {
  ClearGBufferPSOut output;

  // Render Target 0
  output.Color = 0.0f; // RGB Color and specular Highlight

  // Render Target 1
  output.Normal.rgb = 0.5f; // Normal ranges from [-1,1]. So 0.5 -> 0
  output.Normal.a = 0.0f; // Specular Power

  // Render Target 2
  output.Depth = 1.0f; // Depth (Initialize to Max)
  output.LightMap = 0.0f;

  return output;
}

/***************
* Combines ambient, diffuse, and specular data and displays the scene
****************/
struct AmbientData {
  float3 color;
  float light;
};

ConstantBuffer<AmbientData> ambient_data : register(b0, space1);
Texture2D<float4> diffuse_map : register(t0, space1);
sampler diffuse_sampler : register(s0, space1);

Texture2D<float4> light_map : register(t1, space1);
sampler light_sampler : register(s1, space1);

struct VSIn {
  float3 Position : POSITION0;
  float2 TexCoord : TEXCOORD0;
};

struct VSOut {
  float4 Position : SV_POSITION;
  float2 TexCoord : TEXCOORD0;
};

VSOut PresentSceneVS(VSIn In) {
  VSOut Out;

  Out.Position = float4(In.Position, 1.0f);
  Out.TexCoord = In.TexCoord;

  return Out;
}

struct PSIn {
  float2 TexCoord : TEXCOORD0;
};

float4 PresentScenePS(PSIn In) : SV_TARGET {
  float4 diffuseData = diffuse_map.Sample(diffuse_sampler, In.TexCoord);
  float4 lightData = light_map.Sample(light_sampler, In.TexCoord);
  lightData += float4(ambient_data.color, 0.0f) * ambient_data.light;

  return float4(diffuseData.rgb * lightData.rgb + lightData.a, 1.0f);
}
