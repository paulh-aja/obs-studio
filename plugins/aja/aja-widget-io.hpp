#pragma once

#include <ajantv2/includes/ntv2enums.h>

#include <string>

// firmware widget nicknames used by signal routing syntax parser
static constexpr const char *kFramebufferNickname = "fb";
static constexpr const char *kCSCNickname = "csc";
static constexpr const char *kDualLinkInNickname = "dli";
static constexpr const char *kDualLinkOutNickname = "dlo";
static constexpr const char *kLUTNickname = "lut";
static constexpr const char *kSDINickname = "sdi";
static constexpr const char *kMultiLinkNickname = "ml";
static constexpr const char *kMixerNickname = "mix";
static constexpr const char *kHDMINickname = "hdmi";
static constexpr const char *kLUT3DNickname = "lut3d";
static constexpr const char *k4KDownConvertNickname = "4kdc";
static constexpr const char *kAnalogNickname = "analog";
static constexpr const char *kTSIMuxNickname = "tsi";
static constexpr const char *kUpDownConvertNickname = "udc";
static constexpr const char *kCompositeNickname = "composite";
static constexpr const char *kStereoCompNickname = "stereo";
static constexpr const char *kWatermarkNickname = "watermark";
static constexpr const char *kBlackNickname = "black";
static constexpr const char *kCompressionNickname = "comp";
static constexpr const char *kFrameSyncNickname = "fsync";
static constexpr const char *kTestPatternNickname = "pat";

using OutputXpt = NTV2OutputCrosspointID; // src
using InputXpt = NTV2InputCrosspointID;   // dest

// Firmware widget input socket connector
struct WidgetInputSocket {
	InputXpt id;
	NTV2WidgetID widget_id;
	NTV2Channel channel;
	int32_t datastream_index;
	const char *name;

	static bool Find(const std::string &route, NTV2Channel channel,
			 int32_t datastream, WidgetInputSocket &inp);
	static bool GetWidgetInputSocketByXpt(InputXpt id,
					      WidgetInputSocket &inp);
	static int32_t InputXptDatastreamIndex(InputXpt xpt);
	static NTV2Channel InputXptChannel(InputXpt xpt);
	static const char *InputXptName(InputXpt xpt);
};

// Firmware widget output socket connector
struct WidgetOutputSocket {
	OutputXpt id;
	NTV2WidgetID widget_id;
	NTV2Channel channel;
	int32_t datastream_index;
	const char *name;

	static bool Find(const std::string &route, NTV2Channel channel,
			 int32_t datastream, WidgetOutputSocket &out);
	static bool GetWidgetOutputSocketByXpt(OutputXpt id,
					       WidgetOutputSocket &out);
	static int32_t OutputXptDatastreamIndex(OutputXpt xpt);
	static NTV2Channel OutputXptChannel(OutputXpt xpt);
	static const char *OutputXptName(OutputXpt xpt);
};
