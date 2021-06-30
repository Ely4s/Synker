//
// Created by Elyas EL IDRISSI on 26/06/2021.
//

#ifndef SYNKER_PROFILER_H
#define SYNKER_PROFILER_H

#include <vector>

#include <boost/filesystem.hpp>

/**
 * struct 'ProfilerResult' store result of method 'profile()' of class 'Profiler'.
 * So, it contain valid and invalid descendants nodes path
 */
struct ProfilerResult
{
	const std::vector<boost::filesystem::path> valid_paths;
	const std::vector<boost::filesystem::path> invalid_paths;
};

/**
 * class 'Profiler' is used to recover all the folders and files, with write and read rights,
 * contained in the source folder and the target folder.
 */
class Profiler
{

public:

	static ProfilerResult profile(const boost::filesystem::path & path);

private:

	static void directory_verification(const boost::filesystem::path & directory_path);
	static void iterate(const boost::filesystem::path & directory_path, std::vector<boost::filesystem::path> & valid_paths, std::vector<boost::filesystem::path> & invalid_paths);
	static void remove_duplicated_paths(const boost::filesystem::path & path, std::vector<boost::filesystem::path> & valid_paths, std::vector<boost::filesystem::path> & invalid_paths);
};

#endif //SYNKER_PROFILER_H