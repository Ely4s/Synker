//
// Created by Elyas EL IDRISSI on 26/06/2021.
//

#ifndef SYNKER_PROFILER_H
#define SYNKER_PROFILER_H

#include <vector>

#include <boost/filesystem.hpp>

/**
 * class 'Profiler' is used to recover all the folders and files, with write and read rights,
 * contained in the source folder and the target folder.
 */
class Profiler
{

public:

	static std::pair<std::vector<boost::filesystem::path>, std::vector<boost::filesystem::path>> profile(const boost::filesystem::path & path);

private:



};

#endif //SYNKER_PROFILER_H