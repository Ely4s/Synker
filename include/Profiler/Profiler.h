//
// Created by Elyas EL IDRISSI on 26/06/2021.
//

#ifndef SYNKER_PROFILER_H
#define SYNKER_PROFILER_H

#include <vector>

#include <boost/filesystem.hpp>

/**
 * struct 'ProfilerResultElement' store result of method 'profile()' of class 'Profiler'.
 * So, it contain valid and invalid descendants nodes path
 */
struct ProfilerResultElement
{
	std::vector<boost::filesystem::path> valid_paths;
	std::vector<boost::filesystem::path> invalid_paths;
};

/**
 * struct 'ProfilerResult' store an object 'ProfilerResultElement'
 * for the source and an other one for the target.
 */
struct ProfilerResult
{
	ProfilerResultElement source;
	ProfilerResultElement target;
};

/**
 * class 'Profiler' is used to recover all the folders and files, with write and read rights,
 * contained in the source folder and the target folder.
 */
class Profiler
{

public:

	static ProfilerResult profile(const boost::filesystem::path & path1, const boost::filesystem::path & path2);

private:

	static void directory_verification(const boost::filesystem::path & directory_path);
	static void iterate(const boost::filesystem::path & directory_path, std::vector<boost::filesystem::path> & valid_paths, std::vector<boost::filesystem::path> & invalid_paths);
	static void remove_duplicated_paths(const boost::filesystem::path & path, std::vector<boost::filesystem::path> & valid_paths, std::vector<boost::filesystem::path> & invalid_paths);
};

#endif //SYNKER_PROFILER_H