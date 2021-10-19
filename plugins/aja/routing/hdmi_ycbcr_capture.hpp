#pragma once

#include "presets.hpp"

namespace aja {
static const RoutingPresetMap kHDMICaptureYCbCrPresets = {
	{"HDMI_HD_YCBCR_LFR_YCbCr_Capture",
	 {kConnectionKindHDMI,
	  NTV2_MODE_CAPTURE,
	  1,
	  1,
	  0,
	  false,
	  "hdmi[{ch1}][0]->fb[{ch1}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::HD_YCBCR_LFR,
	  SDIWireFormat::Unknown,
	  VPIDStandard_Unknown}},
};
} // aja
