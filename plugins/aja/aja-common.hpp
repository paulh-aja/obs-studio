#include "aja-enums.hpp"

#include <obs-module.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <ajantv2/includes/ntv2enums.h>
#include <ajantv2/includes/ntv2card.h>

using VideoFormatMap = std::map<NTV2Standard, std::vector<NTV2VideoFormat>>;
using VideoFormatList = std::vector<NTV2VideoFormat>;
using VideoStandardList = std::vector<NTV2Standard>;

static const uint32_t kDefaultAudioChannels = 8;
static const uint32_t kDefaultAudioSampleRate = 48000;
static const uint32_t kDefaultAudioSampleSize = 4;
static const int kVideoFormatAuto = -1;
static const int kPixelFormatAuto = -1;
static const NTV2PixelFormat kDefaultAJAPixelFormat = NTV2_FBF_8BIT_YCBCR;

// Common OBS property helpers used by both the capture and output plugins
extern void filter_io_selection_input_list(const std::string &cardID,
					   const std::string &channelOwner,
					   obs_property_t *list);
extern void filter_io_selection_output_list(const std::string &cardID,
					    const std::string &channelOwner,
					    obs_property_t *list);
extern void populate_io_selection_input_list(const std::string &cardID,
					     const std::string &channelOwner,
					     NTV2DeviceID deviceID,
					     obs_property_t *list);
extern void populate_io_selection_output_list(const std::string &cardID,
					      const std::string &channelOwner,
					      NTV2DeviceID deviceID,
					      obs_property_t *list);
extern void
populate_video_format_list(NTV2DeviceID deviceID, obs_property_t *list,
			   NTV2VideoFormat genlockFormat = NTV2_FORMAT_UNKNOWN);
extern void populate_pixel_format_list(NTV2DeviceID deviceID,
				       obs_property_t *list);
extern void populate_sdi_4k_transport_list(obs_property_t *list);
extern bool aja_video_format_changed(obs_properties_t *props,
				     obs_property_t *list,
				     obs_data_t *settings);

// Additional helpers for AJA channel and signal routing configuration not found in the NTV2 SDK
namespace aja {

template<typename T> bool vec_contains(const std::vector<T> &vec, const T &elem)
{
	return std::find(vec.begin(), vec.end(), elem) != vec.end();
}

extern video_format AJAPixelFormatToOBSVideoFormat(NTV2PixelFormat pf);

extern void GetSortedVideoFormats(NTV2DeviceID id,
				  const VideoStandardList &standards,
				  VideoFormatList &videoFormats);

extern uint32_t CardNumFramestores(NTV2DeviceID id);
extern uint32_t CardNumAudioSystems(NTV2DeviceID id);
extern bool CardCanDoSDIMonitorOutput(NTV2DeviceID id);
extern bool CardCanDoHDMIMonitorOutput(NTV2DeviceID id);
extern bool CardCanDo1xSDI12G(NTV2DeviceID id);
extern bool Is3GLevelB(CNTV2Card *card, NTV2Channel channel);
extern NTV2VideoFormat GetLevelAFormatForLevelBFormat(NTV2VideoFormat vf);
extern NTV2VideoFormat InterlacedFormatForPsfFormat(NTV2VideoFormat vf);
extern bool IsSingleSDIDevice(NTV2DeviceID id);
extern bool IsIODevice(NTV2DeviceID id);
extern bool IsRetailSDI12G(NTV2DeviceID id);
extern bool IsOutputOnlyDevice(NTV2DeviceID id);

extern std::string SDI4KTransportToString(SDI4KTransport mode);

extern std::string IOSelectionToString(IOSelection io);
extern void IOSelectionToInputSources(IOSelection io,
				      NTV2InputSourceSet &inputSources);
extern void IOSelectionToOutputDests(IOSelection io,
				     NTV2OutputDestinations &outputDests);
extern bool DeviceCanDoIOSelectionIn(NTV2DeviceID id, IOSelection io);
extern bool DeviceCanDoIOSelectionOut(NTV2DeviceID id, IOSelection io);
extern bool IsSDIOneWireIOSelection(IOSelection io);
extern bool IsSDITwoWireIOSelection(IOSelection io);
extern bool IsSDIFourWireIOSelection(IOSelection io);
extern bool IsMonitorOutputSelection(NTV2DeviceID id, IOSelection io);

extern std::string MakeCardID(CNTV2Card &card);

} // aja
