#pragma once

#include "presets.hpp"

namespace aja {
static const RoutingPresetMap kSDICaptureRGBPresets = {
	// { "SD_ST352_RGB_Capture", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_CAPTURE,
	//     1, 1,
	//     0,
	//     true,
	//     "", {},
	//     RasterDefinition::SD,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::SD_ST352,
	//     VPIDStandard_483_576
	// } }, // NOTE(paulh): SD RGB not a valid capture config
	{"HD_720p_ST292_RGB_Capture",
	 {kConnectionKindSDI,
	  NTV2_MODE_CAPTURE,
	  1,
	  1,
	  0,
	  true,
	  "sdi[{ch1}][0]->dli[{ch1}][0];"
	  "sdi[{ch1}][1]->dli[{ch1}][1];"
	  "dli[{ch1}][0]->fb[{ch1}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_720p_ST292,
	  VPIDStandard_720}},
	{"HD_1080_ST292_RGB_Capture",
	 {kConnectionKindSDI,
	  NTV2_MODE_CAPTURE,
	  1,
	  1,
	  0,
	  true,
	  "sdi[{ch1}][0]->dli[{ch1}][0];"
	  "sdi[{ch1}][1]->dli[{ch1}][1];"
	  "dli[{ch1}][0]->fb[{ch1}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_1080_ST292,
	  VPIDStandard_1080}},
	{"HD_1080_ST372_Dual_RGB_Capture",
	 {kConnectionKindSDI,
	  NTV2_MODE_CAPTURE,
	  2,
	  1,
	  0,
	  true,
	  "sdi[{ch1}][0]->dli[{ch1}][0];"
	  "sdi[{ch2}][0]->dli[{ch1}][1];"
	  "dli[{ch1}][0]->fb[{ch1}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_1080_ST372_Dual,
	  VPIDStandard_1080_DualLink}},
	{"HD_720p_ST425_3Ga_RGB_Capture",
	 {kConnectionKindSDI,
	  NTV2_MODE_CAPTURE,
	  1,
	  1,
	  0,
	  true,
	  "sdi[{ch1}][0]->dli[{ch1}][0];"
	  "sdi[{ch1}][1]->dli[{ch1}][1];"
	  "dli[{ch1}][0]->fb[{ch1}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_720p_ST425_3Ga,
	  VPIDStandard_720_3Ga}},
	{"HD_1080p_ST425_3Ga_RGB_Capture",
	 {kConnectionKindSDI,
	  NTV2_MODE_CAPTURE,
	  1,
	  1,
	  0,
	  true,
	  "sdi[{ch1}][0]->dli[{ch1}][0];"
	  "sdi[{ch1}][1]->dli[{ch1}][1];"
	  "dli[{ch1}][0]->fb[{ch1}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_1080p_ST425_3Ga,
	  VPIDStandard_1080_3Ga}},
	{"HD_1080p_ST425_3Gb_DL_RGB_Capture",
	 {kConnectionKindSDI,
	  NTV2_MODE_CAPTURE,
	  1,
	  1,
	  0,
	  true,
	  "sdi[{ch1}][0]->dli[{ch1}][0];"
	  "sdi[{ch1}][1]->dli[{ch1}][1];"
	  "dli[{ch1}][0]->fb[{ch1}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_1080p_ST425_3Gb_DL,
	  VPIDStandard_1080_DualLink_3Gb}},
	{"HD_720p_ST425_3Gb_RGB_Capture",
	 {kConnectionKindSDI,
	  NTV2_MODE_CAPTURE,
	  1,
	  2,
	  0,
	  true,
	  "sdi[{ch1}][0]->dli[{ch1}][0];"
	  "sdi[{ch1}][1]->dli[{ch1}][1];"
	  "dli[{ch1}][0]->fb[{ch1}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_720p_ST425_3Gb,
	  VPIDStandard_720_3Gb}},
	{"HD_1080p_ST425_3Gb_RGB_Capture",
	 {kConnectionKindSDI,
	  NTV2_MODE_CAPTURE,
	  1,
	  2,
	  0,
	  true,
	  "sdi[{ch1}][0]->dli[{ch1}][0];"
	  "sdi[{ch1}][1]->dli[{ch1}][1];"
	  "dli[{ch1}][0]->fb[{ch1}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_1080p_ST425_3Gb,
	  VPIDStandard_1080_3Gb}},
	{"HD_1080p_ST425_Dual_3Ga_RGB_Capture",
	 {kConnectionKindSDI,
	  NTV2_MODE_CAPTURE,
	  2,
	  2,
	  0,
	  true,
	  "sdi[{ch1}][0]->dli[{ch1}][0];"
	  "sdi[{ch1}][1]->dli[{ch1}][1];"
	  "sdi[{ch2}][0]->dli[{ch2}][0];"
	  "sdi[{ch2}][1]->dli[{ch2}][1];"
	  "dli[{ch1}][0]->fb[{ch1}][0];"
	  "dli[{ch2}][0]->fb[{ch2}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_1080p_ST425_Dual_3Ga,
	  VPIDStandard_1080_Dual_3Ga}},
	{"HD_1080p_ST425_Dual_3Gb_RGB_Capture",
	 {kConnectionKindSDI,
	  NTV2_MODE_CAPTURE,
	  2,
	  2,
	  kConvert3GIn,
	  true,
	  "sdi[{ch1}][0]->dli[{ch1}][0];"
	  "sdi[{ch1}][1]->dli[{ch1}][1];"
	  "sdi[{ch2}][0]->dli[{ch2}][0];"
	  "sdi[{ch2}][1]->dli[{ch2}][1];"
	  "dli[{ch1}][0]->fb[{ch1}][0];"
	  "dli[{ch2}][0]->fb[{ch2}][0];",
	  {},
	  RasterDefinition::HD,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::HD_1080p_ST425_Dual_3Gb,
	  VPIDStandard_1080_Dual_3Gb}},
	// { "UHD4K_ST292_Quad_1_5_Squares_RGB_Capture", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_CAPTURE,
	//     4, 4,
	//     0,
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::UHD_4K,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::UHD4K_ST292_Quad_1_5_Squares,
	//     VPIDStandard_1080
	// } }, // NOTE(paulh): Not a valid RGB capture config
	// { "UHD4K_ST425_Quad_3Ga_Squares_RGB_Capture", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_CAPTURE,
	//     4, 4,
	//     0,
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::UHD_4K,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::UHD4K_ST425_Quad_3Ga_Squares,
	//     VPIDStandard_1080_3Ga
	// } }, // NOTE(paulh): Not a valid RGB capture config
	{"UHD4K_ST425_Quad_3Gb_Squares_RGB_Capture",
	 {
		 kConnectionKindSDI,
		 NTV2_MODE_CAPTURE,
		 4,
		 4,
		 kEnable4KQuads,
		 true,
		 // SDIs -> Dual-Links
		 "sdi[{ch1}][0]->dli[{ch1}][0];"
		 "sdi[{ch1}][1]->dli[{ch1}][1];"
		 "sdi[{ch2}][0]->dli[{ch2}][0];"
		 "sdi[{ch2}][1]->dli[{ch2}][1];"
		 "sdi[{ch3}][0]->dli[{ch3}][0];"
		 "sdi[{ch3}][1]->dli[{ch3}][1];"
		 "sdi[{ch4}][0]->dli[{ch4}][0];"
		 "sdi[{ch4}][1]->dli[{ch4}][1];" // Dual-Links -> Framestores
		 "dli[{ch1}][0]->fb[{ch1}][2];"
		 "dli[{ch2}][0]->fb[{ch2}][2];"
		 "dli[{ch3}][0]->fb[{ch3}][2];"
		 "dli[{ch4}][0]->fb[{ch4}][2];",
		 {},
		 RasterDefinition::UHD_4K,
		 HDMIWireFormat::Unknown,
		 SDIWireFormat::UHD4K_ST425_Quad_3Gb_Squares,
		 VPIDStandard_1080_DualLink_3Gb,
	 }},
	// { "UHD4K_ST425_Dual_3Gb_2SI_RGB_Capture", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_CAPTURE,
	//     2, 4,
	//     (kEnable3GOut | kEnable3GbOut),
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::UHD_4K,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::UHD4K_ST425_Dual_3Gb_2SI,
	//     VPIDStandard_2160_DualLink
	// } }, // NOTE(paulh): Not a valid RGB capture config
	{"UHD4K_ST425_Quad_3Ga_2SI_RGB_Capture",
	 {kConnectionKindSDI,
	  NTV2_MODE_CAPTURE,
	  4,
	  4,
	  (kEnable3GOut | kEnable4KTSI),
	  true,
	  // SDIs -> Dual-Links
	  "sdi[{ch1}][0]->dli[{ch1}][0];"
	  "sdi[{ch1}][1]->dli[{ch1}][1];"
	  "sdi[{ch2}][0]->dli[{ch2}][0];"
	  "sdi[{ch2}][1]->dli[{ch2}][1];"
	  "sdi[{ch3}][0]->dli[{ch3}][0];"
	  "sdi[{ch3}][1]->dli[{ch3}][1];"
	  "sdi[{ch4}][0]->dli[{ch4}][0];"
	  "sdi[{ch4}][1]->dli[{ch4}][1];" // Dual-Links -> TSI Mux
	  "dli[{ch1}][0]->tsi[{ch1}][0];"
	  "dli[{ch2}][0]->tsi[{ch1}][1];"
	  "dli[{ch3}][0]->tsi[{ch2}][0];"
	  "dli[{ch4}][0]->tsi[{ch2}][1];" // TSI Mux -> Framestores
	  "tsi[{ch1}][0]->fb[{ch1}][0];"
	  "tsi[{ch1}][1]->fb[{ch1}][1];"
	  "tsi[{ch2}][0]->fb[{ch2}][0];"
	  "tsi[{ch2}][1]->fb[{ch2}][1];",
	  {},
	  RasterDefinition::UHD_4K,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::UHD4K_ST425_Quad_3Ga_2SI,
	  VPIDStandard_2160_QuadLink_3Ga}},
	{"UHD4K_ST425_Quad_3Gb_2SI_RGB_Capture",
	 {kConnectionKindSDI,
	  NTV2_MODE_CAPTURE,
	  4,
	  4,
	  (kEnable3GOut | kEnable3GbOut | kEnable4KTSI),
	  true,
	  // SDIs -> Dual-Links
	  "sdi[{ch1}][0]->dli[{ch1}][0];"
	  "sdi[{ch1}][1]->dli[{ch1}][1];"
	  "sdi[{ch2}][0]->dli[{ch2}][0];"
	  "sdi[{ch2}][1]->dli[{ch2}][1];"
	  "sdi[{ch3}][0]->dli[{ch3}][0];"
	  "sdi[{ch3}][1]->dli[{ch3}][1];"
	  "sdi[{ch4}][0]->dli[{ch4}][0];"
	  "sdi[{ch4}][1]->dli[{ch4}][1];" // Dual-Links -> TSI Mux
	  "dli[{ch1}][0]->tsi[{ch1}][0];"
	  "dli[{ch2}][0]->tsi[{ch1}][1];"
	  "dli[{ch3}][0]->tsi[{ch2}][0];"
	  "dli[{ch4}][0]->tsi[{ch2}][1];" // TSI Mux -> Framestores
	  "tsi[{ch1}][0]->fb[{ch1}][0];"
	  "tsi[{ch1}][1]->fb[{ch1}][1];"
	  "tsi[{ch2}][0]->fb[{ch2}][0];"
	  "tsi[{ch2}][1]->fb[{ch2}][1];",
	  {},
	  RasterDefinition::UHD_4K,
	  HDMIWireFormat::Unknown,
	  SDIWireFormat::UHD4K_ST425_Quad_3Gb_2SI,
	  VPIDStandard_2160_QuadDualLink_3Gb}},
	// { "UHD4K_ST2018_6G_Squares_2SI_RGB_Capture", {
	//     kConnectionKindSDI,
	//     NTV2_MODE_CAPTURE,
	//     2, 2,
	//     kEnable6GOut,
	//     true,
	//     "",
	//     {},
	//     RasterDefinition::UHD_4K,
	//     HDMIWireFormat::Unknown,
	//     SDIWireFormat::UHD4K_ST2018_6G_Squares_2SI,
	//     VPIDStandard_2160_Single_6Gb
	// } }, // TODO(paulh)
};
} // aja
