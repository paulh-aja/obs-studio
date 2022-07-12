#pragma once

#include <deque>
#include <limits>
#include <type_traits>

#include <ajabase/common/timer.h>

namespace aja {

template<typename T> class MovingAverage {
public:
	MovingAverage()
		: _maxSampleSize{0},
		  _mean{0.0},
		  _M2{0.0},
		  _min_val{std::numeric_limits<int64_t>::max()},
		  _max_val{0},
		  _samples{}
	{
	}
	MovingAverage(uint64_t maxSampleSize)
		: _maxSampleSize{maxSampleSize},
		  _mean{0.0},
		  _M2{0.0},
		  _min_val{std::numeric_limits<int64_t>::max()},
		  _max_val{0},
		  _samples{}
	{
	}
	virtual ~MovingAverage() = default;
	void AddSample(const T &sample)
	{
		if (std::is_arithmetic<T>::value) {
			if (_maxSampleSize == 0 ||
			    _samples.size() < _maxSampleSize) {
				int64_t sample_i64 = (int64_t)sample;
				if (sample_i64 > _max_val)
					_max_val = sample_i64;
				if ((int64_t)sample < _min_val)
					_min_val = sample;
				_samples.push_back(sample);
			} else {
				_samples.pop_front();
			}
		}
	}
	double Mean()
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
	double Variance()
	{
		if (_samples.size() > 1)
			return (double)(_M2 / _samples.size());
		else
			return 0.0;
	}
	double SampleVariance()
	{
		if (_samples.size() > 1)
			return _M2 / (double)((_samples.size() - 1));
		else
			return 0.0;
	}

private:
	uint64_t _maxSampleSize;
	double _mean;
	double _M2;
	int64_t _min_val;
	int64_t _max_val;
	std::deque<T> _samples;
};

template<typename T> class IntervalAverage {
public:
	IntervalAverage(AJATimerPrecision timerPrecision =
				AJATimerPrecisionMilliseconds,
			uint32_t interval = 1000, uint64_t maxSampleSize = 120)
		: mCount{},
		  mInterval{interval},
		  mTimer{timerPrecision},
		  mMovingAvg{maxSampleSize}
	{
	}

	void BeginInterval()
	{
		mTimer.Reset();
		mTimer.Start();
	}

	bool EndInterval()
	{
		if (mTimer.ElapsedTime() >= mInterval) {
			mTimer.Stop();
			mMovingAvg.AddSample(mCount);
			mCount = 0;
			return true;
		}
		return false;
	}

	void Tick() { mCount++; }

	double MovingAverage()
	{
		if (mTimer.ElapsedTime() >= mInterval)
			return mMovingAvg.Mean();
		return 0.0;
	}

private:
	T mCount;
	uint32_t mInterval;
	AJATimer mTimer;
	aja::MovingAverage<T> mMovingAvg;
};

} // aja
