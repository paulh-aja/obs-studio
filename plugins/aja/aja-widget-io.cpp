#include "aja-widget-io.hpp"

#include <ajantv2/includes/ntv2utils.h>

#include <iostream>

// firmware widget nicknames used by signal routing syntax parser
static const char *kFramebufferNickname = "fb";
static const char *kCSCNickname = "csc";
static const char *kDualLinkInNickname = "dli";
static const char *kDualLinkOutNickname = "dlo";
static const char *kLUTNickname = "lut";
static const char *kSDINickname = "sdi";
static const char *kMultiLinkNickname = "ml";
static const char *kMixerNickname = "mix";
static const char *kHDMINickname = "hdmi";
static const char *kLUT3DNickname = "lut3d";
static const char *k4KDownConvertNickname = "4kdc";
static const char *kAnalogNickname = "analog";
static const char *kTSIMuxNickname = "tsi";
static const char *kUpDownConvertNickname = "udc";
static const char *kCompositeNickname = "composite";
static const char *kStereoCompNickname = "stereo";
static const char *kWatermarkNickname = "watermark";
static const char *kBlackNickname = "black";
static const char *kCompressionNickname = "comp";
static const char *kFrameSyncNickname = "fsync";
static const char *kTestPatternNickname = "pat";
static const char *kOENickname = "oe";

// clang-format off

// Table of firmware widget's input crosspoint/id/channel/name/datastream index
static const WidgetInputSocket kWidgetInputSockets[] = {
	// NTV2InputCrosspointID        | NTV2WidgetID       | NTV2Channel         | Name | DatastreamIndex
	{ NTV2_INPUT_CROSSPOINT_INVALID, NTV2_WIDGET_INVALID, NTV2_CHANNEL_INVALID, "", -1},
	{ NTV2_XptFrameBuffer1Input,     NTV2_WgtFrameBuffer1, 	NTV2_CHANNEL1, kFramebufferNickname, 0},
	{ NTV2_XptFrameBuffer1BInput,    NTV2_WgtFrameBuffer1, 	NTV2_CHANNEL1, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer2Input,     NTV2_WgtFrameBuffer2, 	NTV2_CHANNEL2, kFramebufferNickname, 0},
	{ NTV2_XptFrameBuffer2BInput,    NTV2_WgtFrameBuffer2, 	NTV2_CHANNEL2, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer3Input,     NTV2_WgtFrameBuffer3, 	NTV2_CHANNEL3, kFramebufferNickname, 0},
	{ NTV2_XptFrameBuffer3BInput,    NTV2_WgtFrameBuffer3, 	NTV2_CHANNEL3, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer4Input,     NTV2_WgtFrameBuffer4, 	NTV2_CHANNEL4, kFramebufferNickname, 0},
	{ NTV2_XptFrameBuffer4BInput,    NTV2_WgtFrameBuffer4, 	NTV2_CHANNEL4, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer5Input,     NTV2_WgtFrameBuffer5, 	NTV2_CHANNEL5, kFramebufferNickname, 0},
	{ NTV2_XptFrameBuffer5BInput,    NTV2_WgtFrameBuffer5, 	NTV2_CHANNEL5, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer6Input,     NTV2_WgtFrameBuffer6, 	NTV2_CHANNEL6, kFramebufferNickname, 0},
	{ NTV2_XptFrameBuffer6BInput,    NTV2_WgtFrameBuffer6, 	NTV2_CHANNEL6, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer7Input,     NTV2_WgtFrameBuffer7, 	NTV2_CHANNEL7, kFramebufferNickname, 0},
	{ NTV2_XptFrameBuffer7BInput,    NTV2_WgtFrameBuffer7, 	NTV2_CHANNEL7, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer8Input,     NTV2_WgtFrameBuffer8, 	NTV2_CHANNEL8, kFramebufferNickname, 0},
	{ NTV2_XptFrameBuffer8BInput,    NTV2_WgtFrameBuffer8, 	NTV2_CHANNEL8, kFramebufferNickname, 1},
	{ NTV2_XptCSC1VidInput, 		 NTV2_WgtCSC1, 			NTV2_CHANNEL1, kCSCNickname, 0},
	{ NTV2_XptCSC1KeyInput, 		 NTV2_WgtCSC1, 			NTV2_CHANNEL1, kCSCNickname, 1},
	{ NTV2_XptCSC2VidInput, 		 NTV2_WgtCSC2, 			NTV2_CHANNEL2, kCSCNickname, 0},
	{ NTV2_XptCSC2KeyInput, 		 NTV2_WgtCSC2, 			NTV2_CHANNEL2, kCSCNickname, 1},
	{ NTV2_XptCSC3VidInput, 		 NTV2_WgtCSC3, 			NTV2_CHANNEL3, kCSCNickname, 0},
	{ NTV2_XptCSC3KeyInput, 		 NTV2_WgtCSC3, 			NTV2_CHANNEL3, kCSCNickname, 1},
	{ NTV2_XptCSC4VidInput, 		 NTV2_WgtCSC4, 			NTV2_CHANNEL4, kCSCNickname, 0},
	{ NTV2_XptCSC4KeyInput, 		 NTV2_WgtCSC4, 			NTV2_CHANNEL4, kCSCNickname, 1},
	{ NTV2_XptCSC5VidInput, 		 NTV2_WgtCSC5, 			NTV2_CHANNEL5, kCSCNickname, 0},
	{ NTV2_XptCSC5KeyInput, 		 NTV2_WgtCSC5, 			NTV2_CHANNEL5, kCSCNickname, 1},
	{ NTV2_XptCSC6VidInput, 		 NTV2_WgtCSC6, 			NTV2_CHANNEL6, kCSCNickname, 0},
	{ NTV2_XptCSC6KeyInput, 		 NTV2_WgtCSC6, 			NTV2_CHANNEL6, kCSCNickname, 1},
	{ NTV2_XptCSC7VidInput, 		 NTV2_WgtCSC7, 			NTV2_CHANNEL7, kCSCNickname, 0},
	{ NTV2_XptCSC7KeyInput, 		 NTV2_WgtCSC7, 			NTV2_CHANNEL7, kCSCNickname, 1},
	{ NTV2_XptCSC8VidInput, 		 NTV2_WgtCSC8, 			NTV2_CHANNEL8, kCSCNickname, 0},
	{ NTV2_XptCSC8KeyInput, 		 NTV2_WgtCSC8, 			NTV2_CHANNEL8, kCSCNickname, 1},
	{ NTV2_XptLUT1Input, 			 NTV2_WgtLUT1, 			NTV2_CHANNEL1, kLUTNickname, 0},
	{ NTV2_XptLUT2Input, 			 NTV2_WgtLUT2, 			NTV2_CHANNEL2, kLUTNickname, 0},
	{ NTV2_XptLUT3Input, 			 NTV2_WgtLUT3, 			NTV2_CHANNEL3, kLUTNickname, 0},
	{ NTV2_XptLUT4Input, 			 NTV2_WgtLUT4, 			NTV2_CHANNEL4, kLUTNickname, 0},
	{ NTV2_XptLUT5Input, 			 NTV2_WgtLUT5, 			NTV2_CHANNEL5, kLUTNickname, 0},
	{ NTV2_XptLUT6Input, 			 NTV2_WgtLUT6, 			NTV2_CHANNEL6, kLUTNickname, 0},
	{ NTV2_XptLUT7Input, 			 NTV2_WgtLUT7, 			NTV2_CHANNEL7, kLUTNickname, 0},
	{ NTV2_XptLUT8Input, 			 NTV2_WgtLUT8, 			NTV2_CHANNEL8, kLUTNickname, 0},
	{ NTV2_XptMultiLinkOut1Input, 	 NTV2_WgtMultiLinkOut1, NTV2_CHANNEL1, kMultiLinkNickname, 0},
	{ NTV2_XptMultiLinkOut1InputDS2, NTV2_WgtMultiLinkOut1, NTV2_CHANNEL1, kMultiLinkNickname, 0},
	{ NTV2_XptMultiLinkOut2Input, 	 NTV2_WgtMultiLinkOut2, NTV2_CHANNEL1, kMultiLinkNickname, 0},
	{ NTV2_XptMultiLinkOut2InputDS2, NTV2_WgtMultiLinkOut2, NTV2_CHANNEL1, kMultiLinkNickname, 0},
	{ NTV2_XptSDIOut1Input, 		 NTV2_WgtSDIOut1, 		NTV2_CHANNEL1, kSDINickname, 0},
	{ NTV2_XptSDIOut1InputDS2, 		 NTV2_Wgt3GSDIOut1, 	NTV2_CHANNEL1, kSDINickname, 1},
	{ NTV2_XptSDIOut2Input, 		 NTV2_WgtSDIOut2, 		NTV2_CHANNEL2, kSDINickname, 0},
	{ NTV2_XptSDIOut2InputDS2, 		 NTV2_Wgt3GSDIOut2, 	NTV2_CHANNEL2, kSDINickname, 1},
	{ NTV2_XptSDIOut3Input, 		 NTV2_WgtSDIOut3, 		NTV2_CHANNEL3, kSDINickname, 0},
	{ NTV2_XptSDIOut3InputDS2, 		 NTV2_Wgt3GSDIOut3, 	NTV2_CHANNEL3, kSDINickname, 1},
	{ NTV2_XptSDIOut4Input, 		 NTV2_WgtSDIOut4, 		NTV2_CHANNEL4, kSDINickname, 0},
	{ NTV2_XptSDIOut4InputDS2, 		 NTV2_Wgt3GSDIOut4, 	NTV2_CHANNEL4, kSDINickname, 1},
	{ NTV2_XptSDIOut5Input, 		 NTV2_WgtSDIMonOut1, 	NTV2_CHANNEL5, kSDINickname, 0},
	{ NTV2_XptSDIOut5InputDS2, 		 NTV2_WgtSDIMonOut1, 	NTV2_CHANNEL5, kSDINickname, 1},
	{ NTV2_XptSDIOut6Input, 		 NTV2_Wgt3GSDIOut6, 	NTV2_CHANNEL6, kSDINickname, 0},
	{ NTV2_XptSDIOut6InputDS2, 		 NTV2_Wgt3GSDIOut6, 	NTV2_CHANNEL6, kSDINickname, 1},
	{ NTV2_XptSDIOut7Input, 		 NTV2_Wgt3GSDIOut7, 	NTV2_CHANNEL7, kSDINickname, 0},
	{ NTV2_XptSDIOut7InputDS2, 		 NTV2_Wgt3GSDIOut7, 	NTV2_CHANNEL7, kSDINickname, 1},
	{ NTV2_XptSDIOut8Input, 		 NTV2_Wgt3GSDIOut8, 	NTV2_CHANNEL8, kSDINickname, 0},
	{ NTV2_XptSDIOut8InputDS2, 		 NTV2_Wgt3GSDIOut8, 	NTV2_CHANNEL8, kSDINickname, 1},
	{ NTV2_XptDualLinkIn1Input, 	 NTV2_WgtDualLinkV2In1,  NTV2_CHANNEL1, kDualLinkInNickname, 0},
	{ NTV2_XptDualLinkIn1DSInput, 	 NTV2_WgtDualLinkV2In1,  NTV2_CHANNEL1, kDualLinkInNickname, 1},
	{ NTV2_XptDualLinkIn2Input, 	 NTV2_WgtDualLinkV2In2,  NTV2_CHANNEL2, kDualLinkInNickname, 0},
	{ NTV2_XptDualLinkIn2DSInput, 	 NTV2_WgtDualLinkV2In2,  NTV2_CHANNEL2, kDualLinkInNickname, 1},
	{ NTV2_XptDualLinkIn3Input, 	 NTV2_WgtDualLinkV2In3,  NTV2_CHANNEL3, kDualLinkInNickname, 0},
	{ NTV2_XptDualLinkIn3DSInput, 	 NTV2_WgtDualLinkV2In3,  NTV2_CHANNEL3, kDualLinkInNickname, 1},
	{ NTV2_XptDualLinkIn4Input, 	 NTV2_WgtDualLinkV2In4,  NTV2_CHANNEL4, kDualLinkInNickname, 0},
	{ NTV2_XptDualLinkIn4DSInput, 	 NTV2_WgtDualLinkV2In4,  NTV2_CHANNEL4, kDualLinkInNickname, 1},
	{ NTV2_XptDualLinkIn5Input, 	 NTV2_WgtDualLinkV2In5,  NTV2_CHANNEL5, kDualLinkInNickname, 0},
	{ NTV2_XptDualLinkIn5DSInput, 	 NTV2_WgtDualLinkV2In5,  NTV2_CHANNEL5, kDualLinkInNickname, 1},
	{ NTV2_XptDualLinkIn6Input, 	 NTV2_WgtDualLinkV2In6,  NTV2_CHANNEL6, kDualLinkInNickname, 0},
	{ NTV2_XptDualLinkIn6DSInput, 	 NTV2_WgtDualLinkV2In6,  NTV2_CHANNEL6, kDualLinkInNickname, 1},
	{ NTV2_XptDualLinkIn7Input, 	 NTV2_WgtDualLinkV2In7,  NTV2_CHANNEL7, kDualLinkInNickname, 0},
	{ NTV2_XptDualLinkIn7DSInput, 	 NTV2_WgtDualLinkV2In7,  NTV2_CHANNEL7, kDualLinkInNickname, 1},
	{ NTV2_XptDualLinkIn8Input, 	 NTV2_WgtDualLinkV2In8,  NTV2_CHANNEL8, kDualLinkInNickname, 0},
	{ NTV2_XptDualLinkIn8DSInput, 	 NTV2_WgtDualLinkV2In8,  NTV2_CHANNEL8, kDualLinkInNickname, 1},
	{ NTV2_XptDualLinkOut1Input, 	 NTV2_WgtDualLinkV2Out1, NTV2_CHANNEL1, kDualLinkOutNickname, 0},
	{ NTV2_XptDualLinkOut2Input, 	 NTV2_WgtDualLinkV2Out2, NTV2_CHANNEL2, kDualLinkOutNickname, 0},
	{ NTV2_XptDualLinkOut3Input, 	 NTV2_WgtDualLinkV2Out3, NTV2_CHANNEL3, kDualLinkOutNickname, 0},
	{ NTV2_XptDualLinkOut4Input, 	 NTV2_WgtDualLinkV2Out4, NTV2_CHANNEL4, kDualLinkOutNickname, 0},
	{ NTV2_XptDualLinkOut5Input, 	 NTV2_WgtDualLinkV2Out5, NTV2_CHANNEL5, kDualLinkOutNickname, 0},
	{ NTV2_XptDualLinkOut6Input, 	 NTV2_WgtDualLinkV2Out6, NTV2_CHANNEL6, kDualLinkOutNickname, 0},
	{ NTV2_XptDualLinkOut7Input, 	 NTV2_WgtDualLinkV2Out7, NTV2_CHANNEL7, kDualLinkOutNickname, 0},
	{ NTV2_XptDualLinkOut8Input, 	 NTV2_WgtDualLinkV2Out8, NTV2_CHANNEL8, kDualLinkOutNickname, 0},
	{ NTV2_XptMixer1BGKeyInput, 	 NTV2_WgtMixer1, 		 NTV2_CHANNEL1, kMixerNickname, 3},
	{ NTV2_XptMixer1BGVidInput, 	 NTV2_WgtMixer1, 		 NTV2_CHANNEL1, kMixerNickname, 2},
	{ NTV2_XptMixer1FGKeyInput, 	 NTV2_WgtMixer1, 		 NTV2_CHANNEL1, kMixerNickname, 1},
	{ NTV2_XptMixer1FGVidInput, 	 NTV2_WgtMixer1, 		 NTV2_CHANNEL1, kMixerNickname, 0},
	{ NTV2_XptMixer2BGKeyInput, 	 NTV2_WgtMixer2, 		 NTV2_CHANNEL2, kMixerNickname, 3},
	{ NTV2_XptMixer2BGVidInput, 	 NTV2_WgtMixer2, 		 NTV2_CHANNEL2, kMixerNickname, 2},
	{ NTV2_XptMixer2FGKeyInput, 	 NTV2_WgtMixer2, 		 NTV2_CHANNEL2, kMixerNickname, 1},
	{ NTV2_XptMixer2FGVidInput, 	 NTV2_WgtMixer2, 		 NTV2_CHANNEL2, kMixerNickname, 0},
	{ NTV2_XptMixer3BGKeyInput, 	 NTV2_WgtMixer3, 		 NTV2_CHANNEL3, kMixerNickname, 3},
	{ NTV2_XptMixer3BGVidInput, 	 NTV2_WgtMixer3, 		 NTV2_CHANNEL3, kMixerNickname, 2},
	{ NTV2_XptMixer3FGKeyInput, 	 NTV2_WgtMixer3, 		 NTV2_CHANNEL3, kMixerNickname, 1},
	{ NTV2_XptMixer3FGVidInput, 	 NTV2_WgtMixer3, 		 NTV2_CHANNEL3, kMixerNickname, 0},
	{ NTV2_XptMixer4BGKeyInput, 	 NTV2_WgtMixer4, 		 NTV2_CHANNEL4, kMixerNickname, 3},
	{ NTV2_XptMixer4BGVidInput, 	 NTV2_WgtMixer4, 		 NTV2_CHANNEL4, kMixerNickname, 2},
	{ NTV2_XptMixer4FGKeyInput, 	 NTV2_WgtMixer4, 		 NTV2_CHANNEL4, kMixerNickname, 1},
	{ NTV2_XptMixer4FGVidInput, 	 NTV2_WgtMixer4, 		 NTV2_CHANNEL4, kMixerNickname, 0},
	{ NTV2_XptHDMIOutInput, 		 NTV2_WgtHDMIOut1, 		 NTV2_CHANNEL1, kHDMINickname, 0},
	{ NTV2_XptHDMIOutQ2Input, 		 NTV2_WgtHDMIOut1v2,	 NTV2_CHANNEL1, kHDMINickname, 1},
	{ NTV2_XptHDMIOutQ3Input, 		 NTV2_WgtHDMIOut1v2,	 NTV2_CHANNEL1, kHDMINickname, 2},
	{ NTV2_XptHDMIOutQ4Input, 		 NTV2_WgtHDMIOut1v2,	 NTV2_CHANNEL1, kHDMINickname, 3},
	{ NTV2_Xpt4KDCQ1Input, 			 NTV2_Wgt4KDownConverter,NTV2_CHANNEL1, k4KDownConvertNickname, 0},
	{ NTV2_Xpt4KDCQ2Input, 			 NTV2_Wgt4KDownConverter,NTV2_CHANNEL2, k4KDownConvertNickname, 0},
	{ NTV2_Xpt4KDCQ3Input, 			 NTV2_Wgt4KDownConverter,NTV2_CHANNEL3, k4KDownConvertNickname, 0},
	{ NTV2_Xpt4KDCQ4Input, 			 NTV2_Wgt4KDownConverter,NTV2_CHANNEL4, k4KDownConvertNickname, 0},
	{ NTV2_Xpt425Mux1AInput, 		 NTV2_Wgt425Mux1, 		 NTV2_CHANNEL1, kTSIMuxNickname, 0},
	{ NTV2_Xpt425Mux1BInput, 		 NTV2_Wgt425Mux1, 		 NTV2_CHANNEL1, kTSIMuxNickname, 1},
	{ NTV2_Xpt425Mux2AInput, 		 NTV2_Wgt425Mux2, 		 NTV2_CHANNEL2, kTSIMuxNickname, 0},
	{ NTV2_Xpt425Mux2BInput, 		 NTV2_Wgt425Mux2, 		 NTV2_CHANNEL2, kTSIMuxNickname, 1},
	{ NTV2_Xpt425Mux3AInput, 		 NTV2_Wgt425Mux3, 		 NTV2_CHANNEL3, kTSIMuxNickname, 0},
	{ NTV2_Xpt425Mux3BInput, 		 NTV2_Wgt425Mux3, 		 NTV2_CHANNEL3, kTSIMuxNickname, 1},
	{ NTV2_Xpt425Mux4AInput, 		 NTV2_Wgt425Mux4, 		 NTV2_CHANNEL4, kTSIMuxNickname, 0},
	{ NTV2_Xpt425Mux4BInput, 		 NTV2_Wgt425Mux4, 		 NTV2_CHANNEL4, kTSIMuxNickname, 1},
	{ NTV2_XptAnalogOutInput, 		 NTV2_WgtAnalogOut1, 	 NTV2_CHANNEL1, kAnalogNickname, 0},
	{ NTV2_Xpt3DLUT1Input, 			 NTV2_Wgt3DLUT1, 		 NTV2_CHANNEL1, kLUT3DNickname, 0},
	{ NTV2_XptAnalogOutCompositeOut, NTV2_WgtAnalogCompositeOut1, NTV2_CHANNEL1, kCompositeNickname, 0},
	{ NTV2_XptStereoLeftInput, 		 NTV2_WgtStereoCompressor, NTV2_CHANNEL1, kStereoCompNickname, 0},
	{ NTV2_XptStereoRightInput, 	 NTV2_WgtStereoCompressor, NTV2_CHANNEL1, kStereoCompNickname, 0},
	//{ NTV2_XptProAmpInput,???,NTV2_CHANNEL1,"",0 },
	//{ NTV2_XptIICT1Input,???,NTV2_CHANNEL1,"",0 },
	{ NTV2_XptWaterMarker1Input, NTV2_WgtWaterMarker1, NTV2_CHANNEL1, kWatermarkNickname, 0},
	{ NTV2_XptWaterMarker2Input, NTV2_WgtWaterMarker2, NTV2_CHANNEL2, kWatermarkNickname, 0},
	//{ NTV2_XptUpdateRegister,???,NTV2_CHANNEL1,"",0 },
	{ NTV2_XptConversionMod2Input, NTV2_WgtUpDownConverter2, NTV2_CHANNEL2, kUpDownConvertNickname, 0},
	{ NTV2_XptCompressionModInput, NTV2_WgtCompression1, NTV2_CHANNEL1, kCompressionNickname, 0},
	{ NTV2_XptConversionModInput, NTV2_WgtUpDownConverter1, NTV2_CHANNEL1, kUpDownConvertNickname, 0},
	//{ NTV2_XptCSC1KeyFromInput2,???,NTV2_CHANNEL1,"",0 },
	{ NTV2_XptFrameSync2Input, NTV2_WgtFrameSync2, NTV2_CHANNEL2, kFrameSyncNickname, 0},
};

// Table of firmware widget's output crosspoint/id/channel/name/datastream index
static WidgetOutputSocket kWidgetOutputSockets[] = {
	{ NTV2_OUTPUT_CROSSPOINT_INVALID, NTV2_WIDGET_INVALID, NTV2_CHANNEL_INVALID, "", -1},
	{ NTV2_XptBlack, NTV2_WgtUndefined, NTV2_CHANNEL1, kBlackNickname, 0},
	{ NTV2_XptSDIIn1, NTV2_WgtSDIIn1, NTV2_CHANNEL1, kSDINickname, 0},
	{ NTV2_XptSDIIn2, NTV2_WgtSDIIn2, NTV2_CHANNEL2, kSDINickname, 0},
	{ NTV2_XptLUT1YUV, NTV2_WgtLUT1, NTV2_CHANNEL1, kSDINickname, 0},
	{ NTV2_XptCSC1VidYUV, NTV2_WgtCSC1, NTV2_CHANNEL1, kCSCNickname, 0},
	{ NTV2_XptConversionModule, NTV2_WgtUpDownConverter1, NTV2_CHANNEL1, kUpDownConvertNickname, 0},
	{ NTV2_XptCompressionModule, NTV2_WgtCompression1, NTV2_CHANNEL1, kCompressionNickname, 0},
	{ NTV2_XptFrameBuffer1YUV, NTV2_WgtFrameBuffer1, NTV2_CHANNEL1, kFramebufferNickname, 0},
	{ NTV2_XptFrameSync1YUV, NTV2_WgtFrameSync1, NTV2_CHANNEL1, kFrameSyncNickname, 0},
	{ NTV2_XptFrameSync2YUV, NTV2_WgtFrameSync2, NTV2_CHANNEL2, kFrameSyncNickname, 0},
	{ NTV2_XptDuallinkOut1, NTV2_WgtDualLinkV2Out1, NTV2_CHANNEL1, kDualLinkOutNickname, 0},
	//NTV2_XptAlphaOut,???,,,,,
	// ,???,,,,,
	{ NTV2_XptCSC1KeyYUV, NTV2_WgtCSC1, NTV2_CHANNEL1, kCSCNickname, 2},
	{ NTV2_XptFrameBuffer2YUV, NTV2_WgtFrameBuffer2, NTV2_CHANNEL2, kFramebufferNickname, 0},
	{ NTV2_XptCSC2VidYUV, NTV2_WgtCSC2, NTV2_CHANNEL2, kCSCNickname, 0},
	{ NTV2_XptCSC2KeyYUV, NTV2_WgtCSC2, NTV2_CHANNEL2, kCSCNickname, 2},
	{ NTV2_XptMixer1VidYUV, NTV2_WgtMixer1, NTV2_CHANNEL1, kMixerNickname, 0},
	{ NTV2_XptMixer1KeyYUV, NTV2_WgtMixer1, NTV2_CHANNEL1, kMixerNickname, 1},
	{ NTV2_XptMultiLinkOut1DS1, NTV2_WgtMultiLinkOut1, NTV2_CHANNEL1, kMultiLinkNickname, 0},
	{ NTV2_XptMultiLinkOut1DS2, NTV2_WgtMultiLinkOut1, NTV2_CHANNEL1, kMultiLinkNickname, 1},
	{ NTV2_XptAnalogIn, NTV2_WgtAnalogIn1, NTV2_CHANNEL1, kAnalogNickname, 0},
	{ NTV2_XptHDMIIn1, NTV2_WgtHDMIIn1, NTV2_CHANNEL1, kHDMINickname, 0},
	{ NTV2_XptMultiLinkOut1DS3, NTV2_WgtMultiLinkOut1, NTV2_CHANNEL1, kMultiLinkNickname, 2},
	{ NTV2_XptMultiLinkOut1DS4, NTV2_WgtMultiLinkOut1, NTV2_CHANNEL1, kMultiLinkNickname, 3},
	{ NTV2_XptMultiLinkOut2DS1, NTV2_WgtMultiLinkOut2, NTV2_CHANNEL2, kMultiLinkNickname, 0},
	{ NTV2_XptMultiLinkOut2DS2, NTV2_WgtMultiLinkOut2, NTV2_CHANNEL2, kMultiLinkNickname, 1},
	{ NTV2_XptDuallinkOut2, NTV2_WgtDualLinkV2Out2, NTV2_CHANNEL2, kDualLinkOutNickname, 0},
	{ NTV2_XptTestPatternYUV, NTV2_WgtTestPattern1, NTV2_CHANNEL1, kTestPatternNickname, 0},
	{ NTV2_XptSDIIn1DS2, NTV2_Wgt3GSDIIn1, NTV2_CHANNEL1, kSDINickname, 1},
	{ NTV2_XptSDIIn2DS2, NTV2_Wgt3GSDIIn2, NTV2_CHANNEL2, kSDINickname, 1},
	{ NTV2_XptMixer2VidYUV, NTV2_WgtMixer2, NTV2_CHANNEL2, kMixerNickname, 0},
	{ NTV2_XptMixer2KeyYUV, NTV2_WgtMixer2, NTV2_CHANNEL2, kMixerNickname, 1},
	// //NTV2_XptDCIMixerVidY,???,,,,,
	{ NTV2_XptStereoCompressorOut, NTV2_WgtStereoCompressor, NTV2_CHANNEL1, kStereoCompNickname, 0},
	{ NTV2_XptFrameBuffer3YUV, NTV2_WgtFrameBuffer3, NTV2_CHANNEL3, kFramebufferNickname, 0},
	{ NTV2_XptFrameBuffer4YUV, NTV2_WgtFrameBuffer4, NTV2_CHANNEL4, kFramebufferNickname, 0},
	{ NTV2_XptDuallinkOut1DS2, NTV2_WgtDualLinkV2Out1, NTV2_CHANNEL1, kDualLinkOutNickname, 1},
	{ NTV2_XptDuallinkOut2DS2, NTV2_WgtDualLinkV2Out2, NTV2_CHANNEL2, kDualLinkOutNickname, 1},
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	{ NTV2_XptCSC5VidYUV, NTV2_WgtCSC5, NTV2_CHANNEL5, kCSCNickname, 0},
	{ NTV2_XptCSC5KeyYUV, NTV2_WgtCSC5, NTV2_CHANNEL5, kCSCNickname, 1},
	{ NTV2_XptMultiLinkOut2DS3, NTV2_WgtMultiLinkOut2, NTV2_CHANNEL2, kMultiLinkNickname, 2},
	{ NTV2_XptMultiLinkOut2DS4, NTV2_WgtMultiLinkOut2, NTV2_CHANNEL2, kMultiLinkNickname, 3},
	{ NTV2_XptSDIIn3, NTV2_Wgt3GSDIIn3, NTV2_CHANNEL3, kSDINickname, 0},
	{ NTV2_XptSDIIn4, NTV2_Wgt3GSDIIn4, NTV2_CHANNEL4, kSDINickname, 0},
	{ NTV2_XptSDIIn3DS2, NTV2_Wgt3GSDIIn3, NTV2_CHANNEL3, kSDINickname, 1},
	{ NTV2_XptSDIIn4DS2, NTV2_Wgt3GSDIIn4, NTV2_CHANNEL4, kSDINickname, 1},
	// // ,???,,,,,
	// // ,???,,,,,
	{ NTV2_XptDuallinkOut3, NTV2_WgtDualLinkV2Out3, NTV2_CHANNEL3, kDualLinkOutNickname, 0},
	{ NTV2_XptDuallinkOut3DS2, NTV2_WgtDualLinkV2Out3, NTV2_CHANNEL3, kDualLinkOutNickname, 1},
	{ NTV2_XptDuallinkOut4, NTV2_WgtDualLinkV2Out4, NTV2_CHANNEL4, kDualLinkOutNickname, 0},
	{ NTV2_XptDuallinkOut4DS2, NTV2_WgtDualLinkV2Out4, NTV2_CHANNEL4, kDualLinkOutNickname, 1},
	{ NTV2_XptCSC3VidYUV, NTV2_WgtCSC3, NTV2_CHANNEL3, kCSCNickname, 0},
	{ NTV2_XptCSC3KeyYUV, NTV2_WgtCSC3, NTV2_CHANNEL3, kCSCNickname, 2},
	{ NTV2_XptCSC4VidYUV, NTV2_WgtCSC4, NTV2_CHANNEL4, kCSCNickname, 0},
	{ NTV2_XptCSC4KeyYUV, NTV2_WgtCSC4, NTV2_CHANNEL4, kCSCNickname, 2},
	{ NTV2_XptDuallinkOut5, NTV2_WgtDualLinkV2Out5, NTV2_CHANNEL5, kDualLinkOutNickname, 0},
	{ NTV2_XptDuallinkOut5DS2, NTV2_WgtDualLinkV2Out5, NTV2_CHANNEL5, kDualLinkOutNickname, 1},
	{ NTV2_Xpt3DLUT1YUV, NTV2_Wgt3DLUT1, NTV2_CHANNEL1, kLUT3DNickname, 0},
	{ NTV2_XptHDMIIn1Q2, NTV2_WgtHDMIIn1v3, NTV2_CHANNEL1, kHDMINickname, 1},
	{ NTV2_XptHDMIIn1Q3, NTV2_WgtHDMIIn1v3, NTV2_CHANNEL1, kHDMINickname, 2},
	{ NTV2_XptHDMIIn1Q4, NTV2_WgtHDMIIn1v3, NTV2_CHANNEL1, kHDMINickname, 3},
	{ NTV2_Xpt4KDownConverterOut, NTV2_Wgt4KDownConverter, NTV2_CHANNEL1, k4KDownConvertNickname, 0 },
	{ NTV2_XptSDIIn5, NTV2_Wgt3GSDIIn5, NTV2_CHANNEL5, kSDINickname, 0},
	{ NTV2_XptSDIIn6, NTV2_Wgt3GSDIIn6, NTV2_CHANNEL6, kSDINickname, 0},
	{ NTV2_XptSDIIn5DS2, NTV2_Wgt3GSDIIn5, NTV2_CHANNEL5, kSDINickname, 1},
	{ NTV2_XptSDIIn6DS2, NTV2_Wgt3GSDIIn6, NTV2_CHANNEL6, kSDINickname, 1},
	{ NTV2_XptSDIIn7, NTV2_Wgt3GSDIIn7, NTV2_CHANNEL7, kSDINickname, 0},
	{ NTV2_XptSDIIn8, NTV2_Wgt3GSDIIn8, NTV2_CHANNEL8, kSDINickname, 0},
	{ NTV2_XptSDIIn7DS2, NTV2_Wgt3GSDIIn7, NTV2_CHANNEL7, kSDINickname, 1},
	{ NTV2_XptSDIIn8DS2, NTV2_Wgt3GSDIIn8, NTV2_CHANNEL8, kSDINickname, 1},
	// ,???,,,,,
	// ,???,,,,,
	// ,???,,,,,
	// ,???,,,,,
	{ NTV2_XptFrameBuffer5YUV, NTV2_WgtFrameBuffer5, NTV2_CHANNEL5, kFramebufferNickname, 0},
	{ NTV2_XptFrameBuffer6YUV, NTV2_WgtFrameBuffer6, NTV2_CHANNEL6, kFramebufferNickname, 0},
	{ NTV2_XptFrameBuffer7YUV, NTV2_WgtFrameBuffer7, NTV2_CHANNEL7, kFramebufferNickname, 0},
	{ NTV2_XptFrameBuffer8YUV, NTV2_WgtFrameBuffer8, NTV2_CHANNEL8, kFramebufferNickname, 0},
	{ NTV2_XptMixer3VidYUV, NTV2_WgtMixer3, NTV2_CHANNEL3, kMixerNickname, 0},
	{ NTV2_XptMixer3KeyYUV, NTV2_WgtMixer3, NTV2_CHANNEL3, kMixerNickname, 1},
	{ NTV2_XptMixer4VidYUV, NTV2_WgtMixer4, NTV2_CHANNEL4, kMixerNickname, 0},
	{ NTV2_XptMixer4KeyYUV, NTV2_WgtMixer4, NTV2_CHANNEL4, kMixerNickname, 1},
	{ NTV2_XptCSC6VidYUV, NTV2_WgtCSC6, NTV2_CHANNEL6, kCSCNickname, 0},
	{ NTV2_XptCSC6KeyYUV, NTV2_WgtCSC6, NTV2_CHANNEL6, kCSCNickname, 1},
	{ NTV2_XptCSC7VidYUV, NTV2_WgtCSC7, NTV2_CHANNEL7, kCSCNickname, 0},
	{ NTV2_XptCSC7KeyYUV, NTV2_WgtCSC7, NTV2_CHANNEL7, kCSCNickname, 1},
	{ NTV2_XptCSC8VidYUV, NTV2_WgtCSC8, NTV2_CHANNEL8, kCSCNickname, 0},
	{ NTV2_XptCSC8KeyYUV, NTV2_WgtCSC8, NTV2_CHANNEL8, kCSCNickname, 1},
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	{ NTV2_XptDuallinkOut6, NTV2_WgtDualLinkV2Out6, NTV2_CHANNEL6, kDualLinkOutNickname, 0},
	{ NTV2_XptDuallinkOut6DS2, NTV2_WgtDualLinkV2Out6, NTV2_CHANNEL6, kDualLinkOutNickname, 1},
	{ NTV2_XptDuallinkOut7, NTV2_WgtDualLinkV2Out7, NTV2_CHANNEL7, kDualLinkOutNickname, 0},
	{ NTV2_XptDuallinkOut7DS2, NTV2_WgtDualLinkV2Out7, NTV2_CHANNEL7, kDualLinkOutNickname, 1},
	{ NTV2_XptDuallinkOut8, NTV2_WgtDualLinkV2Out8, NTV2_CHANNEL8, kDualLinkOutNickname, 0},
	{ NTV2_XptDuallinkOut8DS2, NTV2_WgtDualLinkV2Out8, NTV2_CHANNEL8, kDualLinkOutNickname, 1},
	{ NTV2_Xpt425Mux1AYUV, NTV2_Wgt425Mux1, NTV2_CHANNEL1, kTSIMuxNickname, 0},
	{ NTV2_Xpt425Mux1BYUV, NTV2_Wgt425Mux1, NTV2_CHANNEL1, kTSIMuxNickname, 1},
	{ NTV2_Xpt425Mux2AYUV, NTV2_Wgt425Mux2, NTV2_CHANNEL2, kTSIMuxNickname, 0},
	{ NTV2_Xpt425Mux2BYUV, NTV2_Wgt425Mux2, NTV2_CHANNEL2, kTSIMuxNickname, 1},
	{ NTV2_Xpt425Mux3AYUV, NTV2_Wgt425Mux3, NTV2_CHANNEL3, kTSIMuxNickname, 0},
	{ NTV2_Xpt425Mux3BYUV, NTV2_Wgt425Mux3, NTV2_CHANNEL3, kTSIMuxNickname, 1},
	{ NTV2_Xpt425Mux4AYUV, NTV2_Wgt425Mux4, NTV2_CHANNEL4, kTSIMuxNickname, 0},
	{ NTV2_Xpt425Mux4BYUV, NTV2_Wgt425Mux4, NTV2_CHANNEL4, kTSIMuxNickname, 1},
	{ NTV2_XptFrameBuffer1_DS2YUV, NTV2_WgtFrameBuffer1, NTV2_CHANNEL1, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer2_DS2YUV, NTV2_WgtFrameBuffer2, NTV2_CHANNEL2, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer3_DS2YUV, NTV2_WgtFrameBuffer3, NTV2_CHANNEL3, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer4_DS2YUV, NTV2_WgtFrameBuffer4, NTV2_CHANNEL4, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer5_DS2YUV, NTV2_WgtFrameBuffer5, NTV2_CHANNEL5, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer6_DS2YUV, NTV2_WgtFrameBuffer6, NTV2_CHANNEL6, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer7_DS2YUV, NTV2_WgtFrameBuffer7, NTV2_CHANNEL7, kFramebufferNickname, 1},
	{ NTV2_XptFrameBuffer8_DS2YUV, NTV2_WgtFrameBuffer8, NTV2_CHANNEL8, kFramebufferNickname, 1},
	{ NTV2_XptHDMIIn2, NTV2_WgtHDMIIn2v4, NTV2_CHANNEL2, kHDMINickname, 0},
	{ NTV2_XptHDMIIn2Q2, NTV2_WgtHDMIIn2v4, NTV2_CHANNEL2, kHDMINickname, 1},
	{ NTV2_XptHDMIIn2Q3, NTV2_WgtHDMIIn2v4, NTV2_CHANNEL2, kHDMINickname, 2},
	{ NTV2_XptHDMIIn2Q4, NTV2_WgtHDMIIn2v4, NTV2_CHANNEL2, kHDMINickname, 3},
	{ NTV2_XptHDMIIn3, NTV2_WgtHDMIIn3v4, NTV2_CHANNEL3, kHDMINickname, 0},
	{ NTV2_XptHDMIIn4, NTV2_WgtHDMIIn4v4, NTV2_CHANNEL4, kHDMINickname, 0},
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	{ NTV2_XptDuallinkIn1, NTV2_WgtDualLinkV2In1, NTV2_CHANNEL1, kDualLinkInNickname, 0},
	{ NTV2_XptLUT1Out, NTV2_WgtLUT1, NTV2_CHANNEL1, kLUTNickname, 0},
	{ NTV2_XptCSC1VidRGB, NTV2_WgtCSC1, NTV2_CHANNEL1, kCSCNickname, 1},
	// // ,???,,,,,
	// // ,???,,,,,
	{ NTV2_XptFrameBuffer1RGB, NTV2_WgtFrameBuffer1, NTV2_CHANNEL1, kFramebufferNickname, 2},
	{ NTV2_XptFrameSync1RGB, NTV2_WgtFrameSync1, NTV2_CHANNEL1, kFrameSyncNickname, 1},
	{ NTV2_XptFrameSync2RGB, NTV2_WgtFrameSync2, NTV2_CHANNEL2, kFrameSyncNickname, 1},
	// // ,???,,,,,
	// // ,???,,,,,
	{ NTV2_XptLUT2Out, NTV2_WgtLUT2, NTV2_CHANNEL2, kLUTNickname, 0},
	// // ,???,,,,,
	{ NTV2_XptFrameBuffer2RGB, NTV2_WgtFrameBuffer2, NTV2_CHANNEL2, kFramebufferNickname, 2},
	{ NTV2_XptCSC2VidRGB, NTV2_WgtCSC2, NTV2_CHANNEL2, kCSCNickname, 1},
	// // ,???,
	{ NTV2_XptMixer1VidRGB, NTV2_WgtMixer1, NTV2_CHANNEL1, kMixerNickname, 1},
	// // ,???,,,,,
	// // NTV2_XptWaterMarkerRGB,???,
	// // NTV2_XptIICTRGB,NTV2_WgtIICT1,
	// // ,???,,,,,
	{ NTV2_XptHDMIIn1RGB, NTV2_WgtHDMIIn1v3, NTV2_CHANNEL1, kHDMINickname, 2},
	// // ,???,,,,,
	// // ,???,,,,,
	// // NTV2_XptWaterMarker2RGB,???,
	// // NTV2_XptIICT2RGB,NTV2_WgtIICT2,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // NTV2_XptDCIMixerVidRGB,???,
	// // ,???,,,,,
	{ NTV2_XptFrameBuffer3RGB, NTV2_WgtFrameBuffer3, NTV2_CHANNEL3, kFramebufferNickname, 2},
	{ NTV2_XptFrameBuffer4RGB, NTV2_WgtFrameBuffer4, NTV2_CHANNEL4, kFramebufferNickname, 2},
	// // ,???,,,,,
	// // ,???,,,,,
	{ NTV2_XptDuallinkIn2, NTV2_WgtDualLinkV2In2, NTV2_CHANNEL2, kDualLinkInNickname, 0},
	{ NTV2_XptLUT3Out, NTV2_WgtLUT3, NTV2_CHANNEL3, kLUTNickname, 0},
	{ NTV2_XptLUT4Out, NTV2_WgtLUT4, NTV2_CHANNEL4, kLUTNickname, 0},
	{ NTV2_XptLUT5Out, NTV2_WgtLUT5, NTV2_CHANNEL5, kLUTNickname, 0},
	{ NTV2_XptCSC5VidRGB, NTV2_WgtCSC5, NTV2_CHANNEL5, kCSCNickname, 2},
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	{ NTV2_XptDuallinkIn3, NTV2_WgtDualLinkV2In3, NTV2_CHANNEL3, kDualLinkInNickname, 0},
	{ NTV2_XptDuallinkIn4, NTV2_WgtDualLinkV2In4, NTV2_CHANNEL4, kDualLinkInNickname, 0},
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	{ NTV2_XptCSC3VidRGB, NTV2_WgtCSC3, NTV2_CHANNEL3, kCSCNickname, 2},
	//  //  ,???,,,,,
	{ NTV2_XptCSC4VidRGB, NTV2_WgtCSC4, NTV2_CHANNEL4, kCSCNickname, 2},
	//  //  ,???,,,,,
	//   //  ,???,,,,,
	//   //  ,???,,,,,
	{ NTV2_Xpt3DLUT1RGB, NTV2_Wgt3DLUT1, NTV2_CHANNEL1, kLUT3DNickname, 1},
	{ NTV2_XptHDMIIn1Q2RGB, NTV2_WgtHDMIIn1v3, NTV2_CHANNEL1, kHDMINickname, 1},
	{ NTV2_XptHDMIIn1Q3RGB, NTV2_WgtHDMIIn1v3, NTV2_CHANNEL1, kHDMINickname, 2},
	{ NTV2_XptHDMIIn1Q4RGB, NTV2_WgtHDMIIn1v3, NTV2_CHANNEL1, kHDMINickname, 3},
	{ NTV2_Xpt4KDownConverterOutRGB, NTV2_Wgt4KDownConverter, NTV2_CHANNEL1, k4KDownConvertNickname, 1},
	//  //  ,???,,,,,
	//   //  ,???,,,,,
	//   //  ,???,,,,,
	//   //  ,???,,,,,
	//   //  ,???,,,,,
	//   //  ,???,,,,,
	// / / ,???,,,,,
	// // ,???,,,,,
	{ NTV2_XptDuallinkIn5, NTV2_WgtDualLinkV2In5, NTV2_CHANNEL5, kDualLinkInNickname, 0},
	{ NTV2_XptDuallinkIn6, NTV2_WgtDualLinkV2In6, NTV2_CHANNEL6, kDualLinkInNickname, 0},
	{ NTV2_XptDuallinkIn7, NTV2_WgtDualLinkV2In7, NTV2_CHANNEL7, kDualLinkInNickname, 0},
	{ NTV2_XptDuallinkIn8, NTV2_WgtDualLinkV2In8, NTV2_CHANNEL8, kDualLinkInNickname, 0},
	{ NTV2_XptFrameBuffer5RGB, NTV2_WgtFrameBuffer5, NTV2_CHANNEL5, kFramebufferNickname, 2},
	{ NTV2_XptFrameBuffer6RGB, NTV2_WgtFrameBuffer6, NTV2_CHANNEL6, kFramebufferNickname, 2},
	{ NTV2_XptFrameBuffer7RGB, NTV2_WgtFrameBuffer7, NTV2_CHANNEL7, kFramebufferNickname, 2},
	{ NTV2_XptFrameBuffer8RGB, NTV2_WgtFrameBuffer8, NTV2_CHANNEL8, kFramebufferNickname, 2},
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	{ NTV2_XptCSC6VidRGB, NTV2_WgtCSC6, NTV2_CHANNEL6, kCSCNickname, 1},
	// // ,???,,,,,
	{ NTV2_XptCSC7VidRGB, NTV2_WgtCSC7, NTV2_CHANNEL7, kCSCNickname, 1},
	// // ,???,,,,,
	{ NTV2_XptCSC8VidRGB, NTV2_WgtCSC8, NTV2_CHANNEL8, kCSCNickname, 1},
	// // ,???,,,,,
	{ NTV2_XptLUT6Out, NTV2_WgtLUT6, NTV2_CHANNEL6, kLUTNickname, 0},
	{ NTV2_XptLUT7Out, NTV2_WgtLUT7, NTV2_CHANNEL7, kLUTNickname, 0},
	{ NTV2_XptLUT8Out, NTV2_WgtLUT8, NTV2_CHANNEL8, kLUTNickname, 0},
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	// // ,???,,,,,
	{ NTV2_Xpt425Mux1ARGB, NTV2_Wgt425Mux1, NTV2_CHANNEL1, kTSIMuxNickname, 2},
	{ NTV2_Xpt425Mux1BRGB, NTV2_Wgt425Mux1, NTV2_CHANNEL1, kTSIMuxNickname, 3},
	{ NTV2_Xpt425Mux2ARGB, NTV2_Wgt425Mux2, NTV2_CHANNEL2, kTSIMuxNickname, 2},
	{ NTV2_Xpt425Mux2BRGB, NTV2_Wgt425Mux2, NTV2_CHANNEL2, kTSIMuxNickname, 3},
	{ NTV2_Xpt425Mux3ARGB, NTV2_Wgt425Mux3, NTV2_CHANNEL3, kTSIMuxNickname, 2},
	{ NTV2_Xpt425Mux3BRGB, NTV2_Wgt425Mux3, NTV2_CHANNEL3, kTSIMuxNickname, 3},
	{ NTV2_Xpt425Mux4ARGB, NTV2_Wgt425Mux4, NTV2_CHANNEL4, kTSIMuxNickname, 2},
	{ NTV2_Xpt425Mux4BRGB, NTV2_Wgt425Mux4, NTV2_CHANNEL4, kTSIMuxNickname, 3},
	{ NTV2_XptFrameBuffer1_DS2RGB, NTV2_WgtFrameBuffer1, NTV2_CHANNEL1, kFramebufferNickname, 3},
	{ NTV2_XptFrameBuffer2_DS2RGB, NTV2_WgtFrameBuffer2, NTV2_CHANNEL2, kFramebufferNickname, 3},
	{ NTV2_XptFrameBuffer3_DS2RGB, NTV2_WgtFrameBuffer3, NTV2_CHANNEL3, kFramebufferNickname, 3},
	{ NTV2_XptFrameBuffer4_DS2RGB, NTV2_WgtFrameBuffer4, NTV2_CHANNEL4, kFramebufferNickname, 3},
	{ NTV2_XptFrameBuffer5_DS2RGB, NTV2_WgtFrameBuffer5, NTV2_CHANNEL5, kFramebufferNickname, 3},
	{ NTV2_XptFrameBuffer6_DS2RGB, NTV2_WgtFrameBuffer6, NTV2_CHANNEL6, kFramebufferNickname, 3},
	{ NTV2_XptFrameBuffer7_DS2RGB, NTV2_WgtFrameBuffer7, NTV2_CHANNEL7, kFramebufferNickname, 3},
	{ NTV2_XptFrameBuffer8_DS2RGB, NTV2_WgtFrameBuffer8, NTV2_CHANNEL8, kFramebufferNickname, 3},
	{ NTV2_XptHDMIIn2RGB, NTV2_WgtHDMIIn2v4, NTV2_CHANNEL2, kHDMINickname, 0},
	{ NTV2_XptHDMIIn2Q2RGB, NTV2_WgtHDMIIn2v4, NTV2_CHANNEL2, kHDMINickname, 1},
	{ NTV2_XptHDMIIn2Q3RGB, NTV2_WgtHDMIIn2v4, NTV2_CHANNEL2, kHDMINickname, 2},
	{ NTV2_XptHDMIIn2Q4RGB, NTV2_WgtHDMIIn2v4, NTV2_CHANNEL2, kHDMINickname, 3},
	{ NTV2_XptHDMIIn3RGB, NTV2_WgtHDMIIn3v4, NTV2_CHANNEL3, kHDMINickname, 0},
	{ NTV2_XptHDMIIn4RGB, NTV2_WgtHDMIIn4v4, NTV2_CHANNEL4, kHDMINickname, 0},
	// ,???,,,,,
};
// clang-format on

static const size_t kNumWidgetInputSockets =
	(sizeof(kWidgetInputSockets) / sizeof(WidgetInputSocket));
static const size_t kNumWidgetOutputSockets =
	(sizeof(kWidgetOutputSockets) / sizeof(WidgetOutputSocket));

bool WidgetInputSocket::Find(const std::string &name, NTV2Channel channel,
			     int32_t datastream, WidgetInputSocket &inp)
{
	// std::cout << "DEBUG -- WidgetInputSocket::Find: name = " << name
	// 	  << ", chan = " << NTV2ChannelToString(channel)
	// 	  << ", datastream = " << datastream << std::endl;
	for (const auto &in : kWidgetInputSockets) {
		if (name == in.name && channel == in.channel &&
		    datastream == in.datastream_index) {
			inp = in;
			return true;
		}
	}

	return false;
}

bool WidgetInputSocket::GetWidgetInputSocketByXpt(InputXpt id,
						  WidgetInputSocket &inp)
{
	for (const auto &in : kWidgetInputSockets) {
		if (in.id == id) {
			inp = in;
			return true;
		}
	}

	return false;
}

int32_t WidgetInputSocket::InputXptDatastreamIndex(InputXpt xpt)
{
	int32_t datastream = 0;
	for (auto &x : kWidgetInputSockets) {
		if (x.id == xpt) {
			datastream = x.datastream_index;
			break;
		}
	}
	return datastream;
}

NTV2Channel WidgetInputSocket::InputXptChannel(InputXpt xpt)
{
	NTV2Channel channel = NTV2_CHANNEL_INVALID;
	for (auto &x : kWidgetInputSockets) {
		if (x.id == xpt) {
			channel = x.channel;
			break;
		}
	}
	return channel;
}

const char *WidgetInputSocket::InputXptName(InputXpt xpt)
{
	const char *name = NULL;
	for (auto &x : kWidgetInputSockets) {
		if (x.id == xpt) {
			name = x.name;
			break;
		}
	}
	return name;
}

bool WidgetOutputSocket::Find(const std::string &name, NTV2Channel channel,
			      int32_t datastream, WidgetOutputSocket &out)
{
	// std::cout << "DEBUG -- WidgetOutputSocket::Find: name = " << name
	// 	  << ", chan = " << NTV2ChannelToString(channel)
	// 	  << ", datastream = " << datastream << std::endl;
	for (const auto &wo : kWidgetOutputSockets) {
		if (name == wo.name && channel == wo.channel &&
		    datastream == wo.datastream_index) {
			out = wo;
			return true;
		}
	}

	return false;
}

bool WidgetOutputSocket::GetWidgetOutputSocketByXpt(OutputXpt id,
						    WidgetOutputSocket &out)
{
	for (const auto &wo : kWidgetOutputSockets) {
		if (wo.id == id) {
			out = wo;
			return true;
		}
	}

	return false;
}

int32_t WidgetOutputSocket::OutputXptDatastreamIndex(OutputXpt xpt)
{
	int32_t datastream = 0;
	for (auto &x : kWidgetOutputSockets) {
		if (x.id == xpt) {
			datastream = x.datastream_index;
			break;
		}
	}
	return datastream;
}

NTV2Channel WidgetOutputSocket::OutputXptChannel(OutputXpt xpt)
{
	NTV2Channel channel = NTV2_CHANNEL_INVALID;
	for (auto &x : kWidgetOutputSockets) {
		if (x.id == xpt) {
			channel = x.channel;
			break;
		}
	}
	return channel;
}

const char *WidgetOutputSocket::OutputXptName(OutputXpt xpt)
{
	const char *name = NULL;
	for (auto &x : kWidgetOutputSockets) {
		if (x.id == xpt) {
			name = x.name;
			break;
		}
	}
	return name;
}
