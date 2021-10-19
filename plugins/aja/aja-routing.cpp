#include "aja-card-manager.hpp"
#include "aja-common.hpp"
#include "aja-routing.hpp"

#include "routing/presets.hpp"

#include <ajabase/common/common.h>
#include <ajantv2/includes/ntv2card.h>
#include <ajantv2/includes/ntv2devicefeatures.h>

#include <obs-module.h>

namespace aja {

// Determine the appropriate SDIWireFormat based on the specified device ID and VPID specification.
bool Routing::DetermineSDIWireFormat(NTV2DeviceID deviceID, VPIDSpec spec,
				     SDIWireFormat &swf)
{
	if (deviceID == DEVICE_ID_KONA5 || deviceID == DEVICE_ID_IO4KPLUS) {
		static const std::vector<VPIDStandard> kRetail6GVpidStandards = {
			VPIDStandard_2160_Single_6Gb,
			VPIDStandard_1080_Single_6Gb,
			VPIDStandard_1080_AFR_Single_6Gb,
		};
		static const std::vector<VPIDStandard> kRetail12GVpidStandards =
			{VPIDStandard_2160_Single_12Gb,
			 VPIDStandard_1080_10_12_AFR_Single_12Gb};
		if (spec.first == RasterDefinition::UHD_4K &&
		    aja::vec_contains<VPIDStandard>(kRetail6GVpidStandards,
						    spec.second)) {
			swf = SDIWireFormat::
				UHD4K_ST2018_6G_Squares_2SI_Kona5_io4KPlus;
			return true;
		} else if (spec.first == RasterDefinition::UHD_4K &&
			   aja::vec_contains<VPIDStandard>(
				   kRetail12GVpidStandards, spec.second)) {
			swf = SDIWireFormat::
				UHD4K_ST2018_12G_Squares_2SI_Kona5_io4KPlus;
			return true;
		} else {
			if (kSDIWireFormats.find(spec) !=
			    kSDIWireFormats.end()) {
				swf = kSDIWireFormats.at(spec);
				return true;
			}
		}
	} else {
		if (kSDIWireFormats.find(spec) != kSDIWireFormats.end()) {
			swf = kSDIWireFormats.at(spec);
			return true;
		}
	}

	return false;
}

void Routing::StartSourceAudio(const SourceProps &props, CNTV2Card *card)
{
	if (!card)
		return;

	auto inputSrc = props.inputSource;
	auto channel = props.Channel();
	auto audioSys = props.AudioSystem();

	card->WriteAudioSource(0, channel);
	card->SetAudioSystemInputSource(
		audioSys, NTV2InputSourceToAudioSource(inputSrc),
		NTV2InputSourceToEmbeddedAudioInput(inputSrc));

	card->SetNumberAudioChannels(props.audioNumChannels, audioSys);
	card->SetAudioRate(props.AudioRate(), audioSys);
	card->SetAudioBufferSize(NTV2_AUDIO_BUFFER_BIG, audioSys);

	// Fix for AJA NTV2 internal bug #11467
	ULWord magicAudioBits = 0;
	if (NTV2_INPUT_SOURCE_IS_HDMI(inputSrc)) {
		magicAudioBits = 0x00100000;
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
		card->SetAudioLoopBack(NTV2_AUDIO_LOOPBACK_OFF,
				       NTV2AudioSystem(a));
	}

	card->StartAudioInput(audioSys);
	card->SetAudioCaptureEnable(audioSys, true);
}

void Routing::StopSourceAudio(const SourceProps &props, CNTV2Card *card)
{
	if (card) {
		auto audioSys = props.AudioSystem();
		card->SetAudioCaptureEnable(audioSys, false);
		card->StopAudioInput(audioSys);
	}
}

// Guess an SDIWireFormat based on specified Video Format, IOSelection, 4K Transport and device ID.
SDIWireFormat GuessSDIWireFormat(NTV2VideoFormat vf, IOSelection io,
				 SDI4KTransport t4k,
				 NTV2DeviceID device_id = DEVICE_ID_NOTFOUND)
{
	auto rd = GetRasterDefinition(io, vf, device_id);
	auto fg = GetNTV2FrameGeometryFromVideoFormat(vf);

	SDIWireFormat swf = SDIWireFormat::Unknown;
	if (rd == RasterDefinition::SD) {
		swf = SDIWireFormat::SD_ST352;
	} else if (rd == RasterDefinition::HD) {
		if (fg == NTV2_FG_1280x720) {
			swf = SDIWireFormat::HD_720p_ST292;
		} else if (fg == NTV2_FG_1920x1080 || fg == NTV2_FG_2048x1080) {
			swf = SDIWireFormat::HD_1080_ST292;
		}
	} else if (rd == RasterDefinition::UHD_4K) {
		if (t4k == SDI4KTransport::Squares) {
			if (aja::IsSDIFourWireIOSelection(io)) {
				swf = SDIWireFormat::UHD4K_ST292_Quad_1_5_Squares;
			} else if (aja::IsSDITwoWireIOSelection(io)) {
				if (t4k == SDI4KTransport::Squares) {
					swf = SDIWireFormat::
						UHD4K_ST292_Dual_1_5_Squares;
				} else {
					swf = SDIWireFormat::
						UHD4K_ST425_Dual_3Gb_2SI;
				}
			}
		} else if (t4k == SDI4KTransport::TwoSampleInterleave) {
			if (aja::IsSDIOneWireIOSelection(io)) {
				if (NTV2_IS_4K_HFR_VIDEO_FORMAT(vf)) {
					if (aja::IsRetailSDI12G(device_id)) {
						swf = SDIWireFormat::
							UHD4K_ST2018_12G_Squares_2SI_Kona5_io4KPlus;
					} else {
						swf = SDIWireFormat::
							UHD4K_ST2018_12G_Squares_2SI;
					}
				} else {
					if (aja::IsRetailSDI12G(device_id)) {
						swf = SDIWireFormat::
							UHD4K_ST2018_6G_Squares_2SI_Kona5_io4KPlus;
					} else {
						swf = SDIWireFormat::
							UHD4K_ST2018_6G_Squares_2SI;
					}
				}
			} else if (aja::IsSDITwoWireIOSelection(io)) {
				swf = SDIWireFormat::UHD4K_ST425_Dual_3Gb_2SI;
			} else if (aja::IsSDIFourWireIOSelection(io)) {
				swf = SDIWireFormat::UHD4K_ST425_Quad_3Gb_2SI;
			}
		}
	}
	return swf;
}

bool Routing::ConfigureSourceRoute(const SourceProps &props, NTV2Mode mode,
				   CNTV2Card *card)
{
	if (!card)
		return false;

	auto deviceID = props.deviceID;

	NTV2VideoFormat vf = props.videoFormat;
	if (NTV2_VIDEO_FORMAT_IS_B(props.videoFormat)) {
		vf = aja::GetLevelAFormatForLevelBFormat(props.videoFormat);
	}

	NTV2InputSourceSet inputSources;
	aja::IOSelectionToInputSources(props.ioSelect, inputSources);
	if (inputSources.empty()) {
		blog(LOG_DEBUG,
		     "No Input Sources specified to configure routing!");
		return false;
	}
	auto init_src = *inputSources.begin();
	auto init_channel = NTV2InputSourceToChannel(init_src);

	RoutingConfigurator rcfg;
	RoutingPreset rp;
	if (NTV2_INPUT_SOURCE_IS_SDI(init_src)) {
		SDIWireFormat swf = SDIWireFormat::Unknown;
		auto standard = VPIDStandard_Unknown;
		auto vpidList = props.vpids;
		if (vpidList.size() > 0)
			standard = vpidList.at(0).Standard();

		if (standard != VPIDStandard_Unknown) {
			// Determine SDI format based on raster "definition" and VPID byte 1 value (AKA SMPTE standard)
			auto rasterDef = GetRasterDefinition(props.ioSelect, vf,
							     props.deviceID);
			VPIDSpec vpidSpec = std::make_pair(rasterDef, standard);
			DetermineSDIWireFormat(deviceID, vpidSpec, swf);
		} else {
			// Best guess SDI format from incoming video format if no VPIDs detected
			swf = GuessSDIWireFormat(vf, props.ioSelect,
						 props.sdi4kTransport,
						 props.deviceID);
		}

		if (swf == SDIWireFormat::Unknown) {
			blog(LOG_DEBUG, "Could not determine SDI Wire Format!");
			return false;
		}

		if (!rcfg.FindFirstPreset(kConnectionKindSDI, props.deviceID,
					  NTV2_MODE_CAPTURE, props.videoFormat,
					  props.pixelFormat, standard, rp)) {
			blog(LOG_DEBUG,
			     "Could not find RoutingConfig for SDI Wire Format!");
			return false;
		}
	} else if (NTV2_INPUT_SOURCE_IS_HDMI(init_src)) {
		HDMIWireFormat hwf = HDMIWireFormat::Unknown;

		if (NTV2_IS_FBF_RGB(props.pixelFormat)) {
			if (NTV2_IS_HD_VIDEO_FORMAT(vf))
				hwf = HDMIWireFormat::HD_RGB_LFR;
		} else {
			if (NTV2_IS_HD_VIDEO_FORMAT(vf))
				hwf = HDMIWireFormat::HD_YCBCR_LFR;
			else if (NTV2_IS_4K_VIDEO_FORMAT(vf))
				hwf = HDMIWireFormat::UHD_4K_YCBCR_LFR;
		}

		if (!rcfg.FindFirstPreset(kConnectionKindHDMI, props.deviceID,
					  NTV2_MODE_CAPTURE, props.videoFormat,
					  props.pixelFormat,
					  VPIDStandard_Unknown, rp)) {
			blog(LOG_DEBUG,
			     "Could not find RoutingConfig for HDMI Wire Format!");
			return false;
		}
	}

	// Substitute channel placeholders for actual indices
	std::string route_string = rp.route_string;
	ULWord start_channel_index = GetIndexForNTV2Channel(init_channel);
	for (ULWord c = 0; c < 8; c++) {
		std::string channel_placeholder =
			std::string("{ch" + aja::to_string(c + 1) + "}");
		route_string =
			aja::replace(route_string, channel_placeholder,
				     aja::to_string(start_channel_index++));
	}

	NTV2XptConnections cnx;
	ParseRouteString(route_string, cnx);

	card->ApplySignalRoute(cnx, false);

	// Apply SDI widget settings
	start_channel_index = GetIndexForNTV2Channel(init_channel);
	for (uint32_t i = (uint32_t)start_channel_index;
	     i < (start_channel_index + rp.num_channels); i++) {
		NTV2Channel channel = GetNTV2ChannelForIndex(i);
		if (::NTV2DeviceHasBiDirectionalSDI(deviceID)) {
			card->SetSDITransmitEnable(channel,
						   mode == NTV2_MODE_DISPLAY);
		}
		card->SetSDIOut3GEnable(channel, rp.flags & kEnable3GOut);
		card->SetSDIOut3GbEnable(channel, rp.flags & kEnable3GbOut);
		card->SetSDIOut6GEnable(channel, rp.flags & kEnable6GOut);
		card->SetSDIOut12GEnable(channel, rp.flags & kEnable12GOut);
		card->SetSDIInLevelBtoLevelAConversion((UWord)i,
						       rp.flags & kConvert3GIn);
		card->SetSDIOutLevelAtoLevelBConversion(
			(UWord)i, rp.flags & kConvert3GOut);
		card->SetSDIOutRGBLevelAConversion(
			(UWord)i, rp.flags & kConvert3GaRGBOut);
	}

	// Apply Framestore settings
	for (uint32_t i = (uint32_t)start_channel_index;
	     i < (start_channel_index + rp.num_framestores); i++) {
		NTV2Channel channel = GetNTV2ChannelForIndex(i);
		card->EnableChannel(channel);
		card->SetMode(channel, mode);
		card->SetVANCMode(NTV2_VANCMODE_OFF, channel);
		card->SetVideoFormat(vf, false, false, channel);
		card->SetFrameBufferFormat(channel, props.pixelFormat);
		card->SetTsiFrameEnable(rp.flags & kEnable4KTSI, channel);
		card->Set4kSquaresEnable(rp.flags & kEnable4KQuads, channel);
		card->SetQuadQuadSquaresEnable(rp.flags & kEnable8KQuads,
					       channel);
	}

	return true;
}

bool Routing::ConfigureOutputRoute(const OutputProps &props, NTV2Mode mode,
				   CNTV2Card *card)
{
	if (!card)
		return false;

	auto deviceID = props.deviceID;

	NTV2OutputDestinations outputDests;
	aja::IOSelectionToOutputDests(props.ioSelect, outputDests);
	if (outputDests.empty()) {
		blog(LOG_DEBUG,
		     "No Output Destinations specified to configure routing!");
		return false;
	}

	auto init_dest = *outputDests.begin();
	auto init_channel = NTV2OutputDestinationToChannel(init_dest);

	RoutingConfigurator rcfg;
	RoutingPreset rp;
	if (NTV2_OUTPUT_DEST_IS_SDI(init_dest)) {
		SDIWireFormat swf = GuessSDIWireFormat(props.videoFormat,
						       props.ioSelect,
						       props.sdi4kTransport,
						       props.deviceID);

		if (swf == SDIWireFormat::Unknown) {
			blog(LOG_DEBUG, "Could not determine SDI Wire Format!");
			return false;
		}

		if (!rcfg.FindFirstPreset(kConnectionKindSDI, props.deviceID,
					  NTV2_MODE_DISPLAY, props.videoFormat,
					  props.pixelFormat,
					  VPIDStandard_Unknown, rp)) {
			blog(LOG_DEBUG,
			     "Could not find RoutingConfig for SDI Wire Format!");
			return false;
		}
	} else if (NTV2_OUTPUT_DEST_IS_HDMI(init_dest)) {
		HDMIWireFormat hwf = HDMIWireFormat::Unknown;

		// special case devices...
		if (props.deviceID == DEVICE_ID_TTAP_PRO) {
			hwf = HDMIWireFormat::TTAP_PRO;
		} else {
			// ...all other devices.
			if (NTV2_IS_FBF_RGB(props.pixelFormat)) {
				if (NTV2_IS_HD_VIDEO_FORMAT(props.videoFormat))
					hwf = HDMIWireFormat::HD_RGB_LFR;
			} else {
				if (NTV2_IS_HD_VIDEO_FORMAT(
					    props.videoFormat)) {
					hwf = HDMIWireFormat::HD_YCBCR_LFR;
				} else if (NTV2_IS_4K_VIDEO_FORMAT(
						   props.videoFormat)) {
					hwf = HDMIWireFormat::UHD_4K_YCBCR_LFR;
				}
			}
		}

		if (!rcfg.FindFirstPreset(kConnectionKindHDMI, props.deviceID,
					  NTV2_MODE_DISPLAY, props.videoFormat,
					  props.pixelFormat,
					  VPIDStandard_Unknown, rp)) {
			blog(LOG_DEBUG,
			     "Could not find RoutingConfig for HDMI Wire Format!");
			return false;
		}
	}

	std::string route_string = rp.route_string;

	// Replace framestore channel placeholders
	ULWord start_framestore_index = initial_framestore_output_index(
		deviceID, props.ioSelect, init_channel);
	for (ULWord c = 0; c < NTV2_MAX_NUM_CHANNELS; c++) {
		std::string fs_channel_placeholder =
			std::string("fb[{ch" + aja::to_string(c + 1) + "}]");
		route_string = aja::replace(
			route_string, fs_channel_placeholder,
			"fb[" + aja::to_string(start_framestore_index++) + "]");
	}

	// Replace other channel placeholders
	ULWord start_channel_index = GetIndexForNTV2Channel(init_channel);
	for (ULWord c = 0; c < NTV2_MAX_NUM_CHANNELS; c++) {
		std::string channel_placeholder =
			std::string("{ch" + aja::to_string(c + 1) + "}");
		route_string =
			aja::replace(route_string, channel_placeholder,
				     aja::to_string(start_channel_index++));
	}

	NTV2XptConnections cnx;
	ParseRouteString(route_string, cnx);
	card->ApplySignalRoute(cnx, false);

	// Apply SDI widget settings
	if (props.ioSelect != IOSelection::HDMIMonitorOut) {
		start_channel_index = GetIndexForNTV2Channel(init_channel);
		for (uint32_t i = (uint32_t)start_channel_index;
		     i < (start_channel_index + rp.num_channels); i++) {
			NTV2Channel channel = GetNTV2ChannelForIndex(i);
			if (::NTV2DeviceHasBiDirectionalSDI(deviceID)) {
				card->SetSDITransmitEnable(
					channel, mode == NTV2_MODE_DISPLAY);
			}
			card->SetSDIOut3GEnable(channel,
						rp.flags & kEnable3GOut);
			card->SetSDIOut3GbEnable(channel,
						 rp.flags & kEnable3GbOut);
			card->SetSDIOut6GEnable(channel,
						rp.flags & kEnable6GOut);
			card->SetSDIOut12GEnable(channel,
						 rp.flags & kEnable12GOut);
			card->SetSDIInLevelBtoLevelAConversion(
				(UWord)i, rp.flags & kConvert3GIn);
			card->SetSDIOutLevelAtoLevelBConversion(
				(UWord)i, rp.flags & kConvert3GOut);
			card->SetSDIOutRGBLevelAConversion(
				(UWord)i, rp.flags & kConvert3GaRGBOut);
		}
	}

	// Apply Framestore settings
	start_framestore_index = initial_framestore_output_index(
		deviceID, props.ioSelect, init_channel);
	for (uint32_t i = (uint32_t)start_framestore_index;
	     i < (start_framestore_index + rp.num_framestores); i++) {
		NTV2Channel channel = GetNTV2ChannelForIndex(i);
		card->EnableChannel(channel);
		card->SetMode(channel, mode);
		card->SetVANCMode(NTV2_VANCMODE_OFF, channel);
		card->SetVideoFormat(props.videoFormat, false, false, channel);
		card->SetFrameBufferFormat(channel, props.pixelFormat);
		card->SetTsiFrameEnable(rp.flags & kEnable4KTSI, channel);
		card->Set4kSquaresEnable(rp.flags & kEnable4KQuads, channel);
		card->SetQuadQuadSquaresEnable(rp.flags & kEnable8KQuads,
					       channel);
	}

	return true;
}

ULWord Routing::initial_framestore_output_index(NTV2DeviceID deviceID,
						IOSelection io,
						NTV2Channel init_channel)
{
	if (deviceID == DEVICE_ID_TTAP_PRO) {
		return 0;
	} else if (deviceID == DEVICE_ID_KONA1) {
		return 1;
	} else if (deviceID == DEVICE_ID_IO4K ||
		   deviceID == DEVICE_ID_IO4KPLUS) {
		// SDI Monitor output uses framestore 4
		if (io == IOSelection::SDI5)
			return 3;
	}

	// HDMI Monitor output uses framestore 4
	if (io == IOSelection::HDMIMonitorOut) {
		return 3;
	}

	return GetIndexForNTV2Channel(init_channel);
}

// Output Routing
void Routing::ConfigureOutputAudio(const OutputProps &props, CNTV2Card *card)
{
	if (!card)
		return;

	auto deviceID = card->GetDeviceID();
	auto audioSys = props.AudioSystem();
	auto channel = props.Channel();

	card->SetNumberAudioChannels(props.audioNumChannels, audioSys);
	card->SetAudioRate(props.AudioRate(), audioSys);
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

	card->SetHDMIOutAudioRate(props.AudioRate());
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

} // aja
