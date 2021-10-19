#include "aja-ui-props.hpp"
#include "aja-card-manager.hpp"
#include "aja-common.hpp"
#include "aja-enums.hpp"

#include <ajantv2/includes/ntv2card.h>

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

	// Gray out the aja::IOSelection list items that are in use by other plugin instances
	for (size_t idx = 0; idx < obs_property_list_item_count(list); idx++) {
		auto io_select = static_cast<aja::IOSelection>(
			obs_property_list_item_int(list, idx));

		if (io_select == aja::IOSelection::Invalid) {
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
		auto io_select = static_cast<aja::IOSelection>(
			obs_property_list_item_int(list, idx));
		if (io_select == aja::IOSelection::Invalid) {
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

	obs_property_list_add_int(
		list, obs_module_text(kUIPropIOSelect.text),
		static_cast<long long>(aja::IOSelection::Invalid));

	for (auto i = 0;
	     i < static_cast<int32_t>(aja::IOSelection::NumIOSelections); i++) {
		auto ioSelect = static_cast<aja::IOSelection>(i);
		if (ioSelect == aja::IOSelection::SDI1_2_Squares ||
		    ioSelect == aja::IOSelection::SDI3_4_Squares)
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

	obs_property_list_add_int(
		list, obs_module_text(kUIPropIOSelect.text),
		static_cast<long long>(aja::IOSelection::Invalid));

	if (deviceID == DEVICE_ID_TTAP_PRO) {
		obs_property_list_add_int(
			list, "SDI & HDMI",
			static_cast<long long>(
				aja::IOSelection::HDMIMonitorOut));
	} else {
		for (auto i = 0; i < static_cast<int32_t>(
					     aja::IOSelection::NumIOSelections);
		     i++) {
			auto ioSelect = static_cast<aja::IOSelection>(i);

			if (ioSelect == aja::IOSelection::Invalid ||
			    ioSelect == aja::IOSelection::SDI1_2_Squares ||
			    ioSelect == aja::IOSelection::SDI3_4_Squares)
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
		aja::SDI4KTransportToString(aja::SDI4KTransport::Squares)
			.c_str(),
		static_cast<long long>(aja::SDI4KTransport::Squares));
	obs_property_list_add_int(
		list,
		aja::SDI4KTransportToString(
			aja::SDI4KTransport::TwoSampleInterleave)
			.c_str(),
		static_cast<long long>(
			aja::SDI4KTransport::TwoSampleInterleave));
}

bool aja_video_format_changed(obs_properties_t *props, obs_property_t *list,
			      obs_data_t *settings)
{
	UNUSED_PARAMETER(list);

	auto vid_fmt = static_cast<NTV2VideoFormat>(
		obs_data_get_int(settings, kUIPropVideoFormatSelect.id));

	obs_property_t *sdi_4k_trx =
		obs_properties_get(props, kUIPropSDI4KTransport.id);

	obs_property_set_visible(sdi_4k_trx, NTV2_IS_4K_VIDEO_FORMAT(vid_fmt));

	return true;
}
