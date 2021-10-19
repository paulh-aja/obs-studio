#pragma once

#include "presets.hpp"

namespace aja {
static RoutingPresetMap kSDIDisplayRGBPresets = {
	// { "SD_ST352_RGB_Display", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_DISPLAY,
	//     1, 1,
	//     0,
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::SD,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::SD_ST352,
	//     VPIDStandard_483_576
	// } }, // NOTE(paulh): Not a valid RGB display config
	{"HD_720p_ST292_RGB_Display",
	 {kConnectionKindSDI,
	  NTV2_MODE_DISPLAY,
	  1,
	  1,
	  0,
	  true,
	  "fb[{ch1}][2]->dlo[{ch1}][0];"
	  "dlo[{ch1}][0]->sdi[{ch1}[0];"
	  "dlo[{ch1}][1]->sdi[{ch2}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_720p_ST292,
	  VPIDStandard_720}},
	{"HD_1080_ST292_RGB_Display",
	 {kConnectionKindSDI,
	  NTV2_MODE_DISPLAY,
	  1,
	  1,
	  0,
	  true,
	  "fb[{ch1}][2]->dlo[{ch1}][0];"
	  "dlo[{ch1}][0]->sdi[{ch1}[0];"
	  "dlo[{ch1}][1]->sdi[{ch2}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_1080_ST292,
	  VPIDStandard_1080}},
	{"HD_1080_ST372_Dual_RGB_Display",
	 {kConnectionKindSDI,
	  NTV2_MODE_DISPLAY,
	  2,
	  1,
	  0,
	  true,
	  "fb[{ch1}][2]->dlo[{ch1}][0];"
	  "dlo[{ch1}][0]->sdi[{ch1}[0];"
	  "dlo[{ch1}][1]->sdi[{ch2}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_1080_ST372_Dual,
	  VPIDStandard_1080_DualLink}},
	{"HD_720p_ST425_3Ga_RGB_Display",
	 {kConnectionKindSDI,
	  NTV2_MODE_DISPLAY,
	  1,
	  1,
	  (kEnable3GOut | kConvert3GaRGBOut),
	  true,
	  "fb[{ch1}][2]->dlo[{ch1}][0];"
	  "dlo[{ch1}][0]->sdi[{ch1}][0];"
	  "dlo[{ch1}][1]->sdi[{ch1}][1];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_720p_ST425_3Ga,
	  VPIDStandard_720_3Ga}},
	{"HD_1080p_ST425_3Ga_RGB_Display",
	 {kConnectionKindSDI,
	  NTV2_MODE_DISPLAY,
	  1,
	  1,
	  (kEnable3GOut | kConvert3GaRGBOut),
	  true,
	  "fb[{ch1}][2]->dlo[{ch1}][0];"
	  "dlo[{ch1}][0]->sdi[{ch1}][0];"
	  "dlo[{ch1}][1]->sdi[{ch1}][1];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_1080p_ST425_3Ga,
	  VPIDStandard_1080_3Ga}},
	{"HD_1080p_ST425_3Gb_DL_RGB_Display",
	 {kConnectionKindSDI,
	  NTV2_MODE_DISPLAY,
	  1,
	  1,
	  (kEnable3GOut | kEnable3GbOut),
	  true,
	  "fb[{ch1}][2]->dlo[{ch1}][0];"
	  "dlo[{ch1}][0]->sdi[{ch1}][0];"
	  "dlo[{ch1}][1]->sdi[{ch1}][1];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_1080p_ST425_3Gb_DL,
	  VPIDStandard_1080_DualLink_3Gb}},
	{"HD_720p_ST425_3Gb_RGB_Display",
	 {kConnectionKindSDI,
	  NTV2_MODE_DISPLAY,
	  1,
	  2,
	  (kEnable3GOut | kEnable3GbOut),
	  true,
	  "fb[{ch1}][2]->dlo[{ch1}][0];"
	  "dlo[{ch1}][0]->sdi[{ch1}][0];"
	  "dlo[{ch1}][1]->sdi[{ch1}][1];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_720p_ST425_3Gb,
	  VPIDStandard_720_3Gb}},
	{"HD_1080p_ST425_3Gb_RGB_Display",
	 {kConnectionKindSDI,
	  NTV2_MODE_DISPLAY,
	  1,
	  2,
	  (kEnable3GOut | kEnable3GbOut),
	  true,
	  "fb[{ch1}][2]->dlo[{ch1}][0];"
	  "dlo[{ch1}][0]->sdi[{ch1}][0];"
	  "dlo[{ch1}][1]->sdi[{ch1}][1];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_1080p_ST425_3Gb,
	  VPIDStandard_1080_3Gb}},
	// { "HD_1080p_ST425_Dual_3Ga_RGB_Display", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_DISPLAY,
	//     2, 2,
	//     kEnable3GOut,
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::HD,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::HD_1080p_ST425_Dual_3Ga,
	//     VPIDStandard_1080_Dual_3Ga
	// } },
	// { "HD_1080p_ST425_Dual_3Gb_RGB_Display", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_DISPLAY,
	//     2, 2,
	//     (kEnable3GOut | kEnable3GbOut),
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::HD,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::HD_1080p_ST425_Dual_3Gb,
	//     VPIDStandard_1080_Dual_3Gb
	// } },
	// { "UHD4K_ST292_Quad_1_5_Squares_RGB_Display", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_DISPLAY,
	//     4, 4,
	//     kEnable4KQuads,
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::UHD_4K,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::UHD4K_ST292_Quad_1_5_Squares,
	//     VPIDStandard_1080
	// } },
	// { "UHD4K_ST425_Quad_3Ga_Squares_RGB_Display", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_DISPLAY,
	//     4, 4,
	//     (kEnable3GOut | kEnable4KQuads),
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::UHD_4K,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::UHD4K_ST425_Quad_3Ga_Squares,
	//     VPIDStandard_1080_3Ga
	// } },
	{"UHD4K_ST425_Quad_3Gb_Squares_RGB_Display",
	 {kConnectionKindSDI,
	  NTV2_MODE_DISPLAY,
	  4,
	  4,
	  (kEnable3GOut | kEnable3GbOut | kEnable4KQuads),
	  true,
	  // Framestores -> Dual-Links
	  "fb[{ch1}][2]->dlo[{ch1}][0];"
	  "fb[{ch2}][2]->dlo[{ch2}][0];"
	  "fb[{ch3}][2]->dlo[{ch3}][0];"
	  "fb[{ch4}][2]->dlo[{ch4}][0];" // Dual-Links -> SDIs
	  "dlo[{ch1}][0]->sdi[{ch1}][0];"
	  "dlo[{ch1}][1]->sdi[{ch1}][1];"
	  "dlo[{ch2}][0]->sdi[{ch2}][0];"
	  "dlo[{ch2}][1]->sdi[{ch2}][1];"
	  "dlo[{ch3}][0]->sdi[{ch3}][0];"
	  "dlo[{ch3}][1]->sdi[{ch3}][1];"
	  "dlo[{ch4}][0]->sdi[{ch4}][0];"
	  "dlo[{ch4}][1]->sdi[{ch4}][1];",
	  {},
	  RasterDefinition::UHD_4K,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::UHD4K_ST425_Quad_3Gb_Squares,
	  VPIDStandard_1080_DualLink_3Gb}},
	// { "UHD4K_ST425_Dual_3Gb_2SI_RGB_Display", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_DISPLAY,
	//     2, 4,
	//     (kEnable3GOut | kEnable3GbOut | kEnable4KQuads),
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::UHD_4K,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::UHD4K_ST425_Dual_3Gb_2SI,
	//     VPIDStandard_2160_DualLink
	// } },
	{"UHD4K_ST425_Quad_3Ga_2SI_RGB_Display",
	 {kConnectionKindSDI,
	  NTV2_MODE_DISPLAY,
	  4,
	  4,
	  (kEnable3GOut | kEnable4KTSI),
	  true,
	  "fb[{ch1}][2]->tsi[{ch1}][0];"
	  "fb[{ch1}][3]->tsi[{ch1}][1];"

	  "fb[{ch2}][2]->tsi[{ch2}][0];"
	  "fb[{ch2}][3]->tsi[{ch2}][1];"

	  "tsi[{ch1}][2]->dlo[{ch1}][0];"
	  "tsi[{ch1}][3]->dlo[{ch2}][0];"
	  "tsi[{ch2}][2]->dlo[{ch3}][0];"
	  "tsi[{ch2}][3]->dlo[{ch4}][0];"

	  "dlo[{ch1}][0]->sdi[{ch1}][0];"
	  "dlo[{ch1}][1]->sdi[{ch1}][1];"
	  "dlo[{ch2}][0]->sdi[{ch2}][0];"
	  "dlo[{ch2}][1]->sdi[{ch2}][1];"
	  "dlo[{ch3}][0]->sdi[{ch3}][0];"
	  "dlo[{ch3}][1]->sdi[{ch3}][1];"
	  "dlo[{ch4}][0]->sdi[{ch4}][0];"
	  "dlo[{ch4}][1]->sdi[{ch4}][1];",
	  {},
	  RasterDefinition::UHD_4K,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::UHD4K_ST425_Quad_3Ga_2SI,
	  VPIDStandard_2160_QuadLink_3Ga}},
	{"UHD4K_ST425_Quad_3Gb_2SI_RGB_Display",
	 {kConnectionKindSDI,
	  NTV2_MODE_DISPLAY,
	  4,
	  4,
	  (kEnable3GOut | kEnable4KTSI),
	  true,
	  "fb[{ch1}][2]->tsi[{ch1}][0];"
	  "fb[{ch1}][3]->tsi[{ch1}][1];"

	  "fb[{ch2}][2]->tsi[{ch2}][0];"
	  "fb[{ch2}][3]->tsi[{ch2}][1];"

	  "tsi[{ch1}][2]->dlo[{ch1}][0];"
	  "tsi[{ch1}][3]->dlo[{ch2}][0];"
	  "tsi[{ch2}][2]->dlo[{ch3}][0];"
	  "tsi[{ch2}][3]->dlo[{ch4}][0];"

	  "dlo[{ch1}][0]->sdi[{ch1}][0];"
	  "dlo[{ch1}][1]->sdi[{ch1}][1];"
	  "dlo[{ch2}][0]->sdi[{ch2}][0];"
	  "dlo[{ch2}][1]->sdi[{ch2}][1];"
	  "dlo[{ch3}][0]->sdi[{ch3}][0];"
	  "dlo[{ch3}][1]->sdi[{ch3}][1];"
	  "dlo[{ch4}][0]->sdi[{ch4}][0];"
	  "dlo[{ch4}][1]->sdi[{ch4}][1];",
	  {},
	  RasterDefinition::UHD_4K,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::UHD4K_ST425_Quad_3Gb_2SI,
	  VPIDStandard_2160_QuadDualLink_3Gb}},
	// { "UHD4K_ST2018_6G_Squares_2SI_RGB_Display", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_DISPLAY,
	//     2, 4,
	//     (kEnable6GOut | kEnable4KTSI),
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::UHD_4K,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::UHD4K_ST2018_6G_Squares_2SI,
	//     VPIDStandard_2160_Single_6Gb
	// } }, // TODO(paulh)
	// { "UHD4K_ST2018_12G_Squares_2SI_RGB_Display", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_DISPLAY,
	//     2, 4,
	//     (kEnable12GOut | kEnable4KTSI),
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::UHD_4K,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::UHD4K_ST2018_12G_Squares_2SI,
	//     VPIDStandard_2160_Single_12Gb
	// } }, // TODO(paulh)
	// { "UHD28K_ST2082_Dual_12G_RGB_Display", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_DISPLAY,
	//     2, 4,
	//     (kEnable12GOut | kEnable4KTSI),
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::UHD_4K,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::UHD28K_ST2082_Dual_12G,
	//     VPIDStandard_4320_DualLink_12Gb
	// } }, // TODO(paulh)
	// { "UHD28K_ST2082_RGB_Dual_12G_RGB_Display", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_DISPLAY,
	//     2, 4,
	//     (kEnable12GOut | kEnable4KTSI),
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::UHD_4K,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::UHD28K_ST2082_RGB_Dual_12G,
	//     VPIDStandard_2160_DualLink_12Gb
	// } }, // TODO(paulh)
	// { "UHD28K_ST2082_Quad_12G_RGB_Display", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_DISPLAY,
	//     2, 4,
	//     (kEnable12GOut | kEnable4KTSI),
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::UHD_4K,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::UHD28K_ST2082_Quad_12G,
	//     VPIDStandard_4320_QuadLink_12Gb
	// } }, // TODO(paulh)
};
} // aja