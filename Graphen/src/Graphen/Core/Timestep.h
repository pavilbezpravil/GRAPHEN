#pragma once

namespace gn {

	class Timestep
	{
	public:
		Timestep(double time = 0.0f)
			: m_Time(time)
		{
		}

		operator float() const { return (float)m_Time; }
		operator double() const { return m_Time; }

		float GetSeconds() const { return (float)m_Time; }
		double GetSecondsDouble() const { return m_Time; }
		float GetMilliseconds() const { return (float)(m_Time * 1000.0f); }
		double GetMillisecondsDouble() const { return m_Time * 1000.0f; }
	private:
		double m_Time;
	};

}