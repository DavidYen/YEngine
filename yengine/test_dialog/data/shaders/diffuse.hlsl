/****************
* Diffuse Shader
*****************/

// Texture Map Effect
// This shader just draws an object with a texture
float4x4 world : register(b0, space1);
float4x4 view_proj : register(b1, space1);
float2 specular_intensity_power : register(b2, space1) = { 0.1f, 0.5f };

Texture2D<float4> texture_map : register(t0, space1);
sampler texture_sampler : register(s0, space1);

Texture2D<float4> normal_map : register(t1, space1);
sampler normal_sampler : register(s1, space1);

struct VSIn {
  float4 Position : POSITION0;
  float3 Normal : NORMAL0;
  float3 Tangent  : TANGENT0;
  float2 TexCoord : TEXCOORD0;
};

/************************************
* DIFFUSE PASS
*************************************/
struct DiffuseVSOut {
  float4 Position : POSITION;
  float2 TexCoord : TEXCOORD0;
  float3 Normal   : TEXCOORD1;
  float3 Tangent  : TEXCOORD2;
  float2 Depth  : TEXCOORD3; // Z and W components to calculate depth
};

struct DiffusePSOut {
  float4 Color : SV_TARGET0;
  float4 Normal : SV_TARGET1;
  float4 Depth : SV_TARGET2; // also contains specular power and highlight
};

DiffuseVSOut DiffuseVertexShader(VSIn input) {
  float4 world_position = mul(input.Position, world);

  DiffuseVSOut Out;
  Out.Position = mul(world_position, view_proj);
  Out.TexCoord = input.TexCoord;
  Out.Normal = mul(float4(input.Normal, 1.0), world).xyz;
  Out.Tangent = mul(float4(input.Tangent, 1.0), world).xyz;
  Out.Depth.x = Out.Position.z;
  Out.Depth.y = Out.Position.w;

  return Out;
}

DiffusePSOut DiffusePixelShader(DiffuseVSOut input) {
  // Should already be in normal coordinates and normalized
  float3 normal = input.Normal;

  // Should already be in world coordinates and normalized
  float3 tangent = input.Tangent;
  float3 binormal = cross(normal, tangent);
  float3x3 NTBMatrix = float3x3(tangent.x, binormal.x, normal.x,
                                tangent.y, binormal.y, normal.y,
                                tangent.z, binormal.z, normal.z);

  float3 sampledNormal = normalize(2.0f *
                                   normal_map.Sample(normal_sampler,
                                                     input.TexCoord).rgb -
                                   1.0f);

  DiffusePSOut Out;
  Out.Color.rgb = texture_map.Sample(texture_sampler, input.TexCoord).rgb;
  Out.Color.a = specular_intensity_power.x;

  Out.Normal.rgb = 0.5f * (mul(sampledNormal, NTBMatrix) + 1.0f); // Transform normal from -1~1 to 0~1.0f
  Out.Normal.a = specular_intensity_power.y;

  Out.Depth = input.Depth.x / input.Depth.y;

  return Out;
}

/*******************************************
* Depth Only pass for Shadow Maps
********************************************/
struct DepthVSOut {
  float4 Position : POSITION;
  float2 Depth : TEXCOORD3; // Z and W components to calculate depth
};

struct DepthPSOut {
  float4 Depth : SV_TARGET; // also contains specular power and highlight
};

DepthVSOut DepthVertexShader(VSIn input) {
  float4 world_position = mul(input.Position, world);

  DepthVSOut Out;
  Out.Position = mul(world_position, view_proj);
  Out.Depth.x = Out.Position.z;
  Out.Depth.y = Out.Position.w;

  return Out;
}

DepthPSOut DepthPixelShader(DepthVSOut input)
{
  DepthPSOut Out;
  Out.Depth = input.Depth.x / input.Depth.y;

  return Out;
}
