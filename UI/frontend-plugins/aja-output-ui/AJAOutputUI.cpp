#include "AJAOutputUI.h"
#include "aja-ui-main.h"

#include "../../../plugins/aja/aja-ui-props.hpp"
#include "../../../plugins/aja/aja-enums.hpp"

#include <ajantv2/includes/ntv2enums.h>

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
}

void AJAOutputUI::ShowHideDialog()
{
	SetupPropertiesView();
	SetupPreviewPropertiesView();

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
static obs_properties_t *GetMiscProps(void *vp, obs_data_t *old_settings, obs_data_t *new_settings)
{
	obs_properties_t *props = obs_properties_create();
	obs_property_t *deviceList = obs_properties_add_list(props,
		kUIPropDevice.id, obs_module_text(kUIPropDevice.text),
		OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);
	obs_property_t *multiViewEnable = obs_properties_add_bool(props,
		kUIPropMultiViewEnable.id, obs_module_text(kUIPropMultiViewEnable.text));
	obs_property_t *multiViewAudioSource = obs_properties_add_list(props,
		kUIPropMultiViewAudioSource.id, obs_module_text(kUIPropMultiViewAudioSource.text),
		OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);

	// PopulateCardList(deviceList);
	// PopulateQuadViewAudioSourceList(quadViewAudioSourceList);
	// obs_property_set_modified_callback(deviceList, OnCardChanged);
	// obs_property_set_modified_callback(quadViewEnableCheckbox, OnQuadViewEnabled);
	return props;
}

void AJAOutputUI::MiscPropertiesChanged()
{

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
	miscPropertiesView = new OBSPropertiesView(settings, "aja_output_misc",
		(PropertiesReloadCallback)GetMiscProps, 170);
}