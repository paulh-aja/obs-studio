#include "aja-props.hpp"
#include "aja-common.hpp"

#include <ajantv2/includes/ntv2devicefeatures.h>
#include <ajantv2/includes/ntv2utils.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////

IOConfig::IOConfig()
	: mDeviceID(DEVICE_ID_NOTFOUND),
	  mVideoFormat(NTV2_FORMAT_UNKNOWN),
	  mPixelFormat(NTV2_FBF_INVALID),
	  mMode(NTV2_MODE_INVALID),
	  mIoSelection(IOSelection::Invalid),
	  mSdiTransport(SDITransport::Unknown),
	  mSdi4kTransport(SDITransport4K::Unknown),
	  mHdmiWireFormat(HDMIWireFormat::Unknown),
	  mVpids(),
	  mAudioNumChannels(kDefaultAudioChannels),
	  mAudioSampleSize(kDefaultAudioSampleSize),
	  mAudioSampleRate(kDefaultAudioSampleRate),
	  mFirstChannelOverride(NTV2_CHANNEL_INVALID),
	  mFirstFramestoreOverride(NTV2_CHANNEL_INVALID),
	  mAutoDetect(false)
{
	init();
}

IOConfig::IOConfig(NTV2DeviceID id, NTV2Mode mode, IOSelection io,
		   NTV2VideoFormat vf, NTV2PixelFormat pf)
	: mDeviceID(id),
	  mVideoFormat(vf),
	  mPixelFormat(pf),
	  mMode(mode),
	  mIoSelection(io),
	  mSdiTransport(SDITransport::Unknown),
	  mSdi4kTransport(SDITransport4K::Unknown),
	  mHdmiWireFormat(HDMIWireFormat::Unknown),
	  mVpids(),
	  mAudioNumChannels(kDefaultAudioChannels),
	  mAudioSampleSize(kDefaultAudioSampleSize),
	  mAudioSampleRate(kDefaultAudioSampleRate),
	  mFirstChannelOverride(NTV2_CHANNEL_INVALID),
	  mFirstFramestoreOverride(NTV2_CHANNEL_INVALID),
	  mAutoDetect(false)
{
	init();
}

void IOConfig::init()
{
	if (Mode() == NTV2_MODE_CAPTURE) {
		aja::IOSelectionToInputSources(IoSelection(), mInputSources);
	} else if (Mode() == NTV2_MODE_DISPLAY) {
		aja::IOSelectionToOutputDests(IoSelection(), mOutputDests);
	}

	if (ConnectKind() == ConnectionKind::HDMI) {
		if (NTV2_IS_FBF_RGB(PixelFormat())) {
			if (NTV2_IS_HD_VIDEO_FORMAT(VideoFormat())) {
				mHdmiWireFormat = HDMIWireFormat::SD_HD_RGB;
			} else if (NTV2_IS_4K_VIDEO_FORMAT(VideoFormat())) {
				mHdmiWireFormat = HDMIWireFormat::UHD_4K_RGB;
			}
		} else {
			if (NTV2_IS_HD_VIDEO_FORMAT(VideoFormat())) {
				mHdmiWireFormat = HDMIWireFormat::SD_HD_YCBCR;
			} else if (NTV2_IS_4K_VIDEO_FORMAT(VideoFormat())) {
				mHdmiWireFormat = HDMIWireFormat::UHD_4K_YCBCR;
			}
		}
	}
}

// Invent a VPIDStandard based on the current IOConfig if we don't have one from the SDI signal.
VPIDStandard IOConfig::contriveVpidStandard() const
{
	VPIDStandard vpid = VPIDStandard_Unknown;
	auto io = IoSelection();
	auto rd = RasterDef();
	auto standard = Standard();
	auto trx = SdiTransport();
	auto t4k = SdiTransport4K();
	bool is_rgb = IsRgb();
	bool is_hfr = IsHighFramerate();
	if (rd == RasterDefinition::SD) {
		vpid = VPIDStandard_483_576;
	} else if (rd == RasterDefinition::HD) {
		vpid = VPIDStandard_1080;
		if (aja::IsSDIOneWireIOSelection(io)) {
			if (is_rgb) {
				if (standard == NTV2_STANDARD_720) {
					if (trx == SDITransport::SingleLink) {
						vpid = VPIDStandard_720;
					} else if (trx ==
						   SDITransport::SDI3Ga) {
						vpid = VPIDStandard_720_3Ga;
					} else if (trx ==
						   SDITransport::SDI3Gb) {
						vpid = VPIDStandard_720_3Gb;
					}
				} else if (aja::IsStandard1080p(standard)) {
					if (trx == SDITransport::SingleLink) {
						vpid = VPIDStandard_1080;
					} else if (trx ==
						   SDITransport::SDI3Ga) {
						vpid = VPIDStandard_1080_3Ga;
					} else if (trx ==
						   SDITransport::SDI3Gb) {
						vpid = VPIDStandard_1080_3Gb;
					}
				}
			} else {
				if (standard == NTV2_STANDARD_720) {
					vpid = VPIDStandard_720;
				} else if (aja::IsStandard1080i(standard)) {
					vpid = VPIDStandard_1080;
				} else if (aja::IsStandard1080p(standard) &&
					   trx == SDITransport::SDI3Ga) {
					vpid = VPIDStandard_1080_3Ga;
				} else if (aja::IsStandard1080p(standard) &&
					   trx == SDITransport::SDI3Gb) {
					vpid = VPIDStandard_1080_3Gb;
				}
			}
		} else if (aja::IsSDITwoWireIOSelection(io)) {
			if (is_rgb) {
				if (standard == NTV2_STANDARD_720) {
					if (trx == SDITransport::SDI3Ga)
						vpid = VPIDStandard_720_3Ga;
					else if (trx == SDITransport::SDI3Gb)
						vpid = VPIDStandard_720_3Gb;
				} else if (aja::IsStandard1080p(standard)) {
					if (trx == SDITransport::HDDualLink) {
						vpid = VPIDStandard_1080_DualLink;
					} else if (trx == SDITransport::SDI3Ga)
						vpid = VPIDStandard_1080_Dual_3Ga;
					else if (trx == SDITransport::SDI3Gb)
						vpid = VPIDStandard_1080_Dual_3Gb;
				}
			} else {
				if (aja::IsStandard1080p(standard) &&
				    trx == SDITransport::HDDualLink) {
					vpid = VPIDStandard_1080_DualLink;
				}
			}
		}
	} else if (rd == RasterDefinition::UHD_4K) {
		if (aja::IsSDIOneWireIOSelection(io)) {
			if (is_rgb) {
				if (trx == SDITransport::SDI6G) {
					vpid = VPIDStandard_2160_DualLink;
				} else if (trx == SDITransport::SDI12G) {
					vpid = VPIDStandard_2160_DualLink_12Gb;
				}
			} else {
				// YCbCr
				if (trx == SDITransport::SDI6G) {
					vpid = VPIDStandard_2160_Single_6Gb;
				} else if (trx == SDITransport::SDI12G) {
					vpid = VPIDStandard_2160_Single_12Gb;
				} else {
					vpid = VPIDStandard_2160_Single_6Gb; // fallback
				}
			}
		} else if (aja::IsSDITwoWireIOSelection(io)) {
			if (is_rgb) {
			} else {
				// YCbCr
				if (t4k == SDITransport4K::Squares) {
					vpid = VPIDStandard_1080;
				} else if (t4k ==
					   SDITransport4K::TwoSampleInterleave) {
					if (is_hfr &&
					    trx == SDITransport::SDI3Ga) {
						vpid = VPIDStandard_2160_QuadLink_3Ga;
					} else if (is_hfr &&
						   trx == SDITransport::SDI3Gb) {
						vpid = VPIDStandard_2160_QuadDualLink_3Gb;
					} else {
						vpid = VPIDStandard_2160_DualLink;
					}
				}
			}
		} else if (aja::IsSDIFourWireIOSelection(io)) {
			if (is_rgb) {
				if (t4k == SDITransport4K::Squares) {
					if (trx == SDITransport::SDI3Ga) {
						vpid = VPIDStandard_1080_3Ga;
					} else if (trx ==
						   SDITransport::SDI3Gb) {
						vpid = VPIDStandard_1080_DualLink_3Gb;
					}
				}
			} else {
				// YCbCr
				if (t4k == SDITransport4K::Squares) {
					if (trx == SDITransport::SDI3Ga) {
						vpid = VPIDStandard_1080_3Ga;
					} else if (trx ==
						   SDITransport::SDI3Gb) {
						vpid = VPIDStandard_1080_DualLink_3Gb;
					} else {
						vpid = VPIDStandard_1080;
					}
				} else if (t4k ==
					   SDITransport4K::TwoSampleInterleave) {
					if (trx == SDITransport::SDI3Ga) {
						vpid = VPIDStandard_2160_QuadLink_3Ga;
					} else if (trx ==
						   SDITransport::SDI3Gb) {
						vpid = VPIDStandard_2160_QuadDualLink_3Gb;
					}
				}
			}
		}
	}

	return vpid;
}

bool IOConfig::operator==(const IOConfig &other)
{
	return (mDeviceID == other.mDeviceID &&
		mIoSelection == other.mIoSelection &&
		mVideoFormat == other.mVideoFormat &&
		mPixelFormat == other.mPixelFormat &&
		mAutoDetect == other.mAutoDetect &&
		mSdiTransport == other.mSdiTransport &&
		mSdi4kTransport == other.mSdi4kTransport &&
		mAudioNumChannels == other.mAudioNumChannels &&
		mAudioSampleSize == other.mAudioSampleSize &&
		mAudioSampleRate == other.mAudioSampleRate);
}

bool IOConfig::operator!=(const IOConfig &other)
{
	return !operator==(other);
}

uint32_t IOConfig::NumChannels() const
{
	return static_cast<uint32_t>(Channels().size());
}

uint32_t IOConfig::NumFramestores() const
{
	return static_cast<uint32_t>(Framestores().size());
}

NTV2ChannelList IOConfig::IoSelectionToChannels(IOSelection io, NTV2Mode mode,
						NTV2VideoFormat vf,
						NTV2DeviceID id)
{
	NTV2ChannelList channels;
	if (mode == NTV2_MODE_CAPTURE) {
		NTV2InputSourceSet inputSources;
		aja::IOSelectionToInputSources(io, inputSources);
		for (const auto &src : inputSources) {
			channels.push_back(NTV2InputSourceToChannel(src));
		}
	} else if (mode == NTV2_MODE_DISPLAY) {
		NTV2OutputDestinations outputDests;
		aja::IOSelectionToOutputDests(io, outputDests);
		if (!outputDests.empty()) {
			auto firstOutputDest = *outputDests.begin();
			if (NTV2_OUTPUT_DEST_IS_HDMI(firstOutputDest) &&
			    aja::CardCanDoHDMIMonitorOutput(id)) {
				if (id == DEVICE_ID_KONAX) {
					channels.push_back(NTV2_CHANNEL2);
				} else {
					if (NTV2_IS_4K_VIDEO_FORMAT(vf) &&
					    (id == DEVICE_ID_IO4K ||
					     id == DEVICE_ID_IO4KPLUS ||
					     id == DEVICE_ID_KONA5)) {
						// These firmware require the use of two framestores for 4K HDMI output.
						channels.push_back(
							NTV2_CHANNEL3);
					}
					channels.push_back(NTV2_CHANNEL4);
				}
			} else {
				if (id == DEVICE_ID_TTAP_PRO) {
					channels.push_back(NTV2_CHANNEL1);
				} else if (id == DEVICE_ID_KONA1) {
					channels.push_back(NTV2_CHANNEL2);
				} else if ((id == DEVICE_ID_IO4K ||
					    id == DEVICE_ID_IO4KPLUS) &&
					   firstOutputDest ==
						   NTV2_OUTPUTDESTINATION_SDI5) {
					// Force framestore 4 for SDI Monitor (AKA SDI5) output on io4K series, as there is no framestore 5.
					channels.push_back(NTV2_CHANNEL4);
				} else {
					for (const auto &dst : outputDests) {
						channels.push_back(
							NTV2OutputDestinationToChannel(
								dst));
					}
				}
			}
		}
	}

	return channels;
}

NTV2ChannelList IOConfig::IoSelectionToFramestores(IOSelection io,
						   NTV2Mode mode,
						   NTV2VideoFormat vf,
						   NTV2DeviceID id)
{
	NTV2ChannelList framestoreChannels =
		IoSelectionToChannels(io, mode, vf, id);
	if (mode == NTV2_MODE_CAPTURE) {
		if (id == DEVICE_ID_KONAHDMI && NTV2_IS_4K_VIDEO_FORMAT(vf)) {
			if (io == IOSelection::HDMI1) {
				framestoreChannels.push_back(NTV2_CHANNEL2);
			} else if (io == IOSelection::HDMI2) {
				// Kona HDMI requires framestores 3 & 4 for HDMI2
				framestoreChannels.clear();
				framestoreChannels.push_back(NTV2_CHANNEL3);
				framestoreChannels.push_back(NTV2_CHANNEL4);
			}
		}
	} else if (mode == NTV2_MODE_DISPLAY) {
		NTV2OutputDestinations outputDests;
		aja::IOSelectionToOutputDests(io, outputDests);
		if (!outputDests.empty()) {
			auto firstOutputDest = *outputDests.begin();
			if (NTV2_OUTPUT_DEST_IS_HDMI(firstOutputDest) &&
			    aja::CardCanDoHDMIMonitorOutput(id)) {
				if (id == DEVICE_ID_KONAX) {
					framestoreChannels.push_back(
						NTV2_CHANNEL2);
				} else {
					if (NTV2_IS_4K_VIDEO_FORMAT(vf) &&
					    (id == DEVICE_ID_IO4K ||
					     id == DEVICE_ID_IO4KPLUS ||
					     id == DEVICE_ID_KONA5)) {
						// These firmware require the use of two framestores for 4K HDMI output.
						framestoreChannels.push_back(
							NTV2_CHANNEL3);
					}
					framestoreChannels.push_back(
						NTV2_CHANNEL4);
				}
			}
		}
	}

	return framestoreChannels;
}

NTV2ChannelList IOConfig::Channels() const
{
	return IoSelectionToChannels(IoSelection(), Mode(), VideoFormat(),
				     DeviceID());
}

NTV2ChannelList IOConfig::Framestores() const
{
	return IoSelectionToFramestores(IoSelection(), Mode(), VideoFormat(),
					DeviceID());
}

NTV2InputSourceSet IOConfig::InputSources() const
{
	return mInputSources;
}
NTV2OutputDestinations IOConfig::OutputDestinations() const
{
	return mOutputDests;
}

uint32_t IOConfig::FirstChannelIndex() const
{
	return GetIndexForNTV2Channel(FirstChannel());
}
uint32_t IOConfig::FirstFramestoreIndex() const
{
	return GetIndexForNTV2Channel(FirstFramestore());
}
NTV2InputSource IOConfig::FirstInputSource() const
{
	if (!mInputSources.empty()) {
		return *mInputSources.begin();
	} else {
		return NTV2_INPUTSOURCE_INVALID;
	}
}
NTV2OutputDest IOConfig::FirstOutputDest() const
{
	if (!mOutputDests.empty()) {
		return *mOutputDests.begin();
	} else {
		return NTV2_OUTPUTDESTINATION_INVALID;
	}
}
NTV2Channel IOConfig::FirstChannel() const
{
	if (mFirstChannelOverride == NTV2_CHANNEL_INVALID) {
		auto channels = Channels();
		if (!channels.empty()) {
			return *channels.begin();
		}
	}
	return mFirstChannelOverride;
}
NTV2Channel IOConfig::FirstFramestore() const
{
	if (mFirstFramestoreOverride == NTV2_CHANNEL_INVALID) {
		auto framestores = Framestores();
		if (!framestores.empty()) {
			return *framestores.begin();
		}
	}
	return mFirstFramestoreOverride;
}

NTV2FormatDesc IOConfig::FormatDesc() const
{
	return NTV2FormatDesc(mVideoFormat, mPixelFormat);
}

NTV2AudioSystem IOConfig::AudioSystem() const
{
	return NTV2ChannelToAudioSystem(FirstChannel());
}
NTV2AudioRate IOConfig::AudioRate() const
{
	switch (mAudioSampleRate) {
	case 96000:
		return NTV2_AUDIO_96K;
	case 192000:
		return NTV2_AUDIO_192K;
	}
	return NTV2_AUDIO_48K;
}
size_t IOConfig::AudioSize() const
{
	return mAudioNumChannels * mAudioSampleSize;
}
audio_format IOConfig::AudioFormat() const
{
	// NTV2 audio data is always 24-bit (upper 8-bits not used)
	return AUDIO_FORMAT_32BIT;
}
speaker_layout IOConfig::SpeakerLayout() const
{
	// NTV2 devices always operate with at least 8 audio channels physically enabled.
	// This function allows specifying the number of channels of audio to transfer to/from OBS.
	switch (mAudioNumChannels) {
	case 1:
		return SPEAKERS_MONO;
	case 2:
		return SPEAKERS_STEREO;
	case 3:
		return SPEAKERS_2POINT1;
	case 4:
		return SPEAKERS_4POINT0;
	case 5:
		return SPEAKERS_4POINT1;
	case 6:
		return SPEAKERS_5POINT1;
	}
	return SPEAKERS_7POINT1;
}

NTV2Standard IOConfig::Standard() const
{
	return GetNTV2StandardFromVideoFormat(VideoFormat());
}
VPIDStandard IOConfig::VpidStandard() const
{
	VPIDStandard standard = VPIDStandard_Unknown;
	if (ConnectKind() == ConnectionKind::SDI) {
		NTV2Mode mode = Mode();
		if (mode == NTV2_MODE_CAPTURE && AutoDetect() &&
		    !mVpids.empty()) {
			standard = mVpids.begin()->Standard();
		} else if (mode == NTV2_MODE_DISPLAY) {
			standard = contriveVpidStandard();
		}
	}
	return standard;
}
RasterDefinition IOConfig::RasterDef() const
{
	return aja::DetermineRasterDefinition(VideoFormat());
}
HDMIWireFormat IOConfig::HdmiWireFormat() const
{
	return mHdmiWireFormat;
}
ConnectionKind IOConfig::ConnectKind() const
{
	return aja::IOSelectionToConnectionKind(IoSelection());
}
NTV2VideoFormat IOConfig::VideoFormat() const
{
	return mVideoFormat;
}
NTV2PixelFormat IOConfig::PixelFormat() const
{
	return mPixelFormat;
}
NTV2DeviceID IOConfig::DeviceID() const
{
	return mDeviceID;
}
NTV2Mode IOConfig::Mode() const
{
	return mMode;
}
IOSelection IOConfig::IoSelection() const
{
	return mIoSelection;
}
SDITransport IOConfig::SdiTransport() const
{
	return mSdiTransport;
}
SDITransport4K IOConfig::SdiTransport4K() const
{
	return mSdi4kTransport;
}
uint32_t IOConfig::AudioNumChannels() const
{
	return mAudioNumChannels;
}
uint32_t IOConfig::AudioSampleSize() const
{
	return mAudioSampleSize;
}
uint32_t IOConfig::AudioSampleRate() const
{
	return mAudioSampleRate;
}
bool IOConfig::IsRgb() const
{
	return NTV2_IS_FBF_RGB(PixelFormat());
}
bool IOConfig::IsHighFramerate() const
{
	return NTV2_IS_HIGH_NTV2FrameRate(
		GetNTV2FrameRateFromVideoFormat(VideoFormat()));
}
bool IOConfig::AutoDetect() const
{
	return mAutoDetect;
}
void IOConfig::SetVideoFormat(NTV2VideoFormat vf)
{
	mVideoFormat = vf;
	init();
}
void IOConfig::SetPixelFormat(NTV2PixelFormat pf)
{
	mPixelFormat = pf;
	init();
}
void IOConfig::SetDeviceID(NTV2DeviceID id)
{
	mDeviceID = id;
}
void IOConfig::SetMode(NTV2Mode mode)
{
	mMode = mode;
	init();
}
void IOConfig::SetIoSelection(IOSelection io)
{
	mIoSelection = io;
	init();
}
void IOConfig::SetSdiTransport(SDITransport trx)
{
	mSdiTransport = trx;
}
void IOConfig::SetSdiTransport4K(SDITransport4K t4k)
{
	mSdi4kTransport = t4k;
}
void IOConfig::SetAudioNumChannels(uint32_t num)
{
	mAudioNumChannels = num;
}
void IOConfig::SetAudioSampleSize(uint32_t size)
{
	mAudioSampleSize = size;
}
void IOConfig::SetAudioSampleRate(uint32_t rate)
{
	mAudioSampleRate = rate;
}
void IOConfig::SetAutoDetect(bool enable)
{
	mAutoDetect = enable;
}
void IOConfig::SetVpidData(const VPIDDataList &vpids)
{
	mVpids = vpids;
}
void IOConfig::SetFirstChannel(NTV2Channel ch)
{
	mFirstChannelOverride = ch;
}
void IOConfig::SetFirstFramestore(NTV2Channel ch)
{
	mFirstFramestoreOverride = ch;
}
void IOConfig::ClearVpidData()
{
	mVpids.clear();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// AJASource Properties
// SourceProps::SourceProps()
// 	: deviceID{DEVICE_ID_NOTFOUND},
// 	  ioSelect{IOSelection::Invalid},
// 	  //   inputSource{NTV2_INPUTSOURCE_INVALID},
// 	  videoFormat{NTV2_FORMAT_UNKNOWN},
// 	  pixelFormat{NTV2_FBF_INVALID},
// 	  sdiTransport{SDITransport::SingleLink},
// 	  sdi4kTransport{SDITransport4K::TwoSampleInterleave},
// 	  audioNumChannels{kDefaultAudioChannels},
// 	  audioSampleSize{kDefaultAudioSampleSize},
// 	  audioSampleRate{kDefaultAudioSampleRate},
// 	  vpids{},
// 	  autoDetect{false},
// 	  deactivateWhileNotShowing{false},
// 	  swapFrontCenterLFE{false}
// {
// }

// SourceProps::SourceProps(NTV2DeviceID devID)
// 	: deviceID{devID},
// 	  ioSelect{IOSelection::Invalid},
// 	  //   inputSource{NTV2_INPUTSOURCE_INVALID},
// 	  videoFormat{NTV2_FORMAT_UNKNOWN},
// 	  pixelFormat{NTV2_FBF_INVALID},
// 	  sdiTransport{SDITransport::SingleLink},
// 	  sdi4kTransport{SDITransport4K::TwoSampleInterleave},
// 	  audioNumChannels{kDefaultAudioChannels},
// 	  audioSampleSize{kDefaultAudioSampleSize},
// 	  audioSampleRate{kDefaultAudioSampleRate},
// 	  vpids{},
// 	  autoDetect{false},
// 	  deactivateWhileNotShowing{false},
// 	  swapFrontCenterLFE{false}
// {
// }

// SourceProps::SourceProps(const SourceProps &props)
// {
// 	deviceID = props.deviceID;
// 	ioSelect = props.ioSelect;
// 	// inputSource = props.inputSource;
// 	videoFormat = props.videoFormat;
// 	pixelFormat = props.pixelFormat;
// 	sdiTransport = props.sdiTransport;
// 	sdi4kTransport = props.sdi4kTransport;
// 	audioNumChannels = props.audioNumChannels;
// 	audioSampleSize = props.audioSampleSize;
// 	audioSampleRate = props.audioSampleRate;
// 	vpids = props.vpids;
// 	autoDetect = props.autoDetect;
// 	deactivateWhileNotShowing = props.deactivateWhileNotShowing;
// 	swapFrontCenterLFE = props.swapFrontCenterLFE;
// }

// SourceProps::SourceProps(SourceProps &&props)
// {
// 	deviceID = props.deviceID;
// 	ioSelect = props.ioSelect;
// 	// inputSource = props.inputSource;
// 	videoFormat = props.videoFormat;
// 	pixelFormat = props.pixelFormat;
// 	sdiTransport = props.sdiTransport;
// 	sdi4kTransport = props.sdi4kTransport;
// 	audioNumChannels = props.audioNumChannels;
// 	audioSampleSize = props.audioSampleSize;
// 	audioSampleRate = props.audioSampleRate;
// 	vpids = props.vpids;
// 	autoDetect = props.autoDetect;
// 	deactivateWhileNotShowing = props.deactivateWhileNotShowing;
// 	swapFrontCenterLFE = props.swapFrontCenterLFE;
// }

// void SourceProps::operator=(const SourceProps &props)
// {
// 	deviceID = props.deviceID;
// 	ioSelect = props.ioSelect;
// 	// inputSource = props.inputSource;
// 	videoFormat = props.videoFormat;
// 	pixelFormat = props.pixelFormat;
// 	sdiTransport = props.sdiTransport;
// 	sdi4kTransport = props.sdi4kTransport;
// 	audioNumChannels = props.audioNumChannels;
// 	audioSampleSize = props.audioSampleSize;
// 	audioSampleRate = props.audioSampleRate;
// 	vpids = props.vpids;
// 	autoDetect = props.autoDetect;
// 	deactivateWhileNotShowing = props.deactivateWhileNotShowing;
// 	swapFrontCenterLFE = props.swapFrontCenterLFE;
// }

// void SourceProps::operator=(SourceProps &&props)
// {
// 	deviceID = props.deviceID;
// 	ioSelect = props.ioSelect;
// 	// inputSource = props.inputSource;
// 	videoFormat = props.videoFormat;
// 	pixelFormat = props.pixelFormat;
// 	sdiTransport = props.sdiTransport;
// 	sdi4kTransport = props.sdi4kTransport;
// 	audioNumChannels = props.audioNumChannels;
// 	audioSampleSize = props.audioSampleSize;
// 	audioSampleRate = props.audioSampleRate;
// 	vpids = props.vpids;
// 	autoDetect = props.autoDetect;
// 	deactivateWhileNotShowing = props.deactivateWhileNotShowing;
// 	swapFrontCenterLFE = props.swapFrontCenterLFE;
// }

// bool SourceProps::operator==(const SourceProps &props)
// {
// 	return (deviceID == props.deviceID && ioSelect == props.ioSelect &&
// 		// inputSource == props.inputSource &&
// 		videoFormat == props.videoFormat &&
// 		pixelFormat == props.pixelFormat &&
// 		// vpid == props.vpid &&
// 		autoDetect == props.autoDetect &&
// 		sdiTransport == props.sdiTransport &&
// 		sdi4kTransport == props.sdi4kTransport &&
// 		audioNumChannels == props.audioNumChannels &&
// 		audioSampleSize == props.audioSampleSize &&
// 		audioSampleRate == props.audioSampleRate &&
// 		deactivateWhileNotShowing == props.deactivateWhileNotShowing &&
// 		swapFrontCenterLFE == props.swapFrontCenterLFE);
// }

// bool SourceProps::operator!=(const SourceProps &props)
// {
// 	return !operator==(props);
// }

// NTV2InputSource SourceProps::InitialInputSource() const
// {
// 	auto inputSources = InputSources();
// 	if (!inputSources.empty()) {
// 		return *inputSources.begin();
// 	}
// 	return NTV2_INPUTSOURCE_INVALID;
// }

// NTV2InputSourceSet SourceProps::InputSources() const
// {
// 	NTV2InputSourceSet inputSources;
// 	aja::IOSelectionToInputSources(ioSelect, inputSources);
// 	return inputSources;
// }

// NTV2Channel SourceProps::Channel() const
// {
// 	return NTV2InputSourceToChannel(InitialInputSource());
// }

// NTV2Channel SourceProps::Framestore() const
// {
// 	if (deviceID == DEVICE_ID_KONAHDMI && ioSelect == IOSelection::HDMI2 &&
// 	    NTV2_IS_4K_VIDEO_FORMAT(videoFormat)) {
// 		return NTV2_CHANNEL3;
// 	}
// 	return Channel();
// }

// NTV2AudioSystem SourceProps::AudioSystem() const
// {
// 	return NTV2ChannelToAudioSystem(Channel());
// }

// NTV2AudioRate SourceProps::AudioRate() const
// {
// 	NTV2AudioRate rate = NTV2_AUDIO_48K;
// 	switch (audioSampleRate) {
// 	default:
// 	case 48000:
// 		rate = NTV2_AUDIO_48K;
// 		break;
// 	case 96000:
// 		rate = NTV2_AUDIO_96K;
// 		break;
// 	case 192000:
// 		rate = NTV2_AUDIO_192K;
// 		break;
// 	}

// 	return rate;
// }

// // Size in bytes of N channels of audio
// size_t SourceProps::AudioSize() const
// {
// 	return audioNumChannels * audioSampleSize;
// }

// audio_format SourceProps::AudioFormat() const
// {
// 	// NTV2 is always 32-bit PCM
// 	return AUDIO_FORMAT_32BIT;
// }

// speaker_layout SourceProps::SpeakerLayout() const
// {
// 	if (audioNumChannels == 1)
// 		return SPEAKERS_MONO;
// 	else if (audioNumChannels == 2)
// 		return SPEAKERS_STEREO;
// 	else if (audioNumChannels == 3)
// 		return SPEAKERS_2POINT1;
// 	else if (audioNumChannels == 4)
// 		return SPEAKERS_4POINT0;
// 	else if (audioNumChannels == 5)
// 		return SPEAKERS_4POINT1;
// 	else if (audioNumChannels == 6)
// 		return SPEAKERS_5POINT1;
// 	// NTV2 card is always set to at least 8ch
// 	return SPEAKERS_7POINT1;
// }

// //
// // AJAOutput Properties
// //
// OutputProps::OutputProps(NTV2DeviceID devID)
// 	: deviceID{devID},
// 	  ioSelect{IOSelection::Invalid},
// 	  outputDest{NTV2_OUTPUTDESTINATION_ANALOG},
// 	  videoFormat{NTV2_FORMAT_UNKNOWN},
// 	  pixelFormat{NTV2_FBF_INVALID},
// 	  sdi4kTransport{SDITransport4K::TwoSampleInterleave},
// 	  audioNumChannels{8},
// 	  audioSampleSize{kDefaultAudioSampleSize},
// 	  audioSampleRate{kDefaultAudioSampleRate}
// {
// }

// OutputProps::OutputProps(OutputProps &&props)
// {
// 	deviceID = props.deviceID;
// 	ioSelect = props.ioSelect;
// 	outputDest = props.outputDest;
// 	videoFormat = props.videoFormat;
// 	pixelFormat = props.pixelFormat;
// 	sdiTransport = props.sdiTransport;
// 	sdi4kTransport = props.sdi4kTransport;
// 	audioNumChannels = props.audioNumChannels;
// 	audioSampleSize = props.audioSampleSize;
// 	audioSampleRate = props.audioSampleRate;
// }

// OutputProps::OutputProps(const OutputProps &props)
// {
// 	deviceID = props.deviceID;
// 	ioSelect = props.ioSelect;
// 	outputDest = props.outputDest;
// 	videoFormat = props.videoFormat;
// 	pixelFormat = props.pixelFormat;
// 	sdiTransport = props.sdiTransport;
// 	sdi4kTransport = props.sdi4kTransport;
// 	audioNumChannels = props.audioNumChannels;
// 	audioSampleSize = props.audioSampleSize;
// 	audioSampleRate = props.audioSampleRate;
// }

// void OutputProps::operator=(const OutputProps &props)
// {
// 	deviceID = props.deviceID;
// 	ioSelect = props.ioSelect;
// 	outputDest = props.outputDest;
// 	videoFormat = props.videoFormat;
// 	pixelFormat = props.pixelFormat;
// 	sdiTransport = props.sdiTransport;
// 	sdi4kTransport = props.sdi4kTransport;
// 	audioNumChannels = props.audioNumChannels;
// 	audioSampleSize = props.audioSampleSize;
// 	audioSampleRate = props.audioSampleRate;
// }

// void OutputProps::operator=(OutputProps &&props)
// {
// 	deviceID = props.deviceID;
// 	ioSelect = props.ioSelect;
// 	outputDest = props.outputDest;
// 	videoFormat = props.videoFormat;
// 	pixelFormat = props.pixelFormat;
// 	sdiTransport = props.sdiTransport;
// 	sdi4kTransport = props.sdi4kTransport;
// 	audioNumChannels = props.audioNumChannels;
// 	audioSampleSize = props.audioSampleSize;
// 	audioSampleRate = props.audioSampleRate;
// }

// bool OutputProps::operator==(const OutputProps &props)
// {
// 	return (deviceID == props.deviceID && ioSelect == props.ioSelect &&
// 		// outputDest == props.outputDest &&
// 		videoFormat == props.videoFormat &&
// 		pixelFormat == props.pixelFormat &&
// 		sdiTransport == props.sdiTransport &&
// 		sdi4kTransport == props.sdi4kTransport &&
// 		audioNumChannels == props.audioNumChannels &&
// 		audioSampleSize == props.audioSampleSize &&
// 		audioSampleRate == props.audioSampleRate);
// }

// bool OutputProps::operator!=(const OutputProps &props)
// {
// 	return !operator==(props);
// }

// NTV2FormatDesc OutputProps::FormatDesc()
// {
// 	return NTV2FormatDesc(videoFormat, pixelFormat, NTV2_VANCMODE_OFF);
// }

// NTV2Channel OutputProps::Channel() const
// {
// 	// Output Channel Special Cases
// 	// KONA1 -- Has 2 framestores but only 1 bi-directional SDI widget
// 	if (deviceID == DEVICE_ID_KONA1) {
// 		return NTV2_CHANNEL2;
// 	} else if ((deviceID == DEVICE_ID_IO4K ||
// 		    deviceID == DEVICE_ID_IO4KPLUS) &&
// 		   outputDest == NTV2_OUTPUTDESTINATION_SDI5) {
// 		// IO4K/IO4K+ SDI Monitor - Use framestore 4 but SDI5
// 		return NTV2_CHANNEL4;
// 	}

// 	if (NTV2_OUTPUT_DEST_IS_HDMI(outputDest)) {
// 		if (aja::CardCanDoHDMIMonitorOutput(deviceID) &&
// 		    NTV2_IS_4K_VIDEO_FORMAT(videoFormat))
// 			return NTV2_CHANNEL3;
// 		return static_cast<NTV2Channel>(
// 			NTV2DeviceGetNumFrameStores(deviceID) - 1);
// 	}

// 	return NTV2OutputDestinationToChannel(outputDest);
// }

// NTV2Channel OutputProps::Framestore() const
// {
// 	if (deviceID == DEVICE_ID_TTAP_PRO) {
// 		return NTV2_CHANNEL1;
// 	} else if (deviceID == DEVICE_ID_KONA1) {
// 		return NTV2_CHANNEL2;
// 	} else if (deviceID == DEVICE_ID_IO4K ||
// 		   deviceID == DEVICE_ID_IO4KPLUS) {
// 		// SDI Monitor output uses framestore 4
// 		if (ioSelect == IOSelection::SDI5)
// 			return NTV2_CHANNEL4;
// 	}
// 	// HDMI Monitor output uses framestore 4
// 	if (ioSelect == IOSelection::HDMIMonitorOut) {
// 		if (deviceID == DEVICE_ID_KONA5_8K)
// 			return NTV2_CHANNEL4;
// 		if (NTV2_IS_4K_VIDEO_FORMAT(videoFormat))
// 			return NTV2_CHANNEL3;
// 		else
// 			return NTV2_CHANNEL4;
// 	}
// 	return NTV2OutputDestinationToChannel(outputDest);
// }

// NTV2AudioSystem OutputProps::AudioSystem() const
// {
// 	return NTV2ChannelToAudioSystem(Channel());
// }

// NTV2AudioRate OutputProps::AudioRate() const
// {
// 	NTV2AudioRate rate = NTV2_AUDIO_48K;
// 	switch (audioSampleRate) {
// 	default:
// 	case 48000:
// 		rate = NTV2_AUDIO_48K;
// 		break;
// 	case 96000:
// 		rate = NTV2_AUDIO_96K;
// 		break;
// 	case 192000:
// 		rate = NTV2_AUDIO_192K;
// 		break;
// 	}

// 	return rate;
// }

// // Size in bytes of N channels of audio
// size_t OutputProps::AudioSize() const
// {
// 	return audioNumChannels * audioSampleSize;
// }

// audio_format OutputProps::AudioFormat() const
// {
// 	// NTV2 is always 32-bit PCM
// 	return AUDIO_FORMAT_32BIT;
// }

// speaker_layout OutputProps::SpeakerLayout() const
// {
// 	if (audioNumChannels == 2)
// 		return SPEAKERS_STEREO;
// 	// NTV2 is always at least 8ch on modern boards
// 	return SPEAKERS_7POINT1;
// }
