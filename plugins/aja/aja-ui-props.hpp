#pragma once

#include <obs-module.h>

#include <ajantv2/includes/ntv2enums.h>

#include <string>

static const char *kProgramOutputID = "aja_output";
static const char *kPreviewOutputID = "aja_preview_output";

struct UIProperty {
	const char *id;
	const char *text;
	const char *tooltip;
};

static const UIProperty kUIPropCaptureModule = {
	"aja_source",
	obs_module_text("AJACapture.Device"),
	"",
};

static const UIProperty kUIPropOutputModule = {
	"aja_output",
	obs_module_text("AJAOutput.Device"),
	"",
};

// This is used as an "invisible" property to give the program and preview
// plugin instances an identifier before the output has been created/started.
// This ID is then used by the CardManager class for tracking device channel
// usage across the capture and output plugin instances.
static const UIProperty kUIPropAJAOutputID = {
	"aja_output_id",
	"",
	"",
};

static const UIProperty kUIPropDevice = {
	"ui_prop_device",
	obs_module_text("Device"),
	"",
};

static const UIProperty kUIPropOutput = {
	"ui_prop_output",
	obs_module_text("Output"),
	"",
};

static const UIProperty kUIPropInput = {
	"ui_prop_input",
	obs_module_text("Input"),
	"",
};

static const UIProperty kUIPropIOSelect = {"ui_prop_select_input",
					   obs_module_text("IOSelect"), ""};

static const UIProperty kUIPropSDI4KTransport = {
	"ui_prop_sdi_transport",
	obs_module_text("SDI4KTransport"),
	"",
};

static const UIProperty kUIPropVideoFormatSelect = {
	"ui_prop_vid_fmt",
	obs_module_text("VideoFormat"),
	"",
};

static const UIProperty kUIPropPixelFormatSelect = {
	"ui_prop_pix_fmt",
	obs_module_text("PixelFormat"),
	"",
};

static const UIProperty kUIPropAutoStartOutput = {
	"ui_prop_auto_start_output",
	obs_module_text("AutoStart"),
	"",
};

static const UIProperty kUIPropDeactivateWhenNotShowing = {
	"ui_prop_deactivate_when_not_showing",
	obs_module_text("DeactivateWhenNotShowing"),
	"",
};

static const UIProperty kUIPropBuffering = {
	"ui_prop_buffering",
	obs_module_text("Buffering"),
	"",
};

// OBS UI property helpers used by both the capture and output plugins
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
