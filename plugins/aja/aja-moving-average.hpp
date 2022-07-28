#pragma once

#include <deque>
#include <limits>
#include <type_traits>

#include <ajabase/common/timer.h>

namespace aja {

template<typename T> class MovingAverage {
public:
	explicit MovingAverage(uint64_t max_samples = 1024)
		: _max_samples{max_samples},
		  _mean{0.0},
		  _M2{0.0},
		  _min_val{std::numeric_limits<int64_t>::max()},
		  _max_val{0},
		  _sum{0},
		  _samples{}
	{
	}
	~MovingAverage() = default;
	
	inline void AddSample(const T sample)
	{
		if (std::is_arithmetic<T>::value) {
			if (_sum >= std::numeric_limits<int64_t>::max()) {
				_sum = 0;
				_samples.clear();
			}
			if (_max_samples == 0 ||
			    _samples.size() < _max_samples) {
				int64_t sample_i64 = (int64_t)sample;
				if (sample_i64 > _max_val)
					_max_val = sample_i64;
				if ((int64_t)sample < _min_val)
					_min_val = sample;
				_samples.push_back(sample);
				_sum += sample;
			} else {
				_samples.pop_front();
			}
		}
	}
	
	inline double Average() {
		if (_samples.size() > 0) {
			return (double)_sum / (double)_samples.size();
		}
		return 0.0;
	}

	inline double Mean()
	{
		if (_samples.size() > 0) {
			T current = _samples.back();
			double delta = (double)current - _mean;
			_mean += delta / (double)_samples.size();
			double delta2 = (double)(current)-_mean;
			_M2 += delta * delta2;
			return _mean;
		} else {
			return 0.0;
		}
	}
	
	inline double Variance()
	{
		if (_samples.size() > 1)
			return (double)(_M2 / _samples.size());
		else
			return 0.0;
	}
	
	inline double SampleVariance()
	{
		if (_samples.size() > 1)
			return _M2 / (double)((_samples.size() - 1));
		else
			return 0.0;
	}

protected:
	uint64_t _max_samples;
	double _mean;
	double _M2;
	int64_t _min_val;
	int64_t _max_val;
	int64_t _sum;
	std::deque<T> _samples;
};

template<typename T> class IntervalAverage {
public:
	IntervalAverage(AJATimerPrecision timerPrecision =
				AJATimerPrecisionMilliseconds,
			uint32_t interval = 1000, uint64_t max_samples = 120)
		: mInterval{interval},
		  mTimer{timerPrecision},
		  mMovingAvg{max_samples}
	{
	}

	~IntervalAverage() = default;

	inline bool IsRunning() {
		return mTimer.IsRunning();
	}

	inline void BeginInterval()
	{
		mTimer.Reset();
		mTimer.Start();
	}

	inline bool EndInterval()
	{
		if (mTimer.ElapsedTime() >= mInterval) {
			mTimer.Stop();
			return true;
		}
		return false;
	}

	inline void AddSample(const T sample) {
		mMovingAvg.AddSample(sample);
	}

	inline double MovingAverage()
	{
		if (mTimer.ElapsedTime() >= mInterval)
			return mMovingAvg.Average();
		return 0.0;
	}

protected:
	uint32_t mInterval;
	AJATimer mTimer;
	aja::MovingAverage<T> mMovingAvg;
};

} // aja
