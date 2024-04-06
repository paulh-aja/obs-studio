#include "aja-card-manager.hpp"
#include "aja-common.hpp"
#include "aja-routing.hpp"
#include "aja-widget-io.hpp"

#include <ajabase/common/common.h>
#include <ajantv2/includes/ntv2card.h>
#include <ajantv2/includes/ntv2devicefeatures.h>
#include <ajantv2/includes/ntv2utils.h>

#include <obs-module.h>

namespace aja {

bool RoutingPreset::ToConnections(NTV2XptConnections &cnx,
				  NTV2Channel firstChannel,
				  NTV2Channel firstFramestore) const
{
	return RoutingManager::ParseRouteString(
		routeStringSubstitute(firstChannel, firstFramestore), cnx);
}

bool RoutingPreset::ToWidgetChannelMap(WidgetChannelMap &wcMap,
				       NTV2Channel firstChannel,
				       NTV2Channel firstFramestore) const
{
	NTV2XptConnections cnx;
	if (!ToConnections(cnx, firstChannel, firstFramestore)) {
		return false;
	}

	for (auto c : cnx) {
		WidgetInputSocket inp;
		WidgetInputSocket::GetWidgetInputSocketByXpt(c.first, inp);
		if (wcMap.find(inp.name) == wcMap.end()) {
			wcMap[inp.name] = {inp.channel};
		} else {
			if (std::find(wcMap[inp.name].begin(),
				      wcMap[inp.name].end(),
				      inp.channel) == wcMap[inp.name].end()) {
				wcMap[inp.name].push_back(inp.channel);
			}
		}

		WidgetOutputSocket out;
		WidgetOutputSocket::GetWidgetOutputSocketByXpt(c.second, out);
		if (wcMap.find(out.name) == wcMap.end()) {
			wcMap[out.name] = {out.channel};
		} else {
			if (std::find(wcMap[out.name].begin(),
				      wcMap[out.name].end(),
				      out.channel) == wcMap[out.name].end()) {
				wcMap[out.name].push_back(out.channel);
			}
		}
	}

	return true;
}

bool RoutingPreset::ToChannelList(NTV2ChannelList &chans, NTV2Mode mode,
				  NTV2Channel firstChannel,
				  NTV2Channel firstFramestore) const
{
	if (mode == NTV2_MODE_CAPTURE && IsKona5Io4KPlus6G12GCapture()) {
		chans.push_back(NTV2_CHANNEL1);
		chans.push_back(NTV2_CHANNEL2);
		return true;
	} else if (mode == NTV2_MODE_DISPLAY && IsKona5Io4KPlus6G12GOutput()) {
		chans.push_back(NTV2_CHANNEL3);
		chans.push_back(NTV2_CHANNEL4);
	}

	WidgetChannelMap wcMap;
	if (!ToWidgetChannelMap(wcMap, firstChannel, firstFramestore)) {
		return false;
	}

	for (auto &&w : wcMap) {
		if (w.first == "hdmi" &&
		    (id == AJA_RP_OUT_HDMI_ONE_WIRE_RGB ||
		     id == AJA_RP_OUT_HDMI_ONE_WIRE_YCBCR ||
		     id == HDMI_UHD_4K_LFR_YCbCr_Display ||
		     id == AJA_RP_OUT_HDMI_UHD4K_YCBCR_ONE_WIRE_INTEGRATED_TSI ||
		     id == AJA_RP_OUT_HDMI_UHD4K_RGB_ONE_WIRE_INTEGRATED_TSI)) {
			continue;
		}
		for (auto &&c : w.second) {
			if (std::find(chans.begin(), chans.end(), c) ==
			    chans.end()) {
				chans.emplace_back(std::move(c));
			}
		}
	}

	return true;
}

bool RoutingPreset::IsKona5Io4KPlus6G12GCapture() const
{
	return (id == UHD4K_ST2018_6G_Squares_2SI_RGB_Capture_Kona5_io4KPlus ||
		id == UHD4K_ST2018_12G_Squares_2SI_RGB_Capture_Kona5_io4KPlus ||
		id == UHD4K_ST2018_6G_Squares_2SI_YCbCr_Capture_Kona5_io4KPlus ||
		id == UHD4K_ST2018_12G_Squares_2SI_YCbCr_Capture_Kona5_io4KPlus);
}

bool RoutingPreset::IsKona5Io4KPlus6G12GOutput() const
{
	return (id == UHD4K_ST2018_6G_Squares_2SI_YCbCr_Display_Kona5_io4KPlus ||
		id == UHD4K_ST2018_12G_Squares_2SI_YCbCr_Display_Kona5_io4KPlus);
}

std::string
RoutingPreset::routeStringSubstitute(NTV2Channel firstChannel,
				     NTV2Channel firstFramestore) const
{
	// if (verbatim) {
	// 	firstChannel = NTV2_CHANNEL1;
	// 	firstFramestore = NTV2_CHANNEL1;
	// }

	std::string tmp = route_string;
	ULWord first_framestore_index = GetIndexForNTV2Channel(firstFramestore);
	const std::vector<std::string> fs_associated = {
		kFramebufferNickname, kTSIMuxNickname, kDualLinkInNickname,
		kDualLinkOutNickname}; // widgets associated to framestores
	for (ULWord c = 0; c < NTV2_MAX_NUM_CHANNELS; c++) {
		for (const auto &name : fs_associated) {
			std::string fs_placeholder = std::string(
				name + "[{ch" + aja::to_string(c + 1) + "}]");
			aja::replace(
				tmp, fs_placeholder,
				name + "[" +
					aja::to_string(first_framestore_index) +
					"]");
		}
		first_framestore_index++;
	}
	ULWord first_channel_index = GetIndexForNTV2Channel(firstChannel);
	for (ULWord c = 0; c < NTV2_MAX_NUM_CHANNELS; c++) {
		std::string channel_placeholder =
			std::string("{ch" + aja::to_string(c + 1) + "}");
		aja::replace(tmp, channel_placeholder,
			     aja::to_string(first_channel_index++));
	}

	return tmp;
}

RoutingManager::RoutingManager()
{
	build_preset_table();
}

/*
 * Parse the widget routing shorthand string into a map of input and output NTV2CrosspointIDs.
 * For example "sdi[0][0]->fb[0][0]" is shorthand for connecting the output crosspoint for
 * SDI1/Datastream1 (NTV2_XptSDIIn1) to the input crosspoint for Framestore1/Datastream1 (NTV2_XptFrameBuffer1Input).
 * These routing shorthand strings are found within the RoutingConfig structs in the "routing" sub-directory of the plugin.
 */
bool RoutingManager::ParseRouteString(const std::string &route,
				      NTV2XptConnections &cnx)
{
	blog(LOG_DEBUG,
	     "aja::RoutingManager::ParseRouteString: Input string: %s",
	     route.c_str());

	std::string route_strip(route);
	aja::lower(route_strip);
	aja::replace(route_strip, " ", "");

	if (route_strip.empty()) {
		blog(LOG_DEBUG,
		     "RoutingManager::ParseRouteString: input string is empty!");
		return false;
	}

	/* TODO(paulh): Tally up the lines and tokens and check that they are all parsed OK.
	 * Right now we just return true if ANY tokens were parsed. This is OK _for now_ because
	 * the route strings currently only come from a known set.
	 */
	NTV2StringList lines;

	lines = aja::split(route_strip, ';');
	if (lines.empty())
		lines.push_back(route_strip);

	int32_t parse_ok = 0;
	for (const auto &l : lines) {
		if (l.empty()) {
			blog(LOG_DEBUG,
			     "aja::RoutingManager::ParseRouteString: Empty line!");
			continue;
		}

		blog(LOG_DEBUG,
		     "aja::RoutingManager::ParseRouteString: Line: %s",
		     l.c_str());

		NTV2StringList tokens = aja::split(l, "->");
		if (tokens.empty() || tokens.size() != 2) {
			blog(LOG_DEBUG,
			     "aja::RoutingManager::ParseRouteString: Invalid token count!");
			continue;
		}

		const std::string &left = tokens[0];  // output crosspoint
		const std::string &right = tokens[1]; // input crosspoint
		if (left.empty() || left.length() > 64) {
			blog(LOG_DEBUG,
			     "aja::RoutingManager::ParseRouteString: Invalid Left token!");
			continue;
		}
		if (right.empty() || right.length() > 64) {
			blog(LOG_DEBUG,
			     "aja::RoutingManager::ParseRouteString: Invalid right token!");
			continue;
		}

		blog(LOG_DEBUG,
		     "aja::RoutingManager::ParseRouteString: Left Token: %s -> Right Token: %s",
		     left.c_str(), right.c_str());

		// Parse Output Crosspoint from left token
		int32_t out_chan = 0;
		int32_t out_ds = 0;
		std::string out_name(64, ' ');
		if (std::sscanf(left.c_str(), "%[A-Za-z_0-9][%d][%d]",
				&out_name[0], &out_chan, &out_ds)) {
			out_name = aja::rstrip(out_name).substr(
				0, out_name.find_first_of('\0'));

			WidgetOutputSocket widget_out;
			if (WidgetOutputSocket::Find(out_name,
						     (NTV2Channel)out_chan,
						     out_ds, widget_out)) {
				blog(LOG_DEBUG,
				     "aja::RoutingManager::ParseRouteString: Found NTV2OutputCrosspointID %s",
				     NTV2OutputCrosspointIDToString(
					     widget_out.id)
					     .c_str());

				// Parse Input Crosspoint from right token
				int32_t inp_chan = 0;
				int32_t inp_ds = 0;
				std::string inp_name(64, ' ');
				if (std::sscanf(right.c_str(),
						"%[A-Za-z_0-9][%d][%d]",
						&inp_name[0], &inp_chan,
						&inp_ds)) {
					inp_name = aja::rstrip(inp_name).substr(
						0,
						inp_name.find_first_of('\0'));

					WidgetInputSocket widget_inp;
					if (WidgetInputSocket::Find(
						    inp_name,
						    (NTV2Channel)inp_chan,
						    inp_ds, widget_inp)) {
						blog(LOG_DEBUG,
						     "aja::RoutingManager::ParseRouteString: Found NTV2InputCrosspointID %s",
						     NTV2InputCrosspointIDToString(
							     widget_inp.id)
							     .c_str());

						cnx[widget_inp.id] =
							widget_out.id;
						parse_ok++;
					} else {
						blog(LOG_DEBUG,
						     "aja::RoutingManager::ParseRouteString: NTV2InputCrosspointID not found!");
					}
				}
			} else {
				blog(LOG_DEBUG,
				     "aja::RoutingManager::ParseRouteString: NTV2OutputCrosspointID not found!");
			}
		}
	}

	return parse_ok > 0;
}

bool RoutingManager::FindPreset(const IOConfig &ioConf, RoutingPreset &rp)
{
	return FindFirstPreset(ioConf.ConnectKind(), ioConf.DeviceID(),
			       ioConf.Mode(), ioConf.VideoFormat(),
			       ioConf.PixelFormat(), ioConf.VpidStandard(), rp);
}

bool RoutingManager::ConfigureRouting(const IOConfig &ioConf,
				      const RoutingPreset &rp, CNTV2Card *card,
				      NTV2XptConnections &cnx)
{
	if (!card) {
		return false;
	}

	LogRoutingPreset(rp);

	if (!rp.ToConnections(cnx, ioConf.FirstChannel(),
			      ioConf.FirstFramestore())) {
		return false;
	}
	CNTV2SignalRouter sr;
	if (!sr.ResetFrom(cnx)) {
		return false;
	}
	if (!card->ApplySignalRoute(sr, false)) {
		return false;
	}

	bool isCapture = ioConf.Mode() == NTV2_MODE_CAPTURE;
	bool isRGB = NTV2_IS_FBF_RGB(ioConf.PixelFormat());
	bool is4K = NTV2_IS_4K_VIDEO_FORMAT(ioConf.VideoFormat());
	NTV2ChannelList doneChans;
	WidgetChannelMap wcMap;
	rp.ToWidgetChannelMap(wcMap, ioConf.FirstChannel(),
			      ioConf.FirstFramestore());
	for (auto &e : wcMap) {
		for (auto &channel : e.second) {
			card->SetRegisterWriteMode(
				NTV2_VIDEO_FORMAT_HAS_PROGRESSIVE_PICTURE(
					ioConf.VideoFormat())
					? NTV2_REGWRITE_SYNCTOFRAME
					: NTV2_REGWRITE_SYNCTOFIELD,
				channel);
			if (e.first == kFramebufferNickname) {
				// Framestore settings
				card->EnableChannel(channel);
				card->SetMode(channel, ioConf.Mode());
				card->SetVANCMode(NTV2_VANCMODE_OFF, channel);
				card->SetVideoFormat(ioConf.VideoFormat(),
						     false, false, channel);
				card->SetFrameBufferFormat(
					channel, ioConf.PixelFormat());
				card->SetTsiFrameEnable(rp.flags & kEnable4KTSI,
							channel);
				card->Set4kSquaresEnable(
					rp.flags & kEnable4KSquares, channel);
				card->SetQuadQuadSquaresEnable(
					rp.flags & kEnable8KSquares, channel);
			}
			if (e.first == kSDINickname) {
				// SDI settings
				// See CardEntry note about Kona5 & io4K+ 6G/12G-SDI
				if ((rp.IsKona5Io4KPlus6G12GCapture() &&
				     channel != NTV2_CHANNEL1) ||
				    (rp.IsKona5Io4KPlus6G12GOutput() &&
				     channel != NTV2_CHANNEL3)) {
					continue;
				}
				if (::NTV2DeviceHasBiDirectionalSDI(
					    card->GetDeviceID())) {
					card->SetSDITransmitEnable(channel,
								   !isCapture);
				}
				UWord channelIndex = static_cast<UWord>(
					GetIndexForNTV2Channel(channel));
				if (isCapture) {
					card->SetSDIInLevelBtoLevelAConversion(
						channelIndex,
						rp.flags & kConvert3GIn);
				} else {
					card->SetSDIOut3GEnable(
						channel,
						rp.flags & kEnable3GOut);
					card->SetSDIOut3GbEnable(
						channel,
						rp.flags & kEnable3GbOut);
					card->SetSDIOut6GEnable(
						channel,
						rp.flags & kEnable6GOut);
					card->SetSDIOut12GEnable(
						channel,
						rp.flags & kEnable12GOut);
					card->SetSDIOutLevelAtoLevelBConversion(
						channelIndex,
						rp.flags & kConvert3GOut);
					card->SetSDIOutRGBLevelAConversion(
						channelIndex,
						rp.flags & kConvert3GaRGBOut);
				}
			} else if (e.first == kHDMINickname) {
				// HDMI settings
				card->SetHDMIOutVideoStandard(
					GetNTV2StandardFromVideoFormat(
						ioConf.VideoFormat()));
				card->SetHDMIOutColorSpace(
					isRGB ? NTV2_HDMIColorSpaceRGB
					      : NTV2_HDMIColorSpaceYCbCr);
				card->SetHDMIOutBitDepth(
					NTV2_IS_FBF_10BIT(ioConf.PixelFormat())
						? NTV2_HDMI10Bit
						: NTV2_HDMI8Bit);
				card->SetHDMIOutSampleStructure(
					isRGB ? NTV2_HDMI_RGB : NTV2_HDMI_422);
				card->SetHDMIOutProtocol(NTV2_HDMIProtocolHDMI);
				card->SetHDMIV2Mode(
					is4K ? (isCapture
							? NTV2_HDMI_V2_4K_CAPTURE
							: NTV2_HDMI_V2_4K_PLAYBACK)
					     : NTV2_HDMI_V2_HDSD_BIDIRECTIONAL);
				card->SetHDMIOutTsiIO(
					is4K && NTV2_IS_TSI_FORMAT(
							ioConf.VideoFormat()));
				card->SetHDMIOutDecimateMode(
					ioConf.HdmiDecimate());
				card->SetHDMIOutRange(ioConf.HdmiRange());
			}
		}
	}

	return true;
}

void RoutingManager::StartSourceAudio(const IOConfig &ioConf, CNTV2Card *card)
{
	if (!card)
		return;

	auto inputSrc = ioConf.FirstInputSource();
	auto channel = ioConf.FirstChannel();
	auto audioSys = ioConf.AudioSystem();

	card->WriteAudioSource(0, channel);
	card->SetAudioSystemInputSource(
		audioSys, NTV2InputSourceToAudioSource(inputSrc),
		NTV2InputSourceToEmbeddedAudioInput(inputSrc));

	card->SetNumberAudioChannels(kDefaultAudioChannels, audioSys);
	card->SetAudioRate(ioConf.AudioRate(), audioSys);
	card->SetAudioBufferSize(NTV2_AUDIO_BUFFER_BIG, audioSys);

	// Fix for AJA NTV2 internal bug #11467
	ULWord magicAudioBits = 0;
	if (NTV2_INPUT_SOURCE_IS_HDMI(inputSrc)) {
		switch (inputSrc) {
		default:
		case NTV2_INPUTSOURCE_HDMI1:
			magicAudioBits = 0x00100000;
			break;
		case NTV2_INPUTSOURCE_HDMI2:
			magicAudioBits = 0x00110000;
			break;
		case NTV2_INPUTSOURCE_HDMI3:
			magicAudioBits = 0x00900000;
			break;
		case NTV2_INPUTSOURCE_HDMI4:
			magicAudioBits = 0x00910000;
			break;
		}
	} else if (NTV2_INPUT_SOURCE_IS_ANALOG(inputSrc)) {
		magicAudioBits = 0x00000990;
	} else { // SDI
		magicAudioBits = 0x00000320;
	}

	// TODO(paulh): Ask aja-seanl about these deprecated calls and if they are still needed
	ULWord oldValue = 0;
	if (card->ReadAudioSource(oldValue, channel)) {
		card->WriteAudioSource(oldValue | magicAudioBits, channel);
	}

	for (int a = 0; a < NTV2DeviceGetNumAudioSystems(card->GetDeviceID());
	     a++) {
		card->SetAudioLoopBack(NTV2_AUDIO_LOOPBACK_ON,
				       NTV2AudioSystem(a));
	}

	card->StartAudioInput(audioSys);
	card->SetAudioCaptureEnable(audioSys, true);
}

void RoutingManager::StopSourceAudio(const IOConfig &ioConf, CNTV2Card *card)
{
	if (card) {
		auto audioSys = ioConf.AudioSystem();
		card->SetAudioCaptureEnable(audioSys, false);
		card->StopAudioInput(audioSys);
	}
}

void RoutingManager::ConfigureOutputAudio(const IOConfig &ioConf,
					  CNTV2Card *card)
{
	if (!card)
		return;

	auto deviceID = card->GetDeviceID();
	auto audioSys = ioConf.AudioSystem();
	auto channel = ioConf.FirstChannel();

	card->SetNumberAudioChannels(ioConf.AudioNumChannels(), audioSys);
	card->SetAudioRate(ioConf.AudioRate(), audioSys);
	card->SetAudioBufferSize(NTV2_AUDIO_BUFFER_BIG, audioSys);
	card->SetAudioOutputDelay(audioSys, 0);

	card->SetSDIOutputAudioSystem(channel, audioSys);
	card->SetSDIOutputDS2AudioSystem(channel, audioSys);

	/* NOTE(paulh):
	 * The SDK has a specifies an SDI audio system by Channel rather than by SDI output
	 * and certain devices require setting the SDI audio system to NTV2_CHANNEL1.
	 * i.e.
	 * SDI 1 = NTV2_CHANNEL1
	 * SDI 2 = NTV2_CHANNEL2
	 * ...
	 * SDI 5/Monitor = NTV2_CHANNEL5
	 * etc...
	 *
	 * This fixes AJA internal bugs: 10730, 10986, 16274
	 */
	if (deviceID == DEVICE_ID_IOXT || deviceID == DEVICE_ID_IO4KUFC ||
	    deviceID == DEVICE_ID_IO4KPLUS || deviceID == DEVICE_ID_KONA1 ||
	    deviceID == DEVICE_ID_KONA3G || deviceID == DEVICE_ID_KONA4UFC ||
	    deviceID == DEVICE_ID_KONA5 || deviceID == DEVICE_ID_KONA5_2X4K) {
		// Make sure SDI out 1 (aka Channel 1) is set to the correct sub-system
		card->SetSDIOutputAudioSystem(NTV2_CHANNEL1, audioSys);
		card->SetSDIOutputDS2AudioSystem(NTV2_CHANNEL1, audioSys);
	}

	// make sure that audio is setup for the SDI monitor output on devices that support it
	if (NTV2DeviceCanDoWidget(deviceID, NTV2_WgtSDIMonOut1)) {
		card->SetSDIOutputAudioSystem(NTV2_CHANNEL5, audioSys);
		card->SetSDIOutputDS2AudioSystem(NTV2_CHANNEL5, audioSys);
	}

	card->SetHDMIOutAudioRate(ioConf.AudioRate());
	card->SetHDMIOutAudioFormat(NTV2_AUDIO_FORMAT_LPCM);

	card->SetAudioOutputMonitorSource(NTV2_AudioChannel1_2, channel);
	card->SetAESOutputSource(NTV2_AudioChannel1_4, audioSys,
				 NTV2_AudioChannel1_4);
	card->SetAESOutputSource(NTV2_AudioChannel5_8, audioSys,
				 NTV2_AudioChannel5_8);
	card->SetAESOutputSource(NTV2_AudioChannel9_12, audioSys,
				 NTV2_AudioChannel9_12);
	card->SetAESOutputSource(NTV2_AudioChannel13_16, audioSys,
				 NTV2_AudioChannel13_16);

	// make sure that audio is setup for HDMI output on devices that support it
	if (NTV2DeviceGetNumHDMIVideoOutputs(deviceID) > 0) {
		if (NTV2DeviceCanDoAudioMixer(deviceID)) {
			card->SetAudioMixerInputAudioSystem(
				NTV2_AudioMixerInputMain, audioSys);
			card->SetAudioMixerInputChannelSelect(
				NTV2_AudioMixerInputMain, NTV2_AudioChannel1_2);
			card->SetAudioMixerInputChannelsMute(
				NTV2_AudioMixerInputAux1,
				NTV2AudioChannelsMuteAll);
			card->SetAudioMixerInputChannelsMute(
				NTV2_AudioMixerInputAux2,
				NTV2AudioChannelsMuteAll);
		}

		card->SetHDMIOutAudioChannels(NTV2_HDMIAudio8Channels);
		card->SetHDMIOutAudioSource2Channel(NTV2_AudioChannel1_2,
						    audioSys);
		card->SetHDMIOutAudioSource8Channel(NTV2_AudioChannel1_8,
						    audioSys);
	}

	card->SetAudioLoopBack(NTV2_AUDIO_LOOPBACK_OFF, audioSys);
	card->StopAudioOutput(audioSys);
}

void RoutingManager::LogRoutingPreset(const RoutingPreset &rp)
{
	auto hexStr = [&](uint8_t val) -> std::string {
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(sizeof(uint8_t) * 2)
		   << std::hex << (val | 0);
		return ss.str();
	};

	std::stringstream ss;
	ss << "\nPreset: " << RoutingPresetIDString(rp.id);
	if (rp.kind == ConnectionKind::SDI) {
		std::stringstream vpidSS;
		bool vpidOk = false;
		for (const auto &v : rp.vpid_standards) {
			if (v != VPIDStandard_Unknown) {
				vpidSS << "0x"
				       << hexStr(static_cast<uint8_t>(v))
				       << ' ';
				vpidOk = true;
			}
		}
		if (vpidOk) {
			ss << "\nSupported VPID Standards: " << vpidSS.str();
		}
	}
	ss << "\nMode: " << NTV2ModeToString(rp.mode)
	   << "\nChannels: " << rp.num_channels
	   << "\nFramestores: " << rp.num_framestores;

	blog(LOG_INFO, "[ AJA Crosspoint Routing Preset ]%s", ss.str().c_str());

	if (rp.device_ids.size() > 0) {
		ss.clear();
		for (auto id : rp.device_ids) {
			ss << " - " << NTV2DeviceIDToString(id) << "\n";
		}
		blog(LOG_INFO, "\nCompatible Device IDs: \n%s",
		     ss.str().c_str());
	}
}

void RoutingManager::AddPreset(RoutingPresetID id, const RoutingPreset &preset)
{
	if (mPresetMap.find(id) != mPresetMap.end())
		return;
	mPresetMap.insert(RoutingPresetPair{id, preset});
}

bool RoutingManager::PresetByName(RoutingPresetID id,
				  RoutingPreset &preset) const
{
	if (mPresetMap.find(id) != mPresetMap.end()) {
		preset = mPresetMap.at(id);
		return true;
	}
	return false;
}

bool RoutingManager::FindFirstPreset(ConnectionKind kind, NTV2DeviceID id,
				     NTV2Mode mode, NTV2VideoFormat vf,
				     NTV2PixelFormat pf, VPIDStandard standard,
				     RoutingPreset &preset)
{
	if (NTV2DeviceCanDoVideoFormat(id, vf) &&
	    NTV2DeviceCanDoFrameBufferFormat(id, pf)) {
		const auto &rd = DetermineRasterDefinition(vf);
		bool is_rgb = NTV2_IS_FBF_RGB(pf);
		std::vector<RoutingPresetPair> query;
		for (const auto &p : mPresetMap) {
			if (is_rgb && !p.second.rgb_ok) {
				continue;
			}
			if (p.second.kind == kind && p.second.mode == mode &&
			    aja::vec_contains(p.second.raster_defs, rd) &&
			    aja::vec_contains(p.second.vpid_standards,
					      standard)) {
				query.push_back(p);
			}
		}
		std::vector<RoutingPresetPair> final_query;
		if (query.size() > 1) {
			// Filter out RGB presets if we want YCbCr.
			// If a preset supports RGB and YC, there should be only 1 match.
			// If we want YC but the match list has RGB and YC presets, get rid
			// of the RGB presets.
			for (auto &&q : query) {
				if (!is_rgb && q.second.rgb_ok) {
					continue;
				} else {
					final_query.emplace_back(std::move(q));
				}
			}
		} else {
			for (auto &&q : query) {
				final_query.emplace_back(std::move(q));
			}
		}

		RoutingPresets device_presets;
		RoutingPresets non_device_presets;
		for (auto &&q : final_query) {
			if (q.second.device_ids.size() == 0) {
				non_device_presets.emplace_back(
					std::move(q.second));
			}
			for (const auto &device_id : q.second.device_ids) {
				if (device_id == id) {
					device_presets.emplace_back(
						std::move(q.second));
					break;
				}
			}
		}

		if (device_presets.size() > 0) {
			preset = device_presets.at(0);
			return true;
		}
		if (non_device_presets.size() > 0) {
			preset = non_device_presets.at(0);
			return true;
		}
	}

	return false;
}

void RoutingManager::cache_connections(const NTV2XptConnections &cnx)
{
	mXptCache = cnx;
}
void RoutingManager::clear_cached_connections()
{
	mXptCache.clear();
}
void RoutingManager::apply_cached_connections(CNTV2Card *card)
{
	for (const auto &xpt : mXptCache) {
		card->Connect(xpt.first, xpt.second);
	}
}
void RoutingManager::remove_cached_connections(CNTV2Card *card)
{
	for (const auto &xpt : mXptCache) {
		card->Connect(xpt.first, NTV2_XptBlack);
	}
}

} // namespace aja
