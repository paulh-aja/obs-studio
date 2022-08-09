#pragma once

#include "aja-enums.hpp"
#include "ajabase/common/timecode.h"

class NTV2_RP188;
class AJAAncillaryData_Timecode;
class AJATimeBase;

namespace aja {

typedef enum {
	kFrameUnitsMask = 0xf,
	kBinaryGroup1Mask = 0xf,
	kFrameTensMask = 0x3,
	kDropFrameMask = 0x1,
	kColorFrameMask = 0x1,
	kBinaryGroup2Mask = 0xf,
	kSecondUnitsMask = 0xf,
	kBinaryGroup3Mask = 0xf,
	kSecondTensMask = 0x7,
	kFieldIdLoMask = 0x1,
	kBinaryGroup4Mask = 0xf
} Smpte12LoWordMask;

typedef enum {
	kFrameUnitsShift = 0,
	kBinaryGroup1Shift = 4,
	kFrameTensShift = 8,
	kDropFrameShift = 10,
	kColorFrameShift = 11,
	kBinaryGroup2Shift = 12,
	kSecondUnitsShift = 16,
	kBinaryGroup3Shift = 20,
	kSecondTensShift = 24,
	kFieldIdLoShift = 27,
	kBinaryGroup4Shift = 28
} Smpte12mLoWordShift;

typedef enum {
	kMinuteUnitsMask = 0xf,
	kBinaryGroup5Mask = 0xf,
	kMinuteTensMask = 0x7,
	kFieldIdHiMask = 0x1,
	kBinaryGroup6Mask = 0xf,
	kHourUnitsMask = 0xf,
	kBinaryGroup7Mask = 0xf,
	kHourTensMask = 0x3,
	kBinaryGroupFlagMask = 0x3,
	kBinaryGroup8Mask = 0xf
} Smpte12mHiWordMask;

typedef enum {
	kMinuteUnitsShift = 0,
	kBinaryGroup5Shift = 4,
	kMinuteTensShift = 8,
	kFieldIdHiShift = 11,
	kBinaryGroup6Shift = 12,
	kHourUnitsShift = 16,
	kBinaryGroup7Shift = 20,
	kHourTensShift = 24,
	kBinaryGroupFlagShift = 26,
	kBinaryGroup8Shift = 28
} Smpte12mHiWordShift;

extern void SerializeRP188(const AJAAncillaryData_Timecode &tc,
			   uint32_t &loWord, uint32_t &hiWord);

class TimecodeGenerator {
public:
	explicit TimecodeGenerator();
	explicit TimecodeGenerator(TimecodeSource src, const AJATimeBase &tb,
				   bool isHalfRateHFR = true);
	virtual AJAStatus GetFrameNum(int64_t &frame, uint64_t timestamp = 0) const;
	virtual AJAStatus GetTimecode(AJATimeCode &tc, uint64_t timestamp = 0) const;
	virtual AJAStatus GetRP188(NTV2_RP188 &rp188, uint64_t timestamp = 0) const;
	virtual AJAStatus GetString(std::string &tcStr, uint64_t timestamp = 0) const;
	virtual AJATimeBase GetTimeBase() const;
	virtual TimecodeSource GetTimecodeSource() const;
	virtual void SetDropFrame(bool isDropFrame);
	virtual bool IsDropFrame() const;
	virtual void SetHalfRateHFR(bool isHalfRateHFR);
	virtual bool IsHalfRateHFR() const;
	virtual void SetTimeDiff(uint64_t timestamp = 0);
protected:
	virtual int64_t microsElapsedToday();

	TimecodeSource mTimecodeSource;
	mutable AJATimeBase mTimeBase;
	int64_t mTimeDiff;
	bool mIsDropFrame;
	bool mHalfRateHFR;
};

} // aja
