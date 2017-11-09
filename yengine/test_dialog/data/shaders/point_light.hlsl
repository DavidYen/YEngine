/***************
* Point Light
****************/
float4x4 world_view_proj  : register(b0, space1);
float4x4 inverse_view_proj : register(b4, space1);
float3 camera_pos : register(b5, space1);

// Light Definition
float3 light_color : register(b6, space1);
float3 light_pos : register(b7, space1);
float light_radius : register (b8, space1);
float light_intensity : register(b9, space1);

// diffuse color, and specularIntensity in the alpha channel
Texture2D<float4> diffuse_map : register(t0, space1);
sampler diffuse_sampler : register(s0, space1);

// normals, and specularPower in the alpha channel
Texture2D<float4> normal_map : register(t1, space1);
sampler normal_sampler : register(s1, space1);

// depth
Texture2D<float> depth_map : register(t2, space1);
sampler depth_sampler : register(s2, space1);

struct VSOut {
  float4 Position : POSITION0;
  float4 ScreenPos : TEXCOORD0;
};

VSOut PointLightVS(float3 pos : POSITION) {
  VSOut output;

  output.Position = mul(float4(pos.xyz, 1.0), world_view_proj);
  output.ScreenPos = output.Position;

  return output;
}

float4 PointLightPS() : SV_TARGET {
  return float4(1.0, 1.0, 1.0, 1.0);
}
