#pragma once

#include "presets.hpp"

namespace aja {
static const RoutingPresetMap kHDMICaptureRGBPresets = {
	{"HDMI_HD_RGB_LFR_RGB_Capture",
	 {kConnectionKindHDMI,
	  NTV2_MODE_CAPTURE,
	  1,
	  1,
	  0,
	  true,
	  "hdmi[{ch1}][0]->fb[{ch1}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::HD_RGB_LFR,
	  SDIWireFormat::Unknown,
	  VPIDStandard_Unknown}},
};
} // aja
