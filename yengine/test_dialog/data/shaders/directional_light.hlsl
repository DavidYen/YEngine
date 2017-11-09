/*************
* Directional light
**************/
float4x4 inverse_view_proj : register(b0, space1);
float3 light_dir : register(b4, space1); // normalized light direction 
float3 light_color : register(b5, space1);
float3 camera_pos : register(b6, space1);
float4x4 light_view_proj : register (b7, space1);

Texture2D<float4> diffuse_map : register(t0, space1);
sampler diffuse_sampler : register(s0, space1);

Texture2D<float4> normal_map : register(t1, space1);
sampler normal_sampler : register(s1, space1);

Texture2D<float> depth_map : register(t2, space1);
sampler depth_sampler : register(s2, space1);

Texture2D<float> shadow_map : register(t3, space1);
sampler shadow_sampler : register(s3, space1);

struct VSIn {
  float3 Position : POSITION0;
  float2 TexCoord : TEXCOORD0;
};

struct VSOut {
  float4 Position : POSITION0;
  float2 TexCoord : TEXCOORD0;
};

struct PSIn {
  float2 TexCoord : TEXCOORD0;
};

VSOut DirectionalLightVS(VSIn In) {
  VSOut Out;

  Out.Position = float4( In.Position, 1.0f );
  Out.TexCoord = In.TexCoord;

  return Out;
}

float4 DirectionalLightPS(PSIn In) : SV_TARGET {
  float4 diffuse_data = diffuse_map.Sample(diffuse_sampler, In.TexCoord);
  float4 normal_data = normal_map.Sample(normal_sampler, In.TexCoord);
  float4 depth_data = depth_map.Sample(depth_sampler, In.TexCoord);

  float specular_intensity = diffuse_data.a;
  float specular_power = normal_data.a;
  float depth = depth_data.r;

  // Get the position from pixel position and depth map
  float4 position;
  position.x = In.TexCoord.x * 2.0f - 1.0f; // transform from [0, 1] -> [-1, 1] 
  position.y = -(In.TexCoord.y * 2.0f - 1.0f); // transform from [0, 1] -> [1, -1]
  position.z = depth;
  position.w = 1.0f;

  // Convert to world position
  position = mul(position, inverse_view_proj);
  position /= position.w;

  // See if this is in a shadow
  float4 light_view_pos = mul(position, light_view_proj);
  light_view_pos.y = -light_view_pos.y;
  float2 shadow_map_uv = (light_view_pos / 2.0f + 0.5f).xy;
  float shadow_map_depth = shadow_map.Sample(shadow_sampler, shadow_map_uv).r;
  if (light_view_pos.z > shadow_map_depth + 0.005f) {
    discard;
  }

  // Get the normal for the pixel position
  float3 normal = 2.0f * normal_data.xyz - 1.0f; // transfrom from [0,1] -> [-1, 1]

  // Calculate Diffuse part
  float3 diffuse = light_color * max(0, dot(-light_dir, normal));

  // Calculate Specular part
  float3 reflection = normalize(reflect(light_dir, normal));
  float3 camDir = normalize(camera_pos - position.xyz);
  float specular = specular_intensity * pow(max(0, dot(reflection, camDir)),
                                            specular_power);

  return float4(diffuse, specular);
}
