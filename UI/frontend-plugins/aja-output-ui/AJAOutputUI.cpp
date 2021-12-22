#include "AJAOutputUI.h"
#include "aja-ui-main.h"

#include "../../../plugins/aja/aja-ui-props.hpp"
#include "../../../plugins/aja/aja-enums.hpp"
#include "../../../plugins/aja/aja-card-manager.hpp"
#include "../../../plugins/aja/aja-routing.hpp"

#include <ajantv2/includes/ntv2card.h>
#include <ajantv2/includes/ntv2devicefeatures.h>
#include <ajantv2/includes/ntv2enums.h>
#include <ajantv2/includes/ntv2utils.h>

#include <obs-module.h>
#include <util/platform.h>
#include <util/util.hpp>

AJAOutputUI::AJAOutputUI(QWidget *parent) : QDialog(parent), ui(new Ui_Output)
{
	ui->setupUi(this);

	setSizeGripEnabled(true);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	propertiesView = nullptr;
	previewPropertiesView = nullptr;
	miscPropertiesView = nullptr;
}

void AJAOutputUI::ShowHideDialog()
{
	SetupPropertiesView();
	SetupPreviewPropertiesView();
	SetupMiscPropertiesView();

	setVisible(!isVisible());
}

void AJAOutputUI::SetupPropertiesView()
{
	if (propertiesView)
		delete propertiesView;

	obs_data_t *settings = obs_data_create();

	OBSData data = load_settings(kProgramPropsFilename);
	if (data) {
		obs_data_apply(settings, data);
	} else {
		// apply default settings
		obs_data_set_int(settings, kUIPropOutput.id,
				 static_cast<long long>(IOSelection::Invalid));
		obs_data_set_int(settings, kUIPropVideoFormatSelect.id,
				 static_cast<long long>(NTV2_FORMAT_720p_5994));
		obs_data_set_int(settings, kUIPropPixelFormatSelect.id,
				 static_cast<long long>(NTV2_FBF_8BIT_YCBCR));
		obs_data_set_int(settings, kUIPropSDI4KTransport.id,
				 static_cast<long long>(
					 SDI4KTransport::TwoSampleInterleave));
	}

	// Assign an ID to the program output plugin instance for channel usage tracking
	obs_data_set_string(settings, kUIPropAJAOutputID.id, kProgramOutputID);

	propertiesView = new OBSPropertiesView(
		settings, "aja_output",
		(PropertiesReloadCallback)obs_get_output_properties, 170);

	ui->propertiesLayout->addWidget(propertiesView);
	obs_data_release(settings);

	connect(propertiesView, SIGNAL(Changed()), this,
		SLOT(PropertiesChanged()));
}

void AJAOutputUI::SaveSettings(const char *filename, obs_data_t *settings)
{
	BPtr<char> modulePath =
		obs_module_get_config_path(obs_current_module(), "");

	os_mkdirs(modulePath);

	BPtr<char> path =
		obs_module_get_config_path(obs_current_module(), filename);

	if (settings)
		obs_data_save_json_safe(settings, path, "tmp", "bak");
}

void AJAOutputUI::SetupPreviewPropertiesView()
{
	if (previewPropertiesView)
		delete previewPropertiesView;

	obs_data_t *settings = obs_data_create();

	OBSData data = load_settings(kPreviewPropsFilename);
	if (data) {
		obs_data_apply(settings, data);
	} else {
		// apply default settings
		obs_data_set_int(settings, kUIPropOutput.id,
				 static_cast<long long>(IOSelection::Invalid));
		obs_data_set_int(settings, kUIPropVideoFormatSelect.id,
				 static_cast<long long>(NTV2_FORMAT_720p_5994));
		obs_data_set_int(settings, kUIPropPixelFormatSelect.id,
				 static_cast<long long>(NTV2_FBF_8BIT_YCBCR));
		obs_data_set_int(settings, kUIPropSDI4KTransport.id,
				 static_cast<long long>(
					 SDI4KTransport::TwoSampleInterleave));
	}

	// Assign an ID to the program output plugin instance for channel usage tracking
	obs_data_set_string(settings, kUIPropAJAOutputID.id, kPreviewOutputID);

	previewPropertiesView = new OBSPropertiesView(
		settings, "aja_output",
		(PropertiesReloadCallback)obs_get_output_properties, 170);

	ui->previewPropertiesLayout->addWidget(previewPropertiesView);
	obs_data_release(settings);

	connect(previewPropertiesView, SIGNAL(Changed()), this,
		SLOT(PreviewPropertiesChanged()));
}

void AJAOutputUI::on_outputButton_clicked()
{
	SaveSettings(kProgramPropsFilename, propertiesView->GetSettings());
	output_toggle();
}

void AJAOutputUI::PropertiesChanged()
{
	SaveSettings(kProgramPropsFilename, propertiesView->GetSettings());
}

void AJAOutputUI::OutputStateChanged(bool active)
{
	QString text;
	if (active) {
		text = QString(obs_module_text("Stop"));
	} else {
		text = QString(obs_module_text("Start"));
	}

	ui->outputButton->setChecked(active);
	ui->outputButton->setText(text);
}

void AJAOutputUI::on_previewOutputButton_clicked()
{
	SaveSettings(kPreviewPropsFilename,
		     previewPropertiesView->GetSettings());
	preview_output_toggle();
}

void AJAOutputUI::PreviewPropertiesChanged()
{
	SaveSettings(kPreviewPropsFilename,
		     previewPropertiesView->GetSettings());
}

void AJAOutputUI::PreviewOutputStateChanged(bool active)
{
	QString text;
	if (active) {
		text = QString(obs_module_text("Stop"));
	} else {
		text = QString(obs_module_text("Start"));
	}

	ui->previewOutputButton->setChecked(active);
	ui->previewOutputButton->setText(text);
}

// **********
// Misc Props
// **********
static void populate_multi_view_audio_sources(obs_property_t *list,
					      NTV2DeviceID id)
{
	obs_property_list_clear(list);
	const QList<NTV2InputSource> kMultiViewAudioInputs = {
		NTV2_INPUTSOURCE_SDI1,  NTV2_INPUTSOURCE_SDI2,
		NTV2_INPUTSOURCE_SDI3,  NTV2_INPUTSOURCE_SDI4,
		NTV2_INPUTSOURCE_HDMI1,
	};
	for (const auto &inp : kMultiViewAudioInputs) {
		if (NTV2DeviceCanDoInputSource(id, inp)) {
			std::string inputSourceStr =
				NTV2InputSourceToString(inp, true);
			obs_property_list_add_int(list, inputSourceStr.c_str(),
						  (long long)inp);
		}
	}
}

bool on_card_changed(void *data, obs_properties_t *props, obs_property_t *list,
		     obs_data_t *settings)
{
	const char *cardID = obs_data_get_string(settings, kUIPropDevice.id);
	if (!cardID)
		return false;
	aja::CardManager *cardManager = (aja::CardManager *)data;
	if (!cardManager)
		return false;
	auto cardEntry = cardManager->GetCardEntry(cardID);
	if (!cardEntry)
		return false;

	NTV2DeviceID deviceID = cardEntry->GetDeviceID();
	bool enableMultiViewUI = NTV2DeviceCanDoHDMIMultiView(deviceID);
	obs_property_t *multiViewCheckbox =
		obs_properties_get(props, kUIPropMultiViewEnable.id);
	obs_property_t *multiViewAudioSource =
		obs_properties_get(props, kUIPropMultiViewAudioSource.id);
	populate_multi_view_audio_sources(multiViewAudioSource, deviceID);
	// obs_property_set_enabled(multiViewCheckbox, enableMultiViewUI);
	// obs_property_set_enabled(multiViewAudioSource, enableMultiViewUI);

	return true;
}

bool on_multi_view_enable(void *data, obs_properties_t *props, obs_property_t *list,
			  obs_data_t *settings)
{
	const bool multiViewEnabled = obs_data_get_bool(settings, kUIPropMultiViewEnable.id);
	const int audioSource = obs_data_get_int(settings, kUIPropMultiViewAudioSource.id);
	const char *cardID = obs_data_get_string(settings, kUIPropDevice.id);
	if (!cardID)
		return false;

	aja::CardManager *cardManager = (aja::CardManager *)data;
	if (!cardManager)
		return false;
	CNTV2Card *card = cardManager->GetCard(cardID);
	if (!card)
		return false;

	std::ostringstream oss;
	for (int i = 0; i < 4; i++) {
		std::string datastream = std::to_string(i);
		oss << "sdi[" << datastream << "][0]->hdmi[0][" << datastream
		    << "];";
	}

	NTV2DeviceID deviceId = card->GetDeviceID();
	if (NTV2DeviceCanDoHDMIMultiView(deviceId)) {
		NTV2XptConnections cnx;
		if (Routing::ParseRouteString(oss.str(), cnx)) {
			card->SetMultiRasterBypassEnable(!multiViewEnabled);
			if (multiViewEnabled) {
				card->ApplySignalRoute(cnx, false);
			} else {
				card->RemoveConnections(cnx);
			}
		}
	}
	return true;
}

static obs_properties_t *get_misc_props(void *vp)
{
	AJAOutputUI *outputUI = (AJAOutputUI *)vp;
	if (!outputUI)
		return nullptr;
	aja::CardManager *cardManager = outputUI->GetCardManager();
	if (!cardManager)
		return nullptr;

	obs_properties_t *props = obs_properties_create();
	obs_property_t *deviceList = obs_properties_add_list(
		props, kUIPropDevice.id, obs_module_text(kUIPropDevice.text),
		OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);
	obs_property_t *multiViewEnable = obs_properties_add_bool(
		props, kUIPropMultiViewEnable.id,
		obs_module_text(kUIPropMultiViewEnable.text));
	obs_property_t *multiViewAudioSources = obs_properties_add_list(
		props, kUIPropMultiViewAudioSource.id,
		obs_module_text(kUIPropMultiViewAudioSource.text),
		OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);

	obs_property_list_clear(deviceList);
	obs_property_list_clear(multiViewAudioSources);

	NTV2DeviceID firstDeviceID = DEVICE_ID_NOTFOUND;
	for (const auto &iter : *cardManager) {
		if (!iter.second)
			continue;
		if (firstDeviceID == DEVICE_ID_NOTFOUND)
			firstDeviceID = iter.second->GetDeviceID();
		obs_property_list_add_string(
			deviceList, iter.second->GetDisplayName().c_str(),
			iter.second->GetCardID().c_str());
	}
	populate_multi_view_audio_sources(multiViewAudioSources, firstDeviceID);
	obs_property_set_modified_callback2(deviceList, on_card_changed,
					    (void *)cardManager);
	obs_property_set_modified_callback2(multiViewEnable, on_multi_view_enable, (void*)cardManager);
	return props;
}

void AJAOutputUI::MiscPropertiesChanged()
{
	SaveSettings(kMiscPropsFilename, miscPropertiesView->GetSettings());
}

void AJAOutputUI::SetCardManager(aja::CardManager *cm)
{
	cardManager = cm;
}

aja::CardManager *AJAOutputUI::GetCardManager()
{
	return cardManager;
}

void AJAOutputUI::SetupMiscPropertiesView()
{
	if (miscPropertiesView)
		delete miscPropertiesView;

	obs_data_t *settings = obs_data_create();
	OBSData data = load_settings(kMiscPropsFilename);
	if (data) {
		obs_data_apply(settings, data);
	} else {
		// TODO: apply defaults
	}

	miscPropertiesView = new OBSPropertiesView(
		settings, this, (PropertiesReloadCallback)get_misc_props,
		nullptr, nullptr, 170);

	ui->miscPropertiesLayout->addWidget(miscPropertiesView);
	obs_data_release(settings);
	connect(miscPropertiesView, SIGNAL(Changed()), this,
		SLOT(MiscPropertiesChanged()));
}