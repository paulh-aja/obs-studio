#pragma once

#include "aja-enums.hpp"
#include "aja-vpid-data.hpp"

#include <media-io/audio-io.h>

#include <ajantv2/includes/ntv2enums.h>
#include <ajantv2/includes/ntv2formatdescriptor.h>

#include <map>
#include <string>
#include <vector>

class IOConfig {
public:
	IOConfig();
	IOConfig(NTV2DeviceID id, NTV2Mode mode, IOSelection io,
		 NTV2VideoFormat vf = NTV2_FORMAT_UNKNOWN,
		 NTV2PixelFormat pf = NTV2_FBF_INVALID);

	bool operator==(const IOConfig &other);
	bool operator!=(const IOConfig &other);

	uint32_t NumChannels() const;
	uint32_t NumFramestores() const;

	static NTV2ChannelList
	IoSelectionToChannels(IOSelection io, NTV2Mode mode,
			      NTV2VideoFormat vf = NTV2_FORMAT_UNKNOWN,
			      NTV2DeviceID id = DEVICE_ID_NOTFOUND);
	static NTV2ChannelList
	IoSelectionToFramestores(IOSelection io, NTV2Mode mode,
				 NTV2VideoFormat vf = NTV2_FORMAT_UNKNOWN,
				 NTV2DeviceID id = DEVICE_ID_NOTFOUND);

	NTV2ChannelList Channels() const;
	NTV2ChannelList Framestores() const;

	NTV2InputSourceSet InputSources() const;
	NTV2OutputDestinations OutputDestinations() const;

	uint32_t FirstChannelIndex() const;
	uint32_t FirstFramestoreIndex() const;
	NTV2InputSource FirstInputSource() const;
	NTV2OutputDest FirstOutputDest() const;
	NTV2Channel FirstChannel() const;
	NTV2Channel FirstFramestore() const;

	NTV2FormatDesc FormatDesc() const;

	NTV2AudioSystem AudioSystem() const;
	NTV2AudioRate AudioRate() const;
	size_t AudioSize() const;
	audio_format AudioFormat() const;
	speaker_layout SpeakerLayout() const;

	NTV2Standard Standard() const;
	VPIDStandard VpidStandard() const;
	RasterDefinition RasterDef() const;
	ConnectionKind ConnectKind() const;
	NTV2VideoFormat VideoFormat() const;
	NTV2PixelFormat PixelFormat() const;
	NTV2DeviceID DeviceID() const;
	NTV2Mode Mode() const;
	IOSelection IoSelection() const;
	SDITransport SdiTransport() const;
	SDITransport4K SdiTransport4K() const;
	HDMIWireFormat HdmiWireFormat() const;
	NTV2HDMIRange HdmiRange() const;
	uint32_t AudioNumChannels() const;
	uint32_t AudioSampleSize() const;
	uint32_t AudioSampleRate() const;
	bool IsRgb() const;
	bool IsHighFramerate() const;
	bool AutoDetect() const;
	bool HdmiDecimate() const;
	void SetVideoFormat(NTV2VideoFormat vf);
	void SetPixelFormat(NTV2PixelFormat pf);
	void SetDeviceID(NTV2DeviceID id);
	void SetMode(NTV2Mode mode);
	void SetIoSelection(IOSelection io);
	void SetSdiTransport(SDITransport trx);
	void SetSdiTransport4K(SDITransport4K t4k);
	void SetAudioNumChannels(uint32_t num);
	void SetAudioSampleSize(uint32_t size);
	void SetAudioSampleRate(uint32_t rate);
	void SetAutoDetect(bool enable = false);
	void SetHdmiDecimate(bool enable = false);
	void SetHdmiRange(NTV2HDMIRange range);
	void SetVpidData(const VPIDDataList &vpids);
	void SetFirstChannel(NTV2Channel ch);
	void SetFirstFramestore(NTV2Channel ch);
	void ClearVpidData();

private:
	void init();
	VPIDStandard contriveVpidStandard() const;
	NTV2DeviceID mDeviceID;
	NTV2VideoFormat mVideoFormat;
	NTV2PixelFormat mPixelFormat;
	NTV2Mode mMode;
	IOSelection mIoSelection;
	SDITransport mSdiTransport;
	SDITransport4K mSdi4kTransport;
	HDMIWireFormat mHdmiWireFormat;
	VPIDDataList mVpids;
	NTV2OutputDestinations mOutputDests;
	NTV2InputSourceSet mInputSources;
	uint32_t mAudioNumChannels;
	uint32_t mAudioSampleSize;
	uint32_t mAudioSampleRate;
	NTV2Channel mFirstChannelOverride;
	NTV2Channel mFirstFramestoreOverride;
	NTV2HDMIRange mHdmiRange;
	bool mHdmiDecimate;
	bool mAutoDetect;
};

//TODO(paulh): Consolidate the two Props classes
// class SourceProps {
// public:
// 	explicit SourceProps();
// 	explicit SourceProps(NTV2DeviceID devID);
// 	~SourceProps() = default;
// 	SourceProps(const SourceProps &props);
// 	SourceProps(SourceProps &&props);
// 	void operator=(const SourceProps &props);
// 	void operator=(SourceProps &&props);
// 	bool operator==(const SourceProps &props);
// 	bool operator!=(const SourceProps &props);

// 	NTV2InputSource InitialInputSource() const;
// 	NTV2InputSourceSet InputSources() const;
// 	NTV2Channel Channel() const;
// 	NTV2Channel Framestore() const;
// 	NTV2AudioSystem AudioSystem() const;
// 	NTV2AudioRate AudioRate() const;
// 	size_t AudioSize() const;
// 	audio_format AudioFormat() const;
// 	speaker_layout SpeakerLayout() const;

// 	NTV2DeviceID deviceID;
// 	IOSelection ioSelect;
// 	NTV2VideoFormat videoFormat;
// 	NTV2PixelFormat pixelFormat;
// 	SDITransport sdiTransport;
// 	SDITransport4K sdi4kTransport;
// 	VPIDDataList vpids;
// 	uint32_t audioNumChannels;
// 	uint32_t audioSampleSize;
// 	uint32_t audioSampleRate;
// 	bool autoDetect;
// 	bool deactivateWhileNotShowing;
// 	bool swapFrontCenterLFE;
// };

// class OutputProps {
// public:
// 	explicit OutputProps(NTV2DeviceID devID);
// 	~OutputProps() = default;
// 	OutputProps(const OutputProps &props);
// 	OutputProps(OutputProps &&props);
// 	void operator=(const OutputProps &props);
// 	void operator=(OutputProps &&props);
// 	bool operator==(const OutputProps &props);
// 	bool operator!=(const OutputProps &props);

// 	NTV2FormatDesc FormatDesc();
// 	NTV2Channel Channel() const;
// 	NTV2Channel Framestore() const;
// 	NTV2AudioSystem AudioSystem() const;
// 	NTV2AudioRate AudioRate() const;
// 	size_t AudioSize() const;
// 	audio_format AudioFormat() const;
// 	speaker_layout SpeakerLayout() const;

// 	NTV2DeviceID deviceID;
// 	IOSelection ioSelect;
// 	NTV2OutputDestination outputDest;
// 	NTV2VideoFormat videoFormat;
// 	NTV2PixelFormat pixelFormat;
// 	SDITransport sdiTransport;
// 	SDITransport4K sdi4kTransport;
// 	uint32_t audioNumChannels;
// 	uint32_t audioSampleSize;
// 	uint32_t audioSampleRate;
// };
