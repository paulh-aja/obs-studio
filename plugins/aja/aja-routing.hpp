#pragma once

#include "aja-props.hpp"

#include <ajantv2/includes/ntv2enums.h>
#include <ajantv2/includes/ntv2signalrouter.h>

#include <string>
#include <map>

class CNTV2Card;

namespace aja {

using WidgetChannelMap = std::map<std::string, NTV2ChannelList>;

/* The AJA hardware and NTV2 SDK uses a concept called "Signal Routing" to connect high-level firmware
 * blocks known as "Widgets" to one another via "crosspoint" connections. This facilitates streaming
 * data from one Widget to another to achieve specific functionality.
 * Such functionality may include SDI/HDMI capture/output, colorspace conversion, hardware LUTs, etc.
 *
 * This code references a table of RoutingPreset entries, where each entry contains the settings required
 * to configure an AJA device for a particular capture or output task. These settings include the number of
 * physical IO Widgets (SDI or HDMI) required, number of framestore Widgets required, register settings
 * that must be enabled or disabled, and a special short-hand "route string".
 * Of special note is the route string, which is parsed into a map of NTV2XptConnections. These connections
 * are then applied as the "signal route", connecting the Widget's crosspoints together.
 */
typedef enum {
	AJA_RP_CAP_HDMI_ONE_WIRE,
	HDMI_HD_HFR_RGB_Capture,
	HDMI_UHD_4K_RGB_Capture_io4KPlus,
	AJA_RP_OUT_HDMI_ONE_WIRE_RGB,
	HDMI_LFR_RGB_Display_TTapPro,
	HDMI_HFR_RGB_Display_TTapPro,
	HDMI_HD_LFR_YCbCr_Capture,
	HDMI_UHD_4K_YCbCr_Capture_io4KPlus,
	HDMI_UHD_4K_YCbCr_Capture,
	AJA_RP_OUT_HDMI_ONE_WIRE_YCBCR,
	HDMI_UHD_4K_LFR_YCbCr_Display,
	HDMI_UHD_4K_LFR_YCbCr_Display_Kona5_8K,
	HDMI_HD_LFR_YCbCr_Display_TTapPro,
	HDMI_HD_HFR_YCbCr_Display_TTapPro,
	HDMI_UHD_4K_LFR_YCbCr_Display_TTapPro,
	HDMI_UHD_4K_HFR_YCbCr_Display_TTapPro,
	HD_720p_ST292_RGB_Capture,
	HD_1080_ST292_RGB_Capture,
	HD_1080_ST372_Dual_RGB_Capture,
	HD_720p_ST425_3Ga_RGB_Capture,
	HD_1080p_ST425_3Ga_RGB_Capture,
	HD_1080p_ST425_3Gb_DL_RGB_Capture,
	HD_720p_ST425_3Gb_RGB_Capture,
	HD_1080p_ST425_3Gb_RGB_Capture,
	HD_1080p_ST425_Dual_3Ga_RGB_Capture,
	HD_1080p_ST425_Dual_3Gb_RGB_Capture,
	UHD4K_ST425_Quad_3Ga_Squares_RGB_Capture,
	UHD4K_ST425_Quad_3Gb_Squares_RGB_Capture,
	UHD4K_ST425_Quad_3Ga_2SI_RGB_Capture,
	UHD4K_ST425_Quad_3Gb_2SI_RGB_Capture,
	UHD4K_ST2018_6G_Squares_2SI_RGB_Capture,
	UHD4K_ST2018_6G_Squares_2SI_RGB_Capture_Kona5_io4KPlus,
	UHD4K_ST2018_12G_Squares_2SI_RGB_Capture,
	UHD4K_ST2018_12G_Squares_2SI_RGB_Capture_Kona5_io4KPlus,
	HD_720p_ST292_RGB_Display,
	HD_1080_ST292_RGB_Display,
	HD_1080_ST372_Dual_RGB_Display,
	HD_720p_ST425_3Ga_RGB_Display,
	HD_1080p_ST425_3Ga_RGB_Display,
	HD_1080p_ST425_3Gb_DL_RGB_Display,
	HD_720p_ST425_3Gb_RGB_Display,
	HD_1080p_ST425_3Gb_RGB_Display,
	UHD4K_ST425_Quad_3Gb_Squares_RGB_Display,
	UHD4K_ST425_Quad_3Ga_2SI_RGB_Display,
	UHD4K_ST425_Quad_3Gb_2SI_RGB_Display,
	AJA_RP_CAP_SDI_SINGLE_SD_HD_YCBCR,
	HD_1080_ST372_Dual_YCbCr_Capture,
	HD_1080p_ST425_3Gb_DL_YCbCr_Capture,
	HD_720p_ST425_3Gb_YCbCr_Capture,
	HD_1080p_ST425_3Gb_YCbCr_Capture,
	HD_1080p_ST425_Dual_3Ga_YCbCr_Capture,
	HD_1080p_ST425_Dual_3Gb_YCbCr_Capture,
	UHD4K_ST292_Quad_1_5_Squares_YCbCr_Capture,
	UHD4K_ST425_Quad_3Ga_Squares_YCbCr_Capture,
	UHD4K_ST425_Quad_3Gb_Squares_YCbCr_Capture,
	UHD4K_ST425_Dual_3Gb_2SI_YCbCr_Capture,
	UHD4K_ST425_Quad_3Ga_2SI_YCbCr_Capture,
	UHD4K_ST425_Quad_3Gb_2SI_YCbCr_Capture,
	UHD4K_ST2018_6G_Squares_2SI_YCbCr_Capture,
	UHD4K_ST2018_6G_Squares_2SI_YCbCr_Capture_Kona5_io4KPlus,
	UHD4K_ST2018_12G_Squares_2SI_YCbCr_Capture,
	UHD4K_ST2018_12G_Squares_2SI_YCbCr_Capture_Kona5_io4KPlus,
	UHD28K_ST2082_Dual_12G_YCbCr_Capture,
	UHD28K_ST2082_Quad_12G_YCbCr_Capture,
	AJA_RP_OUT_SDI_SINGLE_SD_HD_YCBCR,
	HD_1080_ST372_Dual_YCbCr_Display,
	AJA_RP_OUT_SDI_3Ga_HD_YCBCR,
	HD_1080p_ST425_3Gb_DL_YCbCr_Display,
	AJA_RP_OUT_SDI_3GbDS_HD_YCBCR,
	HD_1080p_ST425_Dual_3Ga_YCbCr_Display,
	HD_1080p_ST425_Dual_3Gb_YCbCr_Display,
	UHD4K_ST292_Quad_1_5_Squares_YCbCr_Display,
	UHD4K_ST425_Quad_3Ga_Squares_YCbCr_Display,
	UHD4K_ST425_Quad_3Gb_Squares_YCbCr_Display,
	UHD4K_ST425_Dual_3Gb_2SI_YCbCr_Display,
	UHD4K_ST425_Quad_3Ga_2SI_YCbCr_Display,
	UHD4K_ST425_Quad_3Gb_2SI_YCbCr_Display,
	UHD4K_ST2018_6G_Squares_2SI_YCbCr_Display,
	UHD4K_ST2018_6G_Squares_2SI_YCbCr_Display_Kona5_io4KPlus,
	UHD4K_ST2018_12G_Squares_2SI_YCbCr_Display,
	UHD4K_ST2018_12G_Squares_2SI_YCbCr_Display_Kona5_io4KPlus,
	UHD28K_ST2082_Dual_12G_YCbCr_Display,
	UHD28K_ST2082_Quad_12G_YCbCr_Display,
	AJA_RP_INVALID
} RoutingPresetID;

extern std::string RoutingPresetIDString(RoutingPresetID id);

class RoutingPreset {
public:
	bool ToConnections(NTV2XptConnections &cnx,
			   NTV2Channel firstChannel = NTV2_CHANNEL1,
			   NTV2Channel firstFramestore = NTV2_CHANNEL1) const;
	bool
	ToWidgetChannelMap(WidgetChannelMap &wcMap,
			   NTV2Channel firstChannel = NTV2_CHANNEL1,
			   NTV2Channel firstFramestore = NTV2_CHANNEL1) const;
	bool ToChannelList(NTV2ChannelList &chans, NTV2Mode mode,
			   NTV2Channel firstChannel = NTV2_CHANNEL1,
			   NTV2Channel firstFramestore = NTV2_CHANNEL1) const;

	bool IsKona5Io4KPlus6G12GCapture() const;
	bool IsKona5Io4KPlus6G12GOutput() const;

	RoutingPresetID id;
	ConnectionKind kind;
	NTV2Mode mode;
	uint32_t num_channels;
	uint32_t num_framestores;
	uint32_t flags;
	std::vector<RasterDefinition> raster_defs;
	std::vector<VPIDStandard> vpid_standards;
	std::vector<NTV2DeviceID> device_ids;
	bool rgb_ok;
	std::string route_string;

private:
	std::string routeStringSubstitute(
		NTV2Channel firstChannel = NTV2_CHANNEL1,
		NTV2Channel firstFramestore = NTV2_CHANNEL1) const;
};
using RoutingPresets = std::vector<RoutingPreset>;
using RoutingPresetPair = std::pair<RoutingPresetID, RoutingPreset>;
using RoutingPresetMap = std::map<RoutingPresetID, RoutingPreset>;

// Applies RoutingPreset settings to the card to configure a specific SDI/HDMI capture/output mode.
class RoutingManager {
public:
	RoutingManager();
	void AddPreset(RoutingPresetID id, const RoutingPreset &preset);
	bool PresetByName(RoutingPresetID id, RoutingPreset &preset) const;
	bool FindPreset(const IOConfig &ioConf, RoutingPreset &rp);
	bool FindFirstPreset(ConnectionKind kind, NTV2DeviceID id,
			     NTV2Mode mode, NTV2VideoFormat vf,
			     NTV2PixelFormat pf, VPIDStandard standard,
			     HDMIWireFormat hwf, RoutingPreset &preset);

	bool ConfigureRouting(const IOConfig &ioConf, const RoutingPreset &rp,
			      CNTV2Card *card, NTV2XptConnections &cnx);

	static void StartSourceAudio(const IOConfig &ioConf, CNTV2Card *card);
	static void StopSourceAudio(const IOConfig &ioConf, CNTV2Card *card);
	static void ConfigureOutputAudio(const IOConfig &ioConf,
					 CNTV2Card *card);

	static bool ParseRouteString(const std::string &route,
				     NTV2XptConnections &cnx);
	static void LogRoutingPreset(const RoutingPreset &rp);

private:
	void cache_connections(const NTV2XptConnections &cnx);
	void clear_cached_connections();
	void apply_cached_connections(CNTV2Card *card);
	void remove_cached_connections(CNTV2Card *card);

	void build_preset_table();

	RoutingPresetMap mPresetMap;
	NTV2XptConnections mXptCache;
};

} // namespace aja
