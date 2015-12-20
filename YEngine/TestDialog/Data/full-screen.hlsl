
/**************
* Clear functions
***************/
float4 ClearGBufferVertexShader(float3 pos : POSITION) : POSITION {
  return float4(pos, 1.0);
}

struct ClearGBufferPSOut {
  float4 Color    : COLOR0; // Also contains specular hightlight
  float4 Normal   : COLOR1; // Also contains specular power
  float4 Depth    : COLOR2;
  float4 LightMap : COLOR3;
};

ClearGBufferPSOut ClearGBufferPixelShader() {
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
float2 halfPixel : packoffset(c0);

float ambientLight : packoffset(c0.z); // between 0~1
float3 ambientColor : packoffset(c1);

texture diffuseMap;
sampler diffuseSampler = sampler_state {
  Texture = <diffuseMap>;
  MAGFILTER = POINT;
  MINFILTER = POINT;
  MIPFILTER = POINT;
  AddressU = Clamp;
  AddressV = Clamp;
};

texture lightMap;
sampler lightSampler = sampler_state {
  Texture = <lightMap>;
  MAGFILTER = POINT;
  MINFILTER = POINT;
  MIPFILTER = POINT;
  AddressU = Clamp;
  AddressV = Clamp;
};

struct VSIn {
  float3 Position : POSITION0;
  float2 TexCoord : TEXCOORD0;
};

struct VSOut {
  float4 Position : POSITION0;
  float2 TexCoord : TEXCOORD0;
};

VSOut PresentSceneVS(VSIn In) {
  VSOut Out;

  Out.Position = float4(In.Position, 1.0f);
  Out.TexCoord = In.TexCoord - halfPixel;

  return Out;
}

struct PSIn {
  float2 TexCoord : TEXCOORD0;
};

float4 PresentScenePS(PSIn In) : COLOR {
  float3 diffuseData = tex2D(diffuseSampler, In.TexCoord).rgb;
  float4 lightData = tex2D(lightSampler, In.TexCoord);
  lightData += float4(ambientColor, 0.0f) * ambientLight;

  return float4(diffuseData.rgb * lightData.rgb + lightData.a, 1.0f);
}
