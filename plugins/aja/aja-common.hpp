#include "aja-enums.hpp"

#include <obs-module.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <ajantv2/includes/ntv2enums.h>
#include <ajantv2/includes/ntv2publicinterface.h>

class CNTV2Card;

using VideoFormatMap = std::map<NTV2Standard, std::vector<NTV2VideoFormat>>;
using VideoFormatList = std::vector<NTV2VideoFormat>;
using VideoStandardList = std::vector<NTV2Standard>;

static const uint32_t kDefaultAudioChannels = 8;
static const uint32_t kDefaultAudioSampleRate = 48000;
static const uint32_t kDefaultAudioSampleSize = 4;
static const int kVideoFormatAuto = -1;
static const int kPixelFormatAuto = -1;
static const NTV2PixelFormat kDefaultAJAPixelFormat = NTV2_FBF_8BIT_YCBCR;

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

// RasterDefinition helpers
extern RasterDefinition GetRasterDefinition(IOSelection io, NTV2VideoFormat vf,
					    NTV2DeviceID deviceID);
extern RasterDefinition DetermineRasterDefinition(NTV2VideoFormat vf);
extern std::string RasterDefinitionToString(const RasterDefinition &rd);

// SDIWireFormat helpers
extern std::string SDIWireFormatToString(SDIWireFormat sf);
extern uint32_t GetNumSDIWires(SDIWireFormat sf);
extern bool IsOneWireSDIWireFormat(SDIWireFormat sf);
extern bool IsTwoWireSDIWireFormat(SDIWireFormat sf);
extern bool IsFourWireSDIWireFormat(SDIWireFormat sf);
extern bool Is4KSquaresSDIWireFormat(SDIWireFormat sf);
extern bool Is4K2SISDIWireFormat(SDIWireFormat sf);
extern bool Is8K2SISDIWireFormat(SDIWireFormat sf);

// VPIDStandard helpers
extern bool IsSupportedVPIDStandard(VPIDStandard standard);
extern bool Is3GVPIDStandard(VPIDStandard standard);
extern bool Is12GVPIDStandard(VPIDStandard standard);

} // aja
