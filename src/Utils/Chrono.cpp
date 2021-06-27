//
// Created by Elyas EL IDRISSI on 27/06/2021.
//

#include "Utils/Chrono.h"

Chrono::Chrono(bool start)
{
	if(start) Chrono::start();
}

void Chrono::start()
{
	m_start = std::chrono::system_clock::now();
	m_end = m_start;
}

void Chrono::stop()
{
	m_end = std::chrono::system_clock::now();
}

double Chrono::get_result()
{
	return get_result<std::ratio<1>>();
}