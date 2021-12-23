#include "aja-card-manager.hpp"
#include "aja-common.hpp"
#include "aja-ui-props.hpp"
#include "aja-props.hpp"

#include <ajantv2/includes/ntv2devicescanner.h>
#include <ajantv2/includes/ntv2devicefeatures.h>
#include <ajantv2/includes/ntv2utils.h>

void filter_io_selection_input_list(const std::string &cardID,
				    const std::string &channelOwner,
				    obs_property_t *list)
{
	auto &cardManager = aja::CardManager::Instance();

	auto cardEntry = cardManager.GetCardEntry(cardID);
	if (!cardEntry) {
		blog(LOG_DEBUG,
		     "filter_io_selection_input_list: Card Entry not found for %s",
		     cardID.c_str());
		return;
	}

	NTV2DeviceID deviceID = DEVICE_ID_NOTFOUND;
	CNTV2Card *card = cardEntry->GetCard();
	if (card)
		deviceID = card->GetDeviceID();

	// Gray out the IOSelection list items that are in use by other plugin instances
	for (size_t idx = 0; idx < obs_property_list_item_count(list); idx++) {
		auto io_select = static_cast<IOSelection>(
			obs_property_list_item_int(list, idx));

		if (io_select == IOSelection::Invalid) {
			obs_property_list_item_disable(list, idx, false);
			continue;
		}

		bool enabled = cardEntry->InputSelectionReady(
			io_select, deviceID, channelOwner);
		obs_property_list_item_disable(list, idx, !enabled);
		blog(LOG_DEBUG, "IOSelection %s = %s",
		     aja::IOSelectionToString(io_select).c_str(),
		     enabled ? "enabled" : "disabled");
	}
}

void filter_io_selection_output_list(const std::string &cardID,
				     const std::string &channelOwner,
				     obs_property_t *list)
{
	auto &cardManager = aja::CardManager::Instance();

	auto cardEntry = cardManager.GetCardEntry(cardID);
	if (!cardEntry) {
		blog(LOG_DEBUG,
		     "filter_io_selection_output_list: Card Entry not found for %s",
		     cardID.c_str());
		return;
	}

	NTV2DeviceID deviceID = DEVICE_ID_NOTFOUND;
	CNTV2Card *card = cardEntry->GetCard();
	if (card)
		deviceID = card->GetDeviceID();

	// Gray out the IOSelection list items that are in use by other plugin instances
	for (size_t idx = 0; idx < obs_property_list_item_count(list); idx++) {
		auto io_select = static_cast<IOSelection>(
			obs_property_list_item_int(list, idx));
		if (io_select == IOSelection::Invalid) {
			obs_property_list_item_disable(list, idx, false);
			continue;
		}

		bool enabled = cardEntry->OutputSelectionReady(
			io_select, deviceID, channelOwner);
		obs_property_list_item_disable(list, idx, !enabled);
		blog(LOG_DEBUG, "IOSelection %s = %s",
		     aja::IOSelectionToString(io_select).c_str(),
		     enabled ? "enabled" : "disabled");
	}
}

void populate_io_selection_input_list(const std::string &cardID,
				      const std::string &channelOwner,
				      NTV2DeviceID deviceID,
				      obs_property_t *list)
{
	obs_property_list_clear(list);

	obs_property_list_add_int(list, obs_module_text(kUIPropIOSelect.text),
				  static_cast<long long>(IOSelection::Invalid));

	for (auto i = 0; i < static_cast<int32_t>(IOSelection::NumIOSelections);
	     i++) {
		auto ioSelect = static_cast<IOSelection>(i);
		if (ioSelect == IOSelection::SDI1_2_Squares ||
		    ioSelect == IOSelection::SDI3_4_Squares)
			continue;

		if (aja::DeviceCanDoIOSelectionIn(deviceID, ioSelect)) {
			obs_property_list_add_int(
				list,
				aja::IOSelectionToString(ioSelect).c_str(),
				static_cast<long long>(ioSelect));
		}
	}

	filter_io_selection_input_list(cardID, channelOwner, list);
}

void populate_io_selection_output_list(const std::string &cardID,
				       const std::string &channelOwner,
				       NTV2DeviceID deviceID,
				       obs_property_t *list)
{
	obs_property_list_clear(list);

	obs_property_list_add_int(list, obs_module_text(kUIPropIOSelect.text),
				  static_cast<long long>(IOSelection::Invalid));

	if (deviceID == DEVICE_ID_TTAP_PRO) {
		obs_property_list_add_int(
			list, "SDI & HDMI",
			static_cast<long long>(IOSelection::HDMIMonitorOut));
	} else {
		for (auto i = 0;
		     i < static_cast<int32_t>(IOSelection::NumIOSelections);
		     i++) {
			auto ioSelect = static_cast<IOSelection>(i);

			if (ioSelect == IOSelection::Invalid ||
			    ioSelect == IOSelection::SDI1_2_Squares ||
			    ioSelect == IOSelection::SDI3_4_Squares)
				continue;

			if (aja::DeviceCanDoIOSelectionOut(deviceID,
							   ioSelect)) {
				obs_property_list_add_int(
					list,
					aja::IOSelectionToString(ioSelect)
						.c_str(),
					static_cast<long long>(ioSelect));
			}
		}
	}

	filter_io_selection_output_list(cardID, channelOwner, list);
}

void populate_video_format_list(NTV2DeviceID deviceID, obs_property_t *list,
				NTV2VideoFormat genlockFormat)
{
	VideoFormatList videoFormats = {};
	VideoStandardList orderedStandards = {};
	orderedStandards.push_back(NTV2_STANDARD_525);
	orderedStandards.push_back(NTV2_STANDARD_625);
	if (NTV2DeviceCanDoHDVideo(deviceID)) {
		orderedStandards.push_back(NTV2_STANDARD_720);
		orderedStandards.push_back(NTV2_STANDARD_1080);
		orderedStandards.push_back(NTV2_STANDARD_1080p);
	}
	if (NTV2DeviceCanDo2KVideo(deviceID)) {
		orderedStandards.push_back(NTV2_STANDARD_2K);
		orderedStandards.push_back(NTV2_STANDARD_2Kx1080p);
		orderedStandards.push_back(NTV2_STANDARD_2Kx1080i);
	}
	if (NTV2DeviceCanDo4KVideo(deviceID)) {
		orderedStandards.push_back(NTV2_STANDARD_3840i);
		orderedStandards.push_back(NTV2_STANDARD_3840x2160p);
		orderedStandards.push_back(NTV2_STANDARD_3840HFR);
		orderedStandards.push_back(NTV2_STANDARD_4096i);
		orderedStandards.push_back(NTV2_STANDARD_4096x2160p);
		orderedStandards.push_back(NTV2_STANDARD_4096HFR);
	}

	aja::GetSortedVideoFormats(deviceID, orderedStandards, videoFormats);
	for (const auto &vf : videoFormats) {
		bool addFormat = true;

		// Filter formats by framerate family if specified
		if (genlockFormat != NTV2_FORMAT_UNKNOWN)
			addFormat = IsMultiFormatCompatible(genlockFormat, vf);

		if (addFormat) {
			std::string name = NTV2VideoFormatToString(vf, true);
			obs_property_list_add_int(list, name.c_str(), (int)vf);
		}
	}
}

void populate_pixel_format_list(NTV2DeviceID deviceID, obs_property_t *list)
{
	const NTV2PixelFormat supported_pix_fmts[] = {kDefaultAJAPixelFormat,
						      NTV2_FBF_24BIT_BGR};

	for (auto &&pf : supported_pix_fmts) {
		if (NTV2DeviceCanDoFrameBufferFormat(deviceID, pf)) {
			obs_property_list_add_int(
				list,
				NTV2FrameBufferFormatToString(pf, true).c_str(),
				static_cast<long long>(pf));
		}
	}
}

void populate_sdi_4k_transport_list(obs_property_t *list)
{
	obs_property_list_add_int(
		list,
		aja::SDI4KTransportToString(SDI4KTransport::Squares).c_str(),
		static_cast<long long>(SDI4KTransport::Squares));
	obs_property_list_add_int(
		list,
		aja::SDI4KTransportToString(SDI4KTransport::TwoSampleInterleave)
			.c_str(),
		static_cast<long long>(SDI4KTransport::TwoSampleInterleave));
}

bool aja_video_format_changed(obs_properties_t *props, obs_property_t *list,
			      obs_data_t *settings)
{
	UNUSED_PARAMETER(list);

	auto vid_fmt = static_cast<NTV2VideoFormat>(
		obs_data_get_int(settings, kUIPropVideoFormatSelect.id));

	size_t itemCount = obs_property_list_item_count(list);
	bool itemFound = false;

	for (size_t i = 0; i < itemCount; i++) {
		int itemFormat = obs_property_list_item_int(list, i);
		if (itemFormat == vid_fmt) {
			itemFound = true;
			break;
		}
	}

	if (!itemFound) {
		obs_property_list_insert_int(list, 0, "", vid_fmt);
		obs_property_list_item_disable(list, 0, true);
		return true;
	}

	obs_property_t *sdi_4k_trx =
		obs_properties_get(props, kUIPropSDI4KTransport.id);

	obs_property_set_visible(sdi_4k_trx, NTV2_IS_4K_VIDEO_FORMAT(vid_fmt));

	return true;
}

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
	}
	else {
		def = RasterDefinition::Unknown;
	}

	return def;
}

} // aja
