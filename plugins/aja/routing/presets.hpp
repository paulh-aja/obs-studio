#pragma once

#include "../aja-enums.hpp"

#include <ajantv2/includes/ntv2enums.h>
#include <ajantv2/includes/ntv2signalrouter.h>

#include <map>
#include <string>
#include <vector>

namespace aja {
/* The AJA hardware and NTV2 SDK uses a concept called "Signal Routing" to connect high-level firmware
 * blocks known as "Widgets" to one another via "crosspoint" connections. This facilitates streaming
 * data from one Widget to another to achieve specific functionality.
 * Such functionality may include SDI/HDMI capture/output, colorspace conversion, hardware LUTs, etc.
 * 
 * This code references a table of RoutingPreset entries, where each entry contains the settings required
 * to configure an AJA device for a particular capture or output task. These settings include the number of
 * physical IO Widgets (SDI or HDMI) required, number of framestore Widgets required, register settings
 * that must be enabled or disabled, and a special short-hand "route string".
 * Of special note is the route string, which is parsed into a map of NTV2XptConnections. These connections
 * are then applied as the "signal route", connecting the Widget's crosspoints together.
 */
struct RoutingPreset {
	ConnectionKind kind;      // SDI/HDMI/Analog/etc.
	NTV2Mode mode;            // Capture or Display?
	uint32_t num_channels;    // Number of physical IO ports
	uint32_t num_framestores; // Number of framestore widgets
	uint32_t flags;           // bitfield of RoutingPresetFlags
	bool is_rgb;
	std::string route_string; // Crosspoint routing shorthand string
	std::vector<NTV2DeviceID>
		device_ids; // Optional device ids if this preset only works on specific cards
	RasterDefinition raster_def; // Preset targets SD/HD/4K/etc.
	HDMIWireFormat hdmi_wire_format;
	SDIWireFormat sdi_wire_format;
	VPIDStandard vpid_standard; // SMPTE VPID byte 1 value
};

using RoutingPresets = std::vector<RoutingPreset>;
using RoutingPresetPair = std::pair<std::string, RoutingPreset>;
using RoutingPresetMap = std::map<std::string, RoutingPreset>;

bool ParseRouteString(const std::string &route, NTV2XptConnections &cnx);

class RoutingConfigurator {
public:
	RoutingConfigurator();

	void AddPreset(const std::string &name, const RoutingPreset &preset);
	bool PresetByName(const std::string &name, RoutingPreset &preset) const;

	RoutingPresetMap GetPresetTable() const;

	bool FindFirstPreset(ConnectionKind kind, NTV2DeviceID id,
			     NTV2Mode mode, NTV2VideoFormat vf,
			     NTV2PixelFormat pf, VPIDStandard standard,
			     RoutingPreset &preset);

private:
	void build_preset_table();

	RoutingPresetMap m_presets;
};

} // aja
