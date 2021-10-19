#include "aja-card-manager.hpp"
#include "aja-common.hpp"
#include "aja-ui-props.hpp"
#include "aja-props.hpp"

#include <ajantv2/includes/ntv2card.h>
#include <ajantv2/includes/ntv2devicescanner.h>
#include <ajantv2/includes/ntv2devicefeatures.h>
#include <ajantv2/includes/ntv2utils.h>

namespace aja {

video_format AJAPixelFormatToOBSVideoFormat(NTV2PixelFormat pf)
{
	video_format obs_video_format = VIDEO_FORMAT_NONE;
	switch (pf) {
	case NTV2_FBF_8BIT_YCBCR:
		obs_video_format = VIDEO_FORMAT_UYVY;
		break;
	case NTV2_FBF_24BIT_RGB:
	case NTV2_FBF_24BIT_BGR:
		obs_video_format = VIDEO_FORMAT_BGR3;
		break;
	case NTV2_FBF_ARGB:
	case NTV2_FBF_ABGR:
	case NTV2_FBF_RGBA:
		obs_video_format = VIDEO_FORMAT_BGRA;
		break;
	case NTV2_FBF_10BIT_YCBCR:
	case NTV2_FBF_10BIT_RGB:
	case NTV2_FBF_8BIT_YCBCR_YUY2:
	case NTV2_FBF_10BIT_DPX:
	case NTV2_FBF_10BIT_YCBCR_DPX:
	case NTV2_FBF_8BIT_DVCPRO:
	case NTV2_FBF_8BIT_YCBCR_420PL3:
	case NTV2_FBF_8BIT_HDV:
	case NTV2_FBF_10BIT_YCBCRA:
	case NTV2_FBF_10BIT_DPX_LE:
	case NTV2_FBF_48BIT_RGB:
	case NTV2_FBF_12BIT_RGB_PACKED:
	case NTV2_FBF_PRORES_DVCPRO:
	case NTV2_FBF_PRORES_HDV:
	case NTV2_FBF_10BIT_RGB_PACKED:
	case NTV2_FBF_10BIT_ARGB:
	case NTV2_FBF_16BIT_ARGB:
	case NTV2_FBF_8BIT_YCBCR_422PL3:
	case NTV2_FBF_10BIT_RAW_RGB:
	case NTV2_FBF_10BIT_RAW_YCBCR:
	case NTV2_FBF_10BIT_YCBCR_420PL3_LE:
	case NTV2_FBF_10BIT_YCBCR_422PL3_LE:
	case NTV2_FBF_10BIT_YCBCR_420PL2:
	case NTV2_FBF_10BIT_YCBCR_422PL2:
	case NTV2_FBF_8BIT_YCBCR_420PL2:
	case NTV2_FBF_8BIT_YCBCR_422PL2:
	default:
		obs_video_format = VIDEO_FORMAT_NONE;
		break;
	}

	return obs_video_format;
}

void GetSortedVideoFormats(NTV2DeviceID id, const VideoStandardList &standards,
			   VideoFormatList &videoFormats)
{
	if (standards.empty())
		return;

	VideoFormatMap videoFormatMap;

	// Bin all the formats based on video standard
	for (size_t i = (size_t)NTV2_FORMAT_UNKNOWN;
	     i < (size_t)NTV2_MAX_NUM_VIDEO_FORMATS; i++) {
		NTV2VideoFormat fmt = (NTV2VideoFormat)i;
		NTV2Standard standard = GetNTV2StandardFromVideoFormat(fmt);

		bool addFmt = true;

		if (id != DEVICE_ID_NOTFOUND) {
			addFmt = NTV2DeviceCanDoVideoFormat(id, fmt);
		}

		if (addFmt) {
			if (videoFormatMap.count(standard)) {
				videoFormatMap.at(standard).push_back(fmt);
			} else {
				std::vector<NTV2VideoFormat> v;

				v.push_back(fmt);

				videoFormatMap.insert(
					std::pair<NTV2Standard,
						  std::vector<NTV2VideoFormat>>(
						standard, v));
			}
		}
	}

	for (size_t v = (size_t)NTV2_STANDARD_1080;
	     v < (size_t)NTV2_NUM_STANDARDS; v++) {
		NTV2Standard standard = (NTV2Standard)v;

		if (videoFormatMap.count(standard)) {
			std::sort(videoFormatMap.at(standard).begin(),
				  videoFormatMap.at(standard).end(),
				  [&](const NTV2VideoFormat &d1,
				      const NTV2VideoFormat &d2) {
					  std::string d1Str, d2Str;

					  d1Str = NTV2VideoFormatToString(d1);
					  d2Str = NTV2VideoFormatToString(d2);

					  return d1Str < d2Str;
				  });
		}
	}

	for (size_t v = 0; v < standards.size(); v++) {
		NTV2Standard standard = standards.at(v);
		if (videoFormatMap.count(standard)) {
			for (size_t i = 0;
			     i < videoFormatMap.at(standard).size(); i++) {
				NTV2VideoFormat vf =
					videoFormatMap.at(standard).at(i);
				videoFormats.push_back(vf);
			}
		}
	}
}

uint32_t CardNumFramestores(NTV2DeviceID id)
{
	auto numFramestores = NTV2DeviceGetNumFrameStores(id);
	if (id == DEVICE_ID_CORVIDHBR) {
		numFramestores = 1;
	}
	return numFramestores;
}

uint32_t CardNumAudioSystems(NTV2DeviceID id)
{
	if (id == DEVICE_ID_KONALHI || id == DEVICE_ID_KONALHEPLUS)
		return 2;

	return NTV2DeviceGetNumAudioSystems(id);
}

// IO4K and IO4K+ perform SDI Monitor Output on "SDI5" and "Framestore 4".
bool CardCanDoSDIMonitorOutput(NTV2DeviceID id)
{
	return (id == DEVICE_ID_IO4K || id == DEVICE_ID_IO4KPLUS);
}

// Cards with a dedicated HDMI Monitor Output tie it to "Framestore 4".
bool CardCanDoHDMIMonitorOutput(NTV2DeviceID id)
{
	return (id == DEVICE_ID_IO4K || id == DEVICE_ID_IO4KPLUS ||
		id == DEVICE_ID_IOXT || id == DEVICE_ID_IOX3 ||
		id == DEVICE_ID_KONA4 || id == DEVICE_ID_KONA5 ||
		id == DEVICE_ID_KONA5_8K || id == DEVICE_ID_KONA5_2X4K ||
		id == DEVICE_ID_KONA5_8KMK);
}

// Cards capable of 1x SDI at 6G/12G.
bool CardCanDo1xSDI12G(NTV2DeviceID id)
{
	return (id == DEVICE_ID_KONA5_8K || id == DEVICE_ID_KONA5_8KMK ||
		id == DEVICE_ID_KONA5 || id == DEVICE_ID_KONA5_2X4K ||
		id == DEVICE_ID_IO4KPLUS || id == DEVICE_ID_CORVID44_12G);
}

// Check for 3G level-B SDI on the wire.
bool Is3GLevelB(CNTV2Card *card, NTV2Channel channel)
{
	if (!card)
		return false;

	bool levelB = false;
	auto deviceID = card->GetDeviceID();
	UWord channelIndex = static_cast<UWord>(channel);

	if (NTV2DeviceCanDo3GIn(deviceID, channelIndex) ||
	    NTV2DeviceCanDo12GIn(deviceID, channelIndex)) {
		if (!card->GetSDIInput3GbPresent(levelB, channel))
			return false;
	}

	return levelB;
}

// Get the 3G Level-A enum for a 3G Level-B format enum.
NTV2VideoFormat GetLevelAFormatForLevelBFormat(NTV2VideoFormat vf)
{
	NTV2VideoFormat result = vf;
	switch (vf) {
	default:
		break;
	case NTV2_FORMAT_1080p_5000_B:
		result = NTV2_FORMAT_1080p_5000_A;
		break;
	case NTV2_FORMAT_1080p_5994_B:
		result = NTV2_FORMAT_1080p_5994_A;
		break;
	case NTV2_FORMAT_1080p_6000_B:
		result = NTV2_FORMAT_1080p_6000_A;
		break;
	case NTV2_FORMAT_1080p_2K_4795_B:
		result = NTV2_FORMAT_1080p_2K_4795_A;
		break;
	case NTV2_FORMAT_1080p_2K_4800_B:
		result = NTV2_FORMAT_1080p_2K_4800_A;
		break;
	case NTV2_FORMAT_1080p_2K_5000_B:
		result = NTV2_FORMAT_1080p_2K_5000_A;
		break;
	case NTV2_FORMAT_1080p_2K_5994_B:
		result = NTV2_FORMAT_1080p_2K_5994_A;
		break;
	case NTV2_FORMAT_1080p_2K_6000_B:
		result = NTV2_FORMAT_1080p_2K_6000_A;
		break;
	}
	return result;
}

NTV2VideoFormat InterlacedFormatForPsfFormat(NTV2VideoFormat vf)
{
	NTV2VideoFormat result = vf;
	switch (vf) {
	default:
		break;
	case NTV2_FORMAT_1080psf_2500_2:
		result = NTV2_FORMAT_1080i_5000;
		break;
	case NTV2_FORMAT_1080psf_2997_2:
		result = NTV2_FORMAT_1080i_5994;
		break;
	}
	return result;
}

// Certain cards only have 1 SDI spigot.
bool IsSingleSDIDevice(NTV2DeviceID id)
{
	return (id == DEVICE_ID_TTAP_PRO || id == DEVICE_ID_KONA1);
}

bool IsIODevice(NTV2DeviceID id)
{
	return (id == DEVICE_ID_IOXT || id == DEVICE_ID_IOX3 ||
		id == DEVICE_ID_IO4K || id == DEVICE_ID_IO4KPLUS ||
		id == DEVICE_ID_IOIP_2022 || id == DEVICE_ID_IOIP_2110);
}

bool IsRetailSDI12G(NTV2DeviceID id)
{
	return (id == DEVICE_ID_KONA5 || id == DEVICE_ID_IO4KPLUS);
}

bool IsOutputOnlyDevice(NTV2DeviceID id)
{
	return id == DEVICE_ID_TTAP_PRO;
}

std::string SDI4KTransportToString(SDI4KTransport mode)
{
	std::string str = "";
	switch (mode) {
	case SDI4KTransport::Squares:
		str = "Squares";
		break;
	case SDI4KTransport::TwoSampleInterleave:
		str = "2SI";
		break;
	default:
	case SDI4KTransport::Unknown:
		str = "Unknown";
		break;
	}
	return str;
}

std::string IOSelectionToString(IOSelection io)
{
	std::string str;

	switch (io) {
	case IOSelection::SDI1:
		str = "SDI 1";
		break;
	case IOSelection::SDI2:
		str = "SDI 2";
		break;
	case IOSelection::SDI3:
		str = "SDI 3";
		break;
	case IOSelection::SDI4:
		str = "SDI 4";
		break;
	case IOSelection::SDI5:
		str = "SDI 5";
		break;
	case IOSelection::SDI6:
		str = "SDI 6";
		break;
	case IOSelection::SDI7:
		str = "SDI 7";
		break;
	case IOSelection::SDI8:
		str = "SDI 8";
		break;
	case IOSelection::SDI1_2:
		str = "SDI 1 & 2";
		break;
	case IOSelection::SDI1_2_Squares:
		str = "SDI 1 & 2 (4K Squares)";
		break;
	case IOSelection::SDI3_4:
		str = "SDI 3 & 4";
		break;
	case IOSelection::SDI3_4_Squares:
		str = "SDI 3 & 4 (4K Squares)";
		break;
	case IOSelection::SDI5_6:
		str = "SDI 5 & 6";
		break;
	case IOSelection::SDI7_8:
		str = "SDI 7 & 8";
		break;
	case IOSelection::SDI1__4:
		str = "SDI 1-4";
		break;
	case IOSelection::SDI5__8:
		str = "SDI 5-8";
		break;
	case IOSelection::HDMI1:
		str = "HDMI 1";
		break;
	case IOSelection::HDMI2:
		str = "HDMI 2";
		break;
	case IOSelection::HDMI3:
		str = "HDMI 3";
		break;
	case IOSelection::HDMI4:
		str = "HDMI 4";
		break;
	case IOSelection::HDMIMonitorIn:
		str = "HDMI Monitor In";
		break;
	case IOSelection::HDMIMonitorOut:
		str = "HDMI Monitor Out";
		break;
	case IOSelection::AnalogIn:
		str = "Analog In";
		break;
	case IOSelection::AnalogOut:
		str = "Analog Out";
		break;
	case IOSelection::Invalid:
		str = "Invalid";
		break;
	}

	return str;
}

void IOSelectionToInputSources(IOSelection io, NTV2InputSourceSet &inputSources)
{
	switch (io) {
	case IOSelection::SDI1:
		inputSources.insert(NTV2_INPUTSOURCE_SDI1);
		break;
	case IOSelection::SDI2:
		inputSources.insert(NTV2_INPUTSOURCE_SDI2);
		break;
	case IOSelection::SDI3:
		inputSources.insert(NTV2_INPUTSOURCE_SDI3);
		break;
	case IOSelection::SDI4:
		inputSources.insert(NTV2_INPUTSOURCE_SDI4);
		break;
	case IOSelection::SDI5:
		inputSources.insert(NTV2_INPUTSOURCE_SDI5);
		break;
	case IOSelection::SDI6:
		inputSources.insert(NTV2_INPUTSOURCE_SDI6);
		break;
	case IOSelection::SDI7:
		inputSources.insert(NTV2_INPUTSOURCE_SDI7);
		break;
	case IOSelection::SDI8:
		inputSources.insert(NTV2_INPUTSOURCE_SDI8);
		break;
	case IOSelection::SDI1_2:
		inputSources.insert(NTV2_INPUTSOURCE_SDI1);
		inputSources.insert(NTV2_INPUTSOURCE_SDI2);
		break;
	case IOSelection::SDI1_2_Squares:
		inputSources.insert(NTV2_INPUTSOURCE_SDI1);
		inputSources.insert(NTV2_INPUTSOURCE_SDI2);
		inputSources.insert(NTV2_INPUTSOURCE_SDI3);
		inputSources.insert(NTV2_INPUTSOURCE_SDI4);
		break;
	case IOSelection::SDI3_4:
		inputSources.insert(NTV2_INPUTSOURCE_SDI3);
		inputSources.insert(NTV2_INPUTSOURCE_SDI4);
		break;
	case IOSelection::SDI3_4_Squares:
		inputSources.insert(NTV2_INPUTSOURCE_SDI1);
		inputSources.insert(NTV2_INPUTSOURCE_SDI2);
		inputSources.insert(NTV2_INPUTSOURCE_SDI3);
		inputSources.insert(NTV2_INPUTSOURCE_SDI4);
		break;
	case IOSelection::SDI5_6:
		inputSources.insert(NTV2_INPUTSOURCE_SDI5);
		inputSources.insert(NTV2_INPUTSOURCE_SDI6);
		break;
	case IOSelection::SDI7_8:
		inputSources.insert(NTV2_INPUTSOURCE_SDI7);
		inputSources.insert(NTV2_INPUTSOURCE_SDI8);
		break;
	case IOSelection::SDI1__4:
		inputSources.insert(NTV2_INPUTSOURCE_SDI1);
		inputSources.insert(NTV2_INPUTSOURCE_SDI2);
		inputSources.insert(NTV2_INPUTSOURCE_SDI3);
		inputSources.insert(NTV2_INPUTSOURCE_SDI4);
		break;
	case IOSelection::SDI5__8:
		inputSources.insert(NTV2_INPUTSOURCE_SDI5);
		inputSources.insert(NTV2_INPUTSOURCE_SDI6);
		inputSources.insert(NTV2_INPUTSOURCE_SDI7);
		inputSources.insert(NTV2_INPUTSOURCE_SDI8);
		break;
	case IOSelection::HDMI1:
		inputSources.insert(NTV2_INPUTSOURCE_HDMI1);
		break;
	case IOSelection::HDMI2:
		inputSources.insert(NTV2_INPUTSOURCE_HDMI2);
		break;
	case IOSelection::HDMI3:
		inputSources.insert(NTV2_INPUTSOURCE_HDMI3);
		break;
	case IOSelection::HDMI4:
		inputSources.insert(NTV2_INPUTSOURCE_HDMI4);
		break;
	case IOSelection::HDMIMonitorIn:
		inputSources.insert(NTV2_INPUTSOURCE_HDMI1);
		break;
	case IOSelection::AnalogIn:
		inputSources.insert(NTV2_INPUTSOURCE_ANALOG1);
		break;
	default:
	case IOSelection::HDMIMonitorOut:
	case IOSelection::AnalogOut:
	case IOSelection::Invalid:
		break;
	}
}

void IOSelectionToOutputDests(IOSelection io,
			      NTV2OutputDestinations &outputDests)
{
	switch (io) {
	case IOSelection::SDI1:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI1);
		break;
	case IOSelection::SDI2:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI2);
		break;
	case IOSelection::SDI3:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI3);
		break;
	case IOSelection::SDI4:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI4);
		break;
	case IOSelection::SDI5:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI5);
		break;
	case IOSelection::SDI6:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI6);
		break;
	case IOSelection::SDI7:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI7);
		break;
	case IOSelection::SDI8:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI8);
		break;
	case IOSelection::SDI1_2:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI1);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI2);
		break;
	// Requires 4x framestores and 2x SDI spigots
	case IOSelection::SDI1_2_Squares:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI1);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI2);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI3);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI4);
		break;
	case IOSelection::SDI3_4:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI3);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI4);
		break;
	// Requires 4x framestores and 2x SDI spigots
	case IOSelection::SDI3_4_Squares:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI1);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI2);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI3);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI4);
		break;
	case IOSelection::SDI5_6:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI5);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI6);
		break;
	case IOSelection::SDI7_8:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI7);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI8);
		break;
	case IOSelection::SDI1__4:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI1);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI2);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI3);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI4);
		break;
	case IOSelection::SDI5__8:
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI5);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI6);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI7);
		outputDests.insert(NTV2_OUTPUTDESTINATION_SDI8);
		break;
	case IOSelection::HDMIMonitorOut:
		outputDests.insert(NTV2_OUTPUTDESTINATION_HDMI);
		break;
	case IOSelection::AnalogOut:
		outputDests.insert(NTV2_OUTPUTDESTINATION_ANALOG);
		break;
	default:
	case IOSelection::HDMI1:
	case IOSelection::HDMI2:
	case IOSelection::HDMI3:
	case IOSelection::HDMI4:
	case IOSelection::HDMIMonitorIn:
	case IOSelection::AnalogIn:
	case IOSelection::Invalid:
		break;
	}
}

bool DeviceCanDoIOSelectionIn(NTV2DeviceID id, IOSelection io)
{
	NTV2InputSourceSet inputSources;
	if (io != IOSelection::Invalid) {
		IOSelectionToInputSources(io, inputSources);
		size_t numSrcs = inputSources.size();
		size_t canDo = 0;
		if (numSrcs > 0) {
			for (auto &&inp : inputSources) {
				if (NTV2DeviceCanDoInputSource(id, inp))
					canDo++;
			}

			if (canDo == numSrcs)
				return true;
		}
	}
	return false;
}

bool DeviceCanDoIOSelectionOut(NTV2DeviceID id, IOSelection io)
{
	NTV2OutputDestinations outputDests;
	if (io != IOSelection::Invalid) {
		IOSelectionToOutputDests(io, outputDests);
		size_t numOuts = outputDests.size();
		size_t canDo = 0;
		if (numOuts > 0) {
			for (auto &&out : outputDests) {
				if (NTV2DeviceCanDoOutputDestination(id, out))
					canDo++;
			}

			if (canDo == numOuts)
				return true;
		}
	}
	return false;
}

bool IsSDIOneWireIOSelection(IOSelection io)
{
	bool result = false;
	switch (io) {
	case IOSelection::SDI1:
	case IOSelection::SDI2:
	case IOSelection::SDI3:
	case IOSelection::SDI4:
	case IOSelection::SDI5:
	case IOSelection::SDI6:
	case IOSelection::SDI7:
	case IOSelection::SDI8:
		result = true;
		break;
	default:
		result = false;
	}
	return result;
}

bool IsSDITwoWireIOSelection(IOSelection io)
{
	bool result = false;
	switch (io) {
	case IOSelection::SDI1_2:
	case IOSelection::SDI1_2_Squares:
	case IOSelection::SDI3_4:
	case IOSelection::SDI3_4_Squares:
	case IOSelection::SDI5_6:
	case IOSelection::SDI7_8:
		result = true;
		break;
	default:
		result = false;
	}
	return result;
}

bool IsSDIFourWireIOSelection(IOSelection io)
{
	bool result = false;
	switch (io) {
	case IOSelection::SDI1__4:
	case IOSelection::SDI5__8:
		result = true;
		break;
	default:
		result = false;
	}
	return result;
}

bool IsMonitorOutputSelection(NTV2DeviceID id, IOSelection io)
{
	if (CardCanDoSDIMonitorOutput(id) && io == IOSelection::SDI5)
		return true;

	if (CardCanDoHDMIMonitorOutput(id) && io == IOSelection::HDMIMonitorOut)
		return true;

	return false;
}

std::string MakeCardID(CNTV2Card &card)
{
	std::string cardID;
	if (card.GetSerialNumberString(cardID)) {
		// Try to construct CardID from device ID and serial number...
		cardID = NTV2DeviceIDToString(card.GetDeviceID(), false) + "_" +
			 cardID;
	} else {
		// ...otherwise fall back to the CNTV2DeviceScanner method.
		cardID = CNTV2DeviceScanner::GetDeviceRefName(card);
	}
	return cardID;
}

RasterDefinition GetRasterDefinition(IOSelection io, NTV2VideoFormat vf,
				     NTV2DeviceID deviceID)
{
	RasterDefinition def = RasterDefinition::Unknown;

	if (NTV2_IS_SD_VIDEO_FORMAT(vf)) {
		def = RasterDefinition::SD;
	} else if (NTV2_IS_HD_VIDEO_FORMAT(vf)) {
		def = RasterDefinition::HD;
	} else if (NTV2_IS_QUAD_FRAME_FORMAT(vf)) {
		def = RasterDefinition::UHD_4K;

		/* NOTE(paulh): Special enum for Kona5 Retail & IO4K+ firmwares which route UHD/4K formats
		 * over 1x 6G/12G SDI using an undocumented crosspoint config.
		 */
		if (aja::IsSDIOneWireIOSelection(io) &&
		    aja::IsRetailSDI12G(deviceID))
			def = RasterDefinition::UHD_4K_Retail_12G;
	} else if (NTV2_IS_QUAD_QUAD_FORMAT(vf)) {
		def = RasterDefinition::UHD2_8K;
	} else {
		def = RasterDefinition::Unknown;
	}

	return def;
}

RasterDefinition DetermineRasterDefinition(NTV2VideoFormat vf)
{
	RasterDefinition def = RasterDefinition::Unknown;

	if (NTV2_IS_SD_VIDEO_FORMAT(vf)) {
		def = RasterDefinition::SD;
	} else if (NTV2_IS_HD_VIDEO_FORMAT(vf)) {
		def = RasterDefinition::HD;
	} else if (NTV2_IS_QUAD_FRAME_FORMAT(vf)) {
		def = RasterDefinition::UHD_4K;
	} else if (NTV2_IS_QUAD_QUAD_FORMAT(vf)) {
		def = RasterDefinition::UHD2_8K;
	} else {
		def = RasterDefinition::Unknown;
	}

	return def;
}

// Routing ---------------------------------------------------------------------------------------

static const std::vector<VPIDStandard> kVpidStandardsSupported = {
	VPIDStandard_483_576,
	VPIDStandard_720,
	VPIDStandard_1080,
	VPIDStandard_1080_DualLink,
	VPIDStandard_720_3Ga,
	VPIDStandard_1080_3Ga,
	VPIDStandard_1080_DualLink_3Gb,
	VPIDStandard_720_3Gb,
	VPIDStandard_1080_3Gb,
	VPIDStandard_1080_Dual_3Ga,
	VPIDStandard_1080_Dual_3Gb,
	VPIDStandard_1080_DualLink_3Gb,
	VPIDStandard_2160_DualLink,
	VPIDStandard_2160_QuadLink_3Ga,
	VPIDStandard_2160_QuadDualLink_3Gb,
	VPIDStandard_2160_Single_6Gb,
	VPIDStandard_2160_Single_12Gb,
	VPIDStandard_4320_DualLink_12Gb,
	VPIDStandard_2160_DualLink_12Gb,
	VPIDStandard_4320_QuadLink_12Gb,
};

static const std::vector<SDIWireFormat> kSDIWireFormatsOneWire = {
	SDIWireFormat::SD_ST352,
	SDIWireFormat::HD_720p_ST292,
	SDIWireFormat::HD_1080_ST292,
	SDIWireFormat::HD_720p_ST425_3Ga,
	SDIWireFormat::HD_1080p_ST425_3Ga,
	SDIWireFormat::HD_1080p_ST425_3Gb_DL,
	SDIWireFormat::HD_720p_ST425_3Gb,
	SDIWireFormat::HD_1080p_ST425_3Gb,
	SDIWireFormat::UHD4K_ST2018_6G_Squares_2SI,
	SDIWireFormat::UHD4K_ST2018_12G_Squares_2SI,
};

static const std::vector<SDIWireFormat> kSDIWireFormatsTwoWire = {
	SDIWireFormat::HD_1080_ST372_Dual,
	SDIWireFormat::HD_1080p_ST425_Dual_3Ga,
	SDIWireFormat::HD_1080p_ST425_Dual_3Gb,
	SDIWireFormat::UHD4K_ST425_Dual_3Gb_2SI,
	SDIWireFormat::UHD28K_ST2082_Dual_12G,
	SDIWireFormat::UHD28K_ST2082_RGB_Dual_12G,
};

static const std::vector<SDIWireFormat> kSDIWireFormatsFourWire = {
	SDIWireFormat::UHD4K_ST292_Quad_1_5_Squares,
	SDIWireFormat::UHD4K_ST425_Quad_3Ga_Squares,
	SDIWireFormat::UHD4K_ST425_Quad_3Gb_Squares,
	SDIWireFormat::UHD4K_ST425_Quad_3Ga_2SI,
	SDIWireFormat::UHD4K_ST425_Quad_3Gb_2SI,
	SDIWireFormat::UHD28K_ST2082_Quad_12G,
};

static const std::vector<SDIWireFormat> kSDIWireFormats4KSquares = {
	SDIWireFormat::UHD4K_ST292_Quad_1_5_Squares,
	SDIWireFormat::UHD4K_ST425_Quad_3Ga_Squares,
	SDIWireFormat::UHD4K_ST425_Quad_3Gb_Squares,
	SDIWireFormat::UHD4K_ST2018_6G_Squares_2SI,
	SDIWireFormat::UHD4K_ST2018_12G_Squares_2SI,
};

static const std::vector<SDIWireFormat> kSDIWireFormats4K2SI = {
	SDIWireFormat::UHD4K_ST425_Dual_3Gb_2SI,
	SDIWireFormat::UHD4K_ST425_Quad_3Ga_2SI,
	SDIWireFormat::UHD4K_ST425_Quad_3Gb_2SI,
	// SDIWireFormat::UHD4K_ST2018_6G_Squares_2SI, // TODO(paulh): What are these formats actually, Squares or TSI?
	// SDIWireFormat::UHD4K_ST2018_12G_Squares_2SI,
};

static const std::vector<SDIWireFormat> kSDIWireFormats8K2SI = {
	SDIWireFormat::UHD28K_ST2082_Dual_12G,
	SDIWireFormat::UHD28K_ST2082_RGB_Dual_12G,
	SDIWireFormat::UHD28K_ST2082_Quad_12G,
};

static const std::vector<VPIDStandard> kVpidStandards3gSdi = {
	VPIDStandard_720_3Ga,           VPIDStandard_1080_3Ga,
	VPIDStandard_1080_DualLink_3Gb, VPIDStandard_720_3Gb,
	VPIDStandard_1080_3Gb,          VPIDStandard_1080_Dual_3Ga,
	VPIDStandard_1080_Dual_3Gb,     VPIDStandard_1080_3Ga,
	VPIDStandard_1080_DualLink_3Gb, VPIDStandard_2160_DualLink,
	VPIDStandard_2160_QuadLink_3Ga, VPIDStandard_2160_QuadDualLink_3Gb,
};

static const std::vector<VPIDStandard> kVpidStandards12gSdi = {
	VPIDStandard_2160_Single_6Gb,    VPIDStandard_2160_Single_12Gb,
	VPIDStandard_4320_DualLink_12Gb, VPIDStandard_2160_DualLink_12Gb,
	VPIDStandard_4320_QuadLink_12Gb,
};

static const std::vector<VPIDStandard> kVpidStandardsLevelA = {
	VPIDStandard_720_3Ga,           VPIDStandard_1080_3Ga,
	VPIDStandard_1080_Dual_3Ga,     VPIDStandard_1080_3Ga,
	VPIDStandard_2160_QuadLink_3Ga,
};

static const std::vector<VPIDStandard> kVpidStandardsLevelB = {
	VPIDStandard_1080_DualLink_3Gb,
	VPIDStandard_720_3Gb,
	VPIDStandard_1080_3Gb,
	VPIDStandard_1080_Dual_3Gb,
	VPIDStandard_1080_DualLink_3Gb,
	VPIDStandard_2160_DualLink,
	VPIDStandard_2160_QuadDualLink_3Gb,
};

#define NTV2UTILS_ENUM_CASE_RETURN_STR(enum_name) \
	case (enum_name):                         \
		return #enum_name
std::string RasterDefinitionToString(const RasterDefinition &rd)
{
	std::string str = "";

	switch (rd) {
		NTV2UTILS_ENUM_CASE_RETURN_STR(RasterDefinition::SD);
		NTV2UTILS_ENUM_CASE_RETURN_STR(RasterDefinition::HD);
		NTV2UTILS_ENUM_CASE_RETURN_STR(RasterDefinition::UHD_4K);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			RasterDefinition::UHD_4K_Retail_12G);
		NTV2UTILS_ENUM_CASE_RETURN_STR(RasterDefinition::UHD2_8K);
		NTV2UTILS_ENUM_CASE_RETURN_STR(RasterDefinition::Unknown);
	}

	return str;
}

std::string SDIWireFormatToString(SDIWireFormat svf)
{
	std::string str = "";

	switch (svf) {
		NTV2UTILS_ENUM_CASE_RETURN_STR(SDIWireFormat::SD_ST352);
		NTV2UTILS_ENUM_CASE_RETURN_STR(SDIWireFormat::HD_720p_ST292);
		NTV2UTILS_ENUM_CASE_RETURN_STR(SDIWireFormat::HD_1080_ST292);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::HD_1080_ST372_Dual);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::HD_720p_ST425_3Ga);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::HD_1080p_ST425_3Ga);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::HD_1080p_ST425_3Gb_DL);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::HD_720p_ST425_3Gb);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::HD_1080p_ST425_3Gb);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::HD_1080p_ST425_Dual_3Ga);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::HD_1080p_ST425_Dual_3Gb);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::UHD4K_ST292_Quad_1_5_Squares);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::UHD4K_ST292_Dual_1_5_Squares);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::UHD4K_ST425_Quad_3Ga_Squares);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::UHD4K_ST425_Quad_3Gb_Squares);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::UHD4K_ST425_Dual_3Gb_2SI);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::UHD4K_ST425_Quad_3Ga_2SI);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::UHD4K_ST425_Quad_3Gb_2SI);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::UHD4K_ST2018_6G_Squares_2SI);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::UHD4K_ST2018_12G_Squares_2SI);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::UHD28K_ST2082_Dual_12G);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::UHD28K_ST2082_RGB_Dual_12G);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::UHD28K_ST2082_Quad_12G);
		NTV2UTILS_ENUM_CASE_RETURN_STR(SDIWireFormat::Unknown);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::
				UHD4K_ST2018_6G_Squares_2SI_Kona5_io4KPlus);
		NTV2UTILS_ENUM_CASE_RETURN_STR(
			SDIWireFormat::
				UHD4K_ST2018_12G_Squares_2SI_Kona5_io4KPlus);
	}

	return str;
}

uint32_t GetNumSDIWires(SDIWireFormat sf)
{
	if (IsOneWireSDIWireFormat(sf))
		return 1;
	else if (IsTwoWireSDIWireFormat(sf))
		return 2;
	else if (IsFourWireSDIWireFormat(sf))
		return 4;
	return 0;
}

//
// SDIWireFormat helpers
//
bool IsOneWireSDIWireFormat(SDIWireFormat sf)
{
	return aja::vec_contains<SDIWireFormat>(kSDIWireFormatsOneWire, sf);
}

bool IsTwoWireSDIWireFormat(SDIWireFormat sf)
{
	return aja::vec_contains<SDIWireFormat>(kSDIWireFormatsTwoWire, sf);
}

bool IsFourWireSDIWireFormat(SDIWireFormat sf)
{
	return aja::vec_contains<SDIWireFormat>(kSDIWireFormatsFourWire, sf);
}

bool Is4KSquaresSDIWireFormat(SDIWireFormat sf)
{
	return aja::vec_contains<SDIWireFormat>(kSDIWireFormats4KSquares, sf);
}

bool Is4K2SISDIWireFormat(SDIWireFormat sf)
{
	return aja::vec_contains<SDIWireFormat>(kSDIWireFormats4K2SI, sf);
}

bool Is8K2SISDIWireFormat(SDIWireFormat sf)
{
	return aja::vec_contains<SDIWireFormat>(kSDIWireFormats8K2SI, sf);
}

//
// VPIDStandard helpers
//
bool IsSupportedVPIDStandard(VPIDStandard standard)
{
	return aja::vec_contains<VPIDStandard>(kVpidStandardsSupported,
					       standard);
}

bool Is3GVPIDStandard(VPIDStandard standard)
{
	return aja::vec_contains<VPIDStandard>(kVpidStandards3gSdi, standard);
}

bool Is12GVPIDStandard(VPIDStandard standard)
{
	return aja::vec_contains<VPIDStandard>(kVpidStandards12gSdi, standard);
}

} // aja
