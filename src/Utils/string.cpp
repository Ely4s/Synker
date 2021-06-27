//
// Created by Elyas EL IDRISSI on 27/06/2021.
//

#include "Utils/string.h"

namespace Utils
{
	std::string quoted(const std::string & str)
	{
		return '"' + str + '"';
	}
};
