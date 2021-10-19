#pragma once

#include "presets.hpp"

namespace aja {
static const RoutingPresetMap kHDMIDisplayYCbCrPresets = {
	{"HDMI_HD_YCBCR_LFR_YCbCr_Display",
	 {kConnectionKindHDMI,
	  NTV2_MODE_DISPLAY,
	  1,
	  1,
	  0,
	  false,
	  "fb[{ch1}][0]->hdmi[0][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::HD_YCBCR_LFR,
	  SDIWireFormat::Unknown,
	  VPIDStandard_Unknown}},
	{"HDMI_HD_YCBCR_LFR_YCbCr_Display (TTap Pro)",
	 {kConnectionKindHDMI,
	  NTV2_MODE_DISPLAY,
	  1,
	  1,
	  0,
	  false,
	  "fb[{ch1}][0]->sdi[{ch1}][0];"
	  "fb[{ch1}][0]->hdmi[{ch1}][0];",
	  {DEVICE_ID_TTAP_PRO},
	  RasterDefinition::HD,
	  HDMIWireFormat::HD_YCBCR_LFR,
	  SDIWireFormat::Unknown,
	  VPIDStandard_Unknown}},
};
} // aja
