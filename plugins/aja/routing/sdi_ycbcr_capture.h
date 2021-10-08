#pragma once

#include "../aja-routing.hpp"

// clang-format off
static const std::map<SDIWireFormat, RoutingConfig> kSDIYCbCrCaptureConfigs = {
	{
		SDIWireFormat::SD_ST352, {
			NTV2_MODE_CAPTURE,
			1,									// num wires
			1,									// num framestores
			false,								// enable 3G output?
			false,								// enable 6G output?
			false,								// enable 12G output?
			false,								// convert 3Gb -> 3Ga input?
			false,								// convert 3Ga -> 3Gb output?
			false,								// convert RGB 3Ga output?
			false,								// enable 3Gb output?
			false,								// enable 4K Square Division?
			false,								// enable 8K Square Division?
			false, 								// enable two-sample-interleave?
			"sdi[{ch1}][0]->fb[{ch1}][0]",		// YCbCr Capture Route
		},
	},
	{
		SDIWireFormat::HD_720p_ST292, {
			NTV2_MODE_CAPTURE,
			1, 1,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			"sdi[{ch1}][0]->fb[{ch1}][0]",
		}
	},
	{
		SDIWireFormat::HD_1080_ST292, {
			NTV2_MODE_CAPTURE,
			1, 1,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			"sdi[{ch1}][0]->fb[{ch1}][0]",
		}
	},
	{
		SDIWireFormat::HD_1080_ST372_Dual, {
			NTV2_MODE_CAPTURE,
			2, 2,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			"sdi[{ch1}][0]->fb[{ch1}][0]; sdi[{ch2}][0]->fb[{ch2}][0]",
		}
	},
	{
		SDIWireFormat::HD_720p_ST425_3Ga, {
			NTV2_MODE_CAPTURE,
			1, 1,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			"sdi[{ch1}][0]->fb[{ch1}][0]",
		}
	},
	{
		SDIWireFormat::HD_1080p_ST425_3Ga, {
			NTV2_MODE_CAPTURE,
			1, 1,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			"sdi[{ch1}][0]->fb[{ch1}][0]",
		}
	},
	{
		SDIWireFormat::HD_1080p_ST425_3Gb_DL, {
			NTV2_MODE_CAPTURE,
			1, 1,
			true,
			false,
			false,
			true,
			true,
			false,
			true,
			false,
			false,
			false,
			"sdi[{ch1}][0]->fb[{ch1}][0]",
		}
	},
	{
		SDIWireFormat::HD_720p_ST425_3Gb, {
			NTV2_MODE_CAPTURE,
			1, 2,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			"sdi[{ch1}][0]->fb[{ch1}][0]; sdi[{ch1}][1]->fb[{ch2}][0];",
		}
	},
	{
		SDIWireFormat::HD_1080p_ST425_3Gb, {
			NTV2_MODE_CAPTURE,
			1, 2,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			"sdi[{ch1}][0]->fb[{ch1}][0]; sdi[{ch1}][1]->fb[{ch2}][0];",
		}
	},
	{
		SDIWireFormat::HD_1080p_ST425_Dual_3Ga, {
			NTV2_MODE_CAPTURE,
			2, 2,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			"sdi[{ch1}][0]->fb[{ch1}][0];"\
			"sdi[{ch2}][0]->fb[{ch2}][0];",
		}
	},
	{
		SDIWireFormat::HD_1080p_ST425_Dual_3Gb, {
			NTV2_MODE_CAPTURE,
			2, 2,
			false,
			false,
			false,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			"sdi[{ch1}][0]->fb[{ch1}][0];"\
			"sdi[{ch2}][0]->fb[{ch2}][0];",
		}
	},
	{
		SDIWireFormat::UHD4K_ST292_Dual_1_5_Squares, {
			NTV2_MODE_CAPTURE,
			2, 4,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			true,
			false,
			false,
			"sdi[{ch1}][0]->fb[{ch1}][0];"\
			"sdi[{ch1}][1]->fb[{ch2}][0];"\
			"sdi[{ch2}][0]->fb[{ch3}][0];"\
			"sdi[{ch2}][1]->fb[{ch4}][0];",
		}
	},
	{
		SDIWireFormat::UHD4K_ST292_Quad_1_5_Squares, {
			NTV2_MODE_CAPTURE,
			4, 4,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			true,
			false,
			false,
			// Capture
			"sdi[{ch1}][0]->fb[{ch1}][0];"\
			"sdi[{ch2}][0]->fb[{ch2}][0];"\
			"sdi[{ch3}][0]->fb[{ch3}][0];"\
			"sdi[{ch4}][0]->fb[{ch4}][0];",
		}
	},
	{
		SDIWireFormat::UHD4K_ST425_Quad_3Ga_Squares, {
			NTV2_MODE_CAPTURE,
			4, 4,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			true,
			false,
			false,
			// Capture
			"sdi[{ch1}][0]->fb[{ch1}][0];"\
			"sdi[{ch2}][0]->fb[{ch2}][0];"\
			"sdi[{ch3}][0]->fb[{ch3}][0];"\
			"sdi[{ch4}][0]->fb[{ch4}][0];",
		}
	},
	{
		SDIWireFormat::UHD4K_ST425_Quad_3Gb_Squares, {
			NTV2_MODE_CAPTURE,
			4, 4,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			true,
			false,
			false,
			// Capture
			"sdi[{ch1}][0]->fb[{ch1}][0];"\
			"sdi[{ch2}][0]->fb[{ch2}][0];"\
			"sdi[{ch3}][0]->fb[{ch3}][0];"\
			"sdi[{ch4}][0]->fb[{ch4}][0];",
		}
	},
	{
		SDIWireFormat::UHD4K_ST425_Dual_3Gb_2SI, {
			NTV2_MODE_CAPTURE,
			2, 2,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			true,
			"sdi[{ch1}][0]->tsi[{ch1}][0];"\
			"sdi[{ch1}][1]->tsi[{ch1}][1];"\
			"sdi[{ch2}][0]->tsi[{ch2}][0];"\
			"sdi[{ch2}][1]->tsi[{ch2}][1];"\
			"tsi[{ch1}][0]->fb[{ch1}][0];"\
			"tsi[{ch1}][1]->fb[{ch1}][1];"\
			"tsi[{ch2}][0]->fb[{ch2}][0];"\
			"tsi[{ch2}][1]->fb[{ch2}][1];"\
		}
	},
	{
		SDIWireFormat::UHD4K_ST425_Quad_3Ga_2SI, {
			NTV2_MODE_CAPTURE,
			4, 4,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			true,
			"sdi[{ch1}][0]->tsi[{ch1}][0];"\
			"sdi[{ch2}][0]->tsi[{ch1}][1];"\
			"sdi[{ch3}][0]->tsi[{ch2}][0];"\
			"sdi[{ch4}][0]->tsi[{ch2}][1];"\
			"tsi[{ch1}][0]->fb[{ch1}][0];"\
			"tsi[{ch1}][1]->fb[{ch1}][1];"\
			"tsi[{ch2}][0]->fb[{ch2}][0];"\
			"tsi[{ch2}][1]->fb[{ch2}][1];",
		}
	},
	{
		SDIWireFormat::UHD4K_ST425_Quad_3Gb_2SI, {
			NTV2_MODE_CAPTURE,
			4, 4,
			false,
			false,
			false,
			true,
			false,
			false,
			false,
			false,
			false,
			true,
			"sdi[{ch1}][0]->tsi[{ch1}][0];"\
			"sdi[{ch2}][0]->tsi[{ch1}][1];"\
			"sdi[{ch3}][0]->tsi[{ch2}][0];"\
			"sdi[{ch4}][0]->tsi[{ch2}][1];"\
			"tsi[{ch1}][0]->fb[{ch1}][0];"\
			"tsi[{ch1}][1]->fb[{ch1}][1];"\
			"tsi[{ch2}][0]->fb[{ch2}][0];"\
			"tsi[{ch2}][1]->fb[{ch2}][1];",
		}
	},
	{
		SDIWireFormat::UHD4K_ST2018_6G_Squares_2SI, {
			NTV2_MODE_CAPTURE,
			1, 1,
			false,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			true,
			"sdi[{ch1}][0]->fb[{ch1}][0];",
		}
	},
	{
		SDIWireFormat::UHD4K_ST2018_6G_Squares_2SI_Kona5_io4KPlus, {
			NTV2_MODE_CAPTURE,
			2, 2,
			false,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			true,
			// Capture
			"sdi[{ch1}][0]->tsi[{ch1}][0];"\
			"sdi[{ch2}][0]->tsi[{ch1}][1];"\
			"sdi[{ch3}][0]->tsi[{ch2}][0];"\
			"sdi[{ch4}][0]->tsi[{ch2}][1];"\
			"tsi[{ch1}][0]->fb[{ch1}][0];"\
			"tsi[{ch1}][1]->fb[{ch1}][1];"\
			"tsi[{ch2}][0]->fb[{ch2}][0];"\
			"tsi[{ch2}][1]->fb[{ch2}][1];",
		}
	},
	{
		SDIWireFormat::UHD4K_ST2018_12G_Squares_2SI, {
			NTV2_MODE_CAPTURE,
			1, 1,
			false,
			false,
			true,
			false,
			false,
			true,
			false,
			false,
			false,
			true,
			"sdi[{ch1}][0]->fb[{ch1}][0];",
		}
	},
	{
		SDIWireFormat::UHD4K_ST2018_12G_Squares_2SI_Kona5_io4KPlus, {
			NTV2_MODE_CAPTURE,
			1, 1,
			false,
			false,
			true,
			false,
			false,
			true,
			false,
			false,
			false,
			true,
			"",
		}
	},
	{
		SDIWireFormat::UHD28K_ST2082_Dual_12G, {
			NTV2_MODE_CAPTURE,
			2, 2,
			false,
			false,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			true,
			"",
		}
	},
	{
		SDIWireFormat::UHD28K_ST2082_RGB_Dual_12G, {
			NTV2_MODE_CAPTURE,
			2, 2,
			false,
			false,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			true,
			"",
		}
	},
	{
		SDIWireFormat::UHD28K_ST2082_Quad_12G, {
			NTV2_MODE_CAPTURE,
			4, 4,
			false,
			false,
			true,
			false,
			false,
			false,
			false,
			false,
			false,
			true,
			"",
		}
	},
};
// clang-format on
