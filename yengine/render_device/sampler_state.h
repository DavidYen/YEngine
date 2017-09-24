#ifndef YENGINE_RENDER_DEVICE_SAMPLER_STATE_H
#define YENGINE_RENDER_DEVICE_SAMPLER_STATE_H

namespace yengine { namespace render_device {

enum SamplerFilter {
  kSamplerFilter_MinMagMipPoint,
  kSamplerFilter_MinPoint_MagMipLinear,
  kSamplerFilter_MinLinear_MagMipPoint,
  kSamplerFilter_MinLinear_MagPoint_MipLinear,
  kSamplerFilter_MinMagLinear_MipPoint,
  kSamplerFilter_MinMagMipLinear,
  kSamplerFilter_Anisotropic,

  NUM_SAMPLER_FILTERS
};

// Address mode to use when sampling outside of 0~1
enum SamplerAddressMode {
  kSamplerAddressMode_Wrap,
  kSamplerAddressMode_Mirror,
  kSamplerAddressMode_Clamp,

  NUM_SAMPLER_ADDRESS_MODES
};

struct SamplerState {
  SamplerFilter filter;
  SamplerAddressMode address_mode_u;
  SamplerAddressMode address_mode_v;
  SamplerAddressMode address_mode_w;

  SamplerState()
    : filter(kSamplerFilter_MinMagMipPoint),
      address_mode_u(kSamplerAddressMode_Clamp),
      address_mode_v(kSamplerAddressMode_Clamp),
      address_mode_w(kSamplerAddressMode_Clamp) {
  }

  bool operator==(const SamplerState& other) const {
    return (filter == other.filter &&
            address_mode_u == other.address_mode_u &&
            address_mode_v == other.address_mode_v &&
            address_mode_w == address_mode_w);
  }
};

}} // namespace yengine { namespace render_device {

#endif // YENGINE_RENDER_DEVICE_SAMPLER_STATE_H
