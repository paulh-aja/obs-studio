#include "aja-timecode.hpp"
#include "ajabase/common/timecode.h"
#include "ajabase/system/systemtime.h"
#include "ajantv2/includes/ntv2publicinterface.h"
#include "ajaanc/includes/ancillarydata_timecode.h"

#include <chrono>

namespace aja {

void SerializeRP188(const AJAAncillaryData_Timecode &tc, uint32_t &loWord,
		    uint32_t &hiWord)
{
	uint8_t hourTens, hourUnits, minTens, minUnits, secTens, secUnits,
		frameTens, frameUnits;
	uint8_t bg1, bg2, bg3, bg4, bg5, bg6, bg7, bg8;
	tc.GetTimeDigits(hourTens, hourUnits, minTens, minUnits, secTens,
			 secUnits, frameTens, frameUnits);
	tc.GetBinaryGroups(bg8, bg7, bg6, bg5, bg4, bg3, bg2, bg1);

	bool dropFrame = false;
	tc.GetDropFrameFlag(dropFrame);

	bool colorFrame = false;
	tc.GetColorFrameFlag(colorFrame);

	bool fieldID = false;
	tc.GetFieldIdFlag(fieldID);

	loWord = 0;
	loWord |= (frameUnits & kFrameUnitsMask) << kFrameUnitsShift;
	loWord |= (bg1 & kBinaryGroup1Mask) << kBinaryGroup1Shift;
	loWord |= (frameTens & kFrameTensMask) << kFrameTensShift;
	loWord |= ((uint8_t)dropFrame & kDropFrameMask) << kDropFrameShift;
	loWord |= ((uint8_t)colorFrame & kColorFrameMask) << kColorFrameShift;
	loWord |= (bg2 & kBinaryGroup2Mask) << kBinaryGroup2Shift;
	loWord |= (secUnits & kSecondUnitsMask) << kSecondUnitsShift;
	loWord |= (bg3 & kBinaryGroup3Mask) << kBinaryGroup3Shift;
	loWord |= (secTens & kSecondTensMask) << kSecondTensShift;
	loWord |= (fieldID & kFieldIdLoMask) << kFieldIdLoShift;
	loWord |= (bg4 & kBinaryGroup4Mask) << kBinaryGroup4Shift;

	uint8_t bgFlag;
	tc.GetBinaryGroupFlag(bgFlag);

	hiWord = 0;
	hiWord |= (minUnits & kMinuteUnitsMask) << kMinuteUnitsShift;
	hiWord |= (bg5 & kBinaryGroup5Mask) << kBinaryGroup5Shift;
	hiWord |= (minTens & kMinuteTensMask) << kMinuteTensShift;
	hiWord |= (fieldID & kFieldIdHiMask) << kFieldIdHiShift;
	hiWord |= (bg6 & kBinaryGroup6Mask) << kBinaryGroup6Shift;
	hiWord |= (hourUnits & kHourUnitsMask) << kHourUnitsShift;
	hiWord |= (bg7 & kBinaryGroup7Mask) << kBinaryGroup7Shift;
	hiWord |= (hourTens & kHourTensMask) << kHourTensShift;
	hiWord |= (bgFlag & kBinaryGroupFlagMask) << kBinaryGroupFlagShift;
	hiWord |= (bg8 & kBinaryGroup8Mask) << kBinaryGroup8Shift;
}

TimecodeGenerator::TimecodeGenerator()
	: mTimecodeSource{TimecodeSource::None},
	  mTimeBase{},
	  mTimeDiff{0},
	  mIsDropFrame{false},
	  mHalfRateHFR{false}
{
}

TimecodeGenerator::TimecodeGenerator(TimecodeSource src, const AJATimeBase &tb,
				     bool isHalfRateHFR)
	: mTimecodeSource{src},
	  mTimeBase{tb},
	  mTimeDiff{0},
	  mIsDropFrame{tb.IsNonIntegralRatio()},
	  mHalfRateHFR{isHalfRateHFR}
{
	if (mTimecodeSource == TimecodeSource::TimeOfDay) {
		SetTimeDiff();
	}
}

AJAStatus TimecodeGenerator::GetFrameNum(int64_t &frame, uint64_t timestamp) const
{
	if (mTimecodeSource == TimecodeSource::TimeOfDay) {
		int64_t t = timestamp;
		if (t != 0)
			t = (int64_t)AJATime::GetSystemMicroseconds();
		frame = mTimeBase.MicrosecondsToFrames(t + mTimeDiff);
		return AJA_STATUS_SUCCESS;
	}
	return AJA_STATUS_UNSUPPORTED;
}

AJAStatus TimecodeGenerator::GetTimecode(AJATimeCode &tc, uint64_t timestamp) const
{
	AJAStatus status = AJA_STATUS_UNSUPPORTED;
	tc.SetStdTimecodeForHfr(mHalfRateHFR);
	if (mTimecodeSource == TimecodeSource::TimeOfDay) {
		int64_t frame = 0;
		status = GetFrameNum(frame, timestamp);
		tc.Set((uint32_t)frame);
	}
	return status;
}

AJAStatus TimecodeGenerator::GetRP188(NTV2_RP188 &rp188, uint64_t timestamp) const
{
	AJATimeCode tc;
	AJAStatus status = GetTimecode(tc, timestamp);
	if (status == AJA_STATUS_SUCCESS) {
		AJAAncillaryData_Timecode adtc;
		status = adtc.SetTimecode(tc, mTimeBase, mIsDropFrame);
		if (status == AJA_STATUS_SUCCESS) {
			aja::SerializeRP188(adtc, rp188.fLo, rp188.fHi);
		}
	}
	return status;
}

AJAStatus TimecodeGenerator::GetString(std::string &tcStr, uint64_t timestamp) const
{
	AJATimeCode tc;
	AJAStatus status = GetTimecode(tc, timestamp);
	tc.QueryString(tcStr, mTimeBase, mIsDropFrame);
	return status;
}

TimecodeSource TimecodeGenerator::GetTimecodeSource() const
{
	return mTimecodeSource;
}

bool TimecodeGenerator::IsDropFrame() const
{
	return mIsDropFrame;
}

void TimecodeGenerator::SetDropFrame(bool isDropFrame)
{
	mIsDropFrame = isDropFrame;
}

void TimecodeGenerator::SetHalfRateHFR(bool isHalfRateHFR)
{
	mHalfRateHFR = isHalfRateHFR;
}

bool TimecodeGenerator::IsHalfRateHFR() const
{
	return mHalfRateHFR;
}

int64_t TimecodeGenerator::microsElapsedToday()
{
	using clock = std::chrono::system_clock;
	auto now = clock::now();
	time_t time_now = clock::to_time_t(now);
	tm *date = std::localtime(&time_now);
	date->tm_hour = 0;
	date->tm_min = 0;
	date->tm_sec = 0;
	auto midnight = clock::from_time_t(std::mktime(date));
	auto elapsed = now - midnight;
	return std::chrono::duration_cast<std::chrono::microseconds>(elapsed)
		.count();
}

AJATimeBase TimecodeGenerator::GetTimeBase() const
{
	return mTimeBase;
}

void TimecodeGenerator::SetTimeDiff(uint64_t timestamp)
{
	int64_t ts = (int64_t)timestamp;
	if (ts == 0)
		ts = (int64_t)AJATime::GetSystemMicroseconds();
	mTimeDiff = microsElapsedToday() - ts;
}

} // aja
