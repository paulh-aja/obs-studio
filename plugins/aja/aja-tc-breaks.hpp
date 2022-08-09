#pragma once
#include <stdint.h>
#include <vector>

namespace aja {

class TimecodeBreakFinder {
public:
	using TimecodePair = std::pair<uint64_t, std::string>;
	TimecodeBreakFinder()
		: _frame_count{0}, _last_frame_num{0}, _tc_breaks{}
	{
	}

	// void NextFrame(uint64_t num, const std::string &tc_str)
	void Clear()
	{
		_tc_breaks.clear();
	}
	void NextFrame(uint64_t num)
	{
		if (_frame_count > 0) {
			if (num != _last_frame_num + 1)
				_tc_breaks.push_back(num);
		}
		_last_frame_num = num;
		_frame_count++;
	}
	uint64_t LastFrame() const { return _last_frame_num; }
	uint64_t FirstBreak() const
	{
		if (!TimecodeBroke()) {
			return _tc_breaks.front();
		}
		return 0;
	}
	uint64_t BreakCount() const { return _tc_breaks.size(); }
	bool TimecodeBroke() const { return !_tc_breaks.empty(); }

private:
	uint64_t _frame_count;
	uint64_t _last_frame_num;
	std::vector <uint64_t> _tc_breaks;
	// std::vector<TimecodePair> _tc_breaks;
};

} // aja
