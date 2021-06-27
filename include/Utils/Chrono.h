//
// Created by Elyas EL IDRISSI on 27/06/2021.
//

#ifndef SYNKER_CHRONO_H
#define SYNKER_CHRONO_H

#include <chrono>

class Chrono
{

public:

	explicit Chrono(bool start = false);
	~Chrono() = default;

	void start();
	void stop();

	template<class T>
	double get_result();
	double get_result();

private:

	std::chrono::time_point<std::chrono::system_clock> m_start, m_end;

};

template<class T>
double Chrono::get_result()
{
	std::chrono::duration<double, T> elapsed_time = m_end - m_start;

	return elapsed_time.count();
}

#endif //SYNKER_CHRONO_H
