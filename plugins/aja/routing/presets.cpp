#include "boolinq.h"
#include "presets.hpp"
#include "widget-io.hpp"
#include "sdi_ycbcr_capture.hpp"
#include "sdi_ycbcr_display.hpp"
#include "sdi_rgb_capture.hpp"
#include "sdi_rgb_display.hpp"
#include "hdmi_ycbcr_capture.hpp"
#include "hdmi_ycbcr_display.hpp"
#include "hdmi_rgb_capture.hpp"
#include "hdmi_rgb_display.hpp"

#include "../aja-common.hpp"

#include <ajabase/common/common.h>
#include <ajantv2/includes/ntv2utils.h>
#include <ajantv2/includes/ntv2devicefeatures.h>

namespace aja {

/*
 * Parse the widget routing shorthand string into a map of input and output NTV2CrosspointIDs.
 * For example "sdi[0][0]->fb[0][0]" is shorthand for connecting the output crosspoint for
 * SDI1/Datastream1 (NTV2_XptSDIIn1) to the input crosspoint for Framestore1/Datastream1 (NTV2_XptFrameBuffer1Input).
 * These routing shorthand strings are found within the RoutingConfig structs in the "routing" sub-directory of the plugin.
 */
bool ParseRouteString(const std::string &route, NTV2XptConnections &cnx)
{
	blog(LOG_DEBUG, "aja::Routing::ParseRouteString: Input string: %s",
	     route.c_str());

	std::string route_lower(route);
	route_lower = aja::lower(route_lower);
	const std::string &route_strip = aja::replace(route_lower, " ", "");

	if (route_strip.empty()) {
		blog(LOG_DEBUG,
		     "Routing::ParseRouteString: input string is empty!");
		return false;
	}

	/* TODO(paulh): Tally up the lines and tokens and check that they are all parsed OK.
	 * Right now we just return true if ANY tokens were parsed. This is OK _for now_ because
	 * the route strings currently only come from a known set.
	 */
	NTV2StringList lines;
	NTV2StringList tokens;

	lines = aja::split(route_strip, ';');
	if (lines.empty())
		lines.push_back(route_strip);

	int32_t parse_ok = 0;
	for (const auto &l : lines) {
		if (l.empty()) {
			blog(LOG_DEBUG,
			     "aja::Routing::ParseRouteString: Empty line!");
			continue;
		}

		blog(LOG_DEBUG, "aja::Routing::ParseRouteString: Line: %s",
		     l.c_str());

		NTV2StringList tokens = aja::split(l, "->");
		if (tokens.empty() || tokens.size() != 2) {
			blog(LOG_DEBUG,
			     "aja::Routing::ParseRouteString: Invalid token count!");
			continue;
		}

		const std::string &left = tokens[0];  // output crosspoint
		const std::string &right = tokens[1]; // input crosspoint
		if (left.empty() || left.length() > 64) {
			blog(LOG_DEBUG,
			     "aja::Routing::ParseRouteString: Invalid Left token!");
			continue;
		}
		if (right.empty() || right.length() > 64) {
			blog(LOG_DEBUG,
			     "aja::Routing::ParseRouteString: Invalid right token!");
			continue;
		}

		blog(LOG_DEBUG,
		     "aja::Routing::ParseRouteString: Left Token: %s -> Right Token: %s",
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
				     "aja::Routing::ParseRouteString: Found NTV2OutputCrosspointID %s",
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
						     "aja::Routing::ParseRouteString: Found NTV2InputCrosspointID %s",
						     NTV2InputCrosspointIDToString(
							     widget_inp.id)
							     .c_str());

						cnx[widget_inp.id] =
							widget_out.id;
						parse_ok++;
					} else {
						blog(LOG_DEBUG,
						     "aja::Routing::ParseRouteString: NTV2InputCrosspointID not found!");
					}
				}
			} else {
				blog(LOG_DEBUG,
				     "aja::Routing::ParseRouteString: NTV2OutputCrosspointID not found!");
			}
		}
	}

	return parse_ok > 0;
}

RoutingConfigurator::RoutingConfigurator()
{
	build_preset_table();
}

void RoutingConfigurator::AddPreset(const std::string &name,
				    const RoutingPreset &preset)
{
	auto p = RoutingPresetPair{name, preset};

	if (m_presets.find(name) != m_presets.end())
		return;

	m_presets.insert(RoutingPresetPair{name, preset});
}

bool RoutingConfigurator::PresetByName(const std::string &name,
				       RoutingPreset &preset) const
{
	if (m_presets.find(name) != m_presets.end()) {
		preset = m_presets.at(name);
		return true;
	}
	return false;
}

void RoutingConfigurator::build_preset_table()
{
	// SDI YCbCr
	for (auto &&rp : kSDICaptureYCbCrPresets)
		AddPreset(std::move(rp.first), std::move(rp.second));
	for (auto &&rp : kSDIDisplayYCbCrPresets)
		AddPreset(std::move(rp.first), std::move(rp.second));
	// SDI RGB
	for (auto &&rp : kSDICaptureRGBPresets)
		AddPreset(std::move(rp.first), std::move(rp.second));
	for (auto &&rp : kSDIDisplayRGBPresets)
		AddPreset(std::move(rp.first), std::move(rp.second));

	// HDMI YCbCr
	for (auto &&rp : kHDMICaptureYCbCrPresets)
		AddPreset(std::move(rp.first), std::move(rp.second));
	for (auto &&rp : kHDMIDisplayYCbCrPresets)
		AddPreset(std::move(rp.first), std::move(rp.second));
	// HDMI RGB
	for (auto &&rp : kHDMICaptureRGBPresets)
		AddPreset(std::move(rp.first), std::move(rp.second));
	for (auto &&rp : kHDMIDisplayRGBPresets)
		AddPreset(std::move(rp.first), std::move(rp.second));
}

RoutingPresetMap RoutingConfigurator::GetPresetTable() const
{
	return m_presets;
}

bool RoutingConfigurator::FindFirstPreset(ConnectionKind kind, NTV2DeviceID id,
					  NTV2Mode mode, NTV2VideoFormat vf,
					  NTV2PixelFormat pf,
					  VPIDStandard standard,
					  RoutingPreset &preset)
{
	if (NTV2DeviceCanDoVideoFormat(id, vf) &&
	    NTV2DeviceCanDoFrameBufferFormat(id, pf)) {
		const auto &rd = DetermineRasterDefinition(vf);

		bool is_rgb = NTV2_IS_FBF_RGB(pf);

		auto query = boolinq::from(m_presets)
				     .where([&](const RoutingPresetPair &rpp) {
					     return rpp.second.kind == kind;
				     })
				     .where([&](const RoutingPresetPair &rpp) {
					     return rpp.second.mode == mode;
				     })
				     .where([&](const RoutingPresetPair &rpp) {
					     return rpp.second.raster_def == rd;
				     })
				     .where([&](const RoutingPresetPair &rpp) {
					     return rpp.second.is_rgb == is_rgb;
				     })
				     .where([&](const RoutingPresetPair &rpp) {
					     return rpp.second.vpid_standard ==
						    standard;
				     })
				     .toStdVector();

		if (query.size() >= 1) {
			RoutingPresets device_presets;
			for (auto &&q : query) {
				if (boolinq::from(q.second.device_ids)
					    .contains(id))
					device_presets.push_back(q.second);
			}
			if (device_presets.size() > 0) {
				preset = device_presets.at(0);
				return true;
			}
			preset = query.at(0).second;
			return true;
		}
	}

	return false;
}
} // aja
