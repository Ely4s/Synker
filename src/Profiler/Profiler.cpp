//
// Created by Elyas EL IDRISSI on 26/06/2021.
//

#include <chrono>

#include <boost/format.hpp>
#include "spdlog/spdlog.h"

#include "Node/Node.h"
#include "Node/File.h"
#include "Node/Directory.h"
#include "Utils/string.h"

#include "Profiler/Profiler.h"

std::pair<std::vector<boost::filesystem::path>, std::vector<boost::filesystem::path>> Profiler::profile(const boost::filesystem::path & path)
{
	std::vector<boost::filesystem::path> valid_paths;
	std::vector<boost::filesystem::path> unvalid_paths;

	//path not refers to a directory
	if(Node::get_guessed_type(path) != Node::DIRECTORY)
	{
		std::string message = (boost::format("[profiling : %1%] invalid profiling root directory") % path).str();
		spdlog::error(message);
		throw std::runtime_error(message);
	}

	Directory root_directory(path);

	//root_directory to profile hasn't user read and write permissions
	if(!root_directory.get_has_permissions(Node::USER, {Node::READ, Node::WRITE}))
	{
		std::string message = (boost::format("[profiling : %1%] profiling root directory don't have read and write rights") % root_directory.get_path()).str();
		spdlog::error(message);
		throw std::runtime_error(message);
	}

	spdlog::info("[profiling : {}] start of profiling", Utils::quoted(root_directory.get_path().string()));

	std::chrono::time_point<std::chrono::system_clock> start_time_profiling, end_time_profiling;
	start_time_profiling = std::chrono::system_clock::now();

	spdlog::info("[profiling : {}] start of profiling iteration", Utils::quoted(root_directory.get_path().string()));

	std::chrono::time_point<std::chrono::system_clock> start_time_profiling_iteration, end_time_profiling_iteration;
	start_time_profiling_iteration = std::chrono::system_clock::now();

	boost::system::error_code ec, no_error;
	boost::filesystem::recursive_directory_iterator node_it (root_directory.get_path(), ec), node_end;

	//iterate over all root_directory descendants
	while(node_it != node_end)
	{
		//node refers to an access denied node
		if(Node::get_guessed_type(node_it->path()) == Node::ACCES_DENIED)
		{
			unvalid_paths.push_back(node_it->path());
			node_it.no_push();
			spdlog::warn("[profiling : {}] can't add node path {} to the profile : {}", Utils::quoted(root_directory.get_path().string()), Utils::quoted(node_it->path().string()), "Permission denied");
		}
		//node refers to an undefined node type
		if(Node::get_guessed_type(node_it->path()) == Node::NODE)
		{
			unvalid_paths.push_back(node_it->path());
			node_it.no_push();
			spdlog::warn("[profiling : {}] can't add node path {} to the profile : {}", Utils::quoted(root_directory.get_path().string()), Utils::quoted(node_it->path().string()), "Undefined node type");
		}
		//node refers to a symlink
		else if(Node::get_guessed_type(node_it->path()) == Node::SYMLINK)
		{
			unvalid_paths.push_back(node_it->path());
			node_it.no_push();
			spdlog::warn("[profiling : {}] can't add symlink file path {} to the profile : {}", Utils::quoted(root_directory.get_path().string()), Utils::quoted(node_it->path().string()), "Symlink not supported");
		}
		//node refers to a file
		else if(Node::get_guessed_type(node_it->path()) == Node::FILE)
		{
			File file(node_it->path());

			//file has user read and write permissions
			if(file.get_has_permissions(Node::USER, {Node::READ, Node::WRITE}))
			{
				valid_paths.push_back(file.get_path());
				spdlog::info("[profiling : {}] add file path {} to the profile", Utils::quoted(root_directory.get_path().string()), Utils::quoted(file.get_path().string()));
			}
			//file hasn't user read and write permissions
			else
			{
				unvalid_paths.push_back(file.get_path());
				node_it.no_push();
				spdlog::warn("[profiling : {}] can't add file path {} to the profile : {}", Utils::quoted(root_directory.get_path().string()), Utils::quoted(file.get_path().string()), "Permission denied");
			}
		}
		//node refers to a directory
		else if(Node::get_guessed_type(node_it->path()) == Node::DIRECTORY)
		{
			Directory directory(node_it->path());

			//try to access to directory's child nodes
			boost::system::error_code ec_tmp;
			boost::filesystem::recursive_directory_iterator node_tmp_it(directory.get_path(), ec_tmp);

			//can't access to directory child nodes
			if(ec_tmp == no_error)
			{
				valid_paths.push_back(directory.get_path());
				spdlog::info("[profiling : {}] add directory path {} to the profile", Utils::quoted(root_directory.get_path().string()), Utils::quoted(directory.get_path().string()));
			}
			//can access to directory child nodes
			else
			{
				unvalid_paths.push_back(directory.get_path());
				spdlog::warn("[profiling : {}] can't add directory path {} to the profile : {}", Utils::quoted(root_directory.get_path().string()), Utils::quoted(directory.get_path().string()), ec_tmp.message());
				//go to directory sibling node
				node_it.no_push();
			}
		}

		node_it.increment(ec);
	}

	end_time_profiling_iteration = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds_profiling_iteration = end_time_profiling_iteration - start_time_profiling_iteration;
	spdlog::info("[profiling : {}] end of profiling iteration ({} valid and {} unvalid node(s) found in {} second(s))", Utils::quoted(root_directory.get_path().string()), valid_paths.size(), unvalid_paths.size(), elapsed_seconds_profiling_iteration.count());

	spdlog::info("[profiling : {}] start of duplicated path removal", Utils::quoted(root_directory.get_path().string()));

	std::chrono::time_point<std::chrono::system_clock> start_time_duplicated_removal, end_time_duplicated_removal;
	start_time_duplicated_removal = std::chrono::system_clock::now();

	//remove all potentially duplicated paths
	std::vector<std::reference_wrapper<std::vector<boost::filesystem::path>>> paths_vector = {valid_paths, unvalid_paths};
	size_t size_before_removal = 0, size_after_removal = 0, nbr_removed_path = 0;
	for(auto & paths : paths_vector)
	{
		size_before_removal = paths.get().size();

		std::sort(paths.get().begin(), paths.get().end());
		auto last = std::unique(paths.get().begin(), paths.get().end());
		paths.get().erase(last, paths.get().end());

		size_after_removal = paths.get().size();
		nbr_removed_path += size_before_removal - size_after_removal;
	}

	end_time_duplicated_removal = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds_duplicated_removal = end_time_duplicated_removal - start_time_duplicated_removal;
	spdlog::info("[profiling : {}] end of duplicated path removal ({} path(s) removed in {} second(s))", Utils::quoted(root_directory.get_path().string()), nbr_removed_path, elapsed_seconds_duplicated_removal.count());

	end_time_profiling = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds_profiling = end_time_profiling - start_time_profiling;

	spdlog::info("[profiling : {}] end of profiling (done in {} second(s))", Utils::quoted(root_directory.get_path().string()), elapsed_seconds_profiling.count());

	return {valid_paths, unvalid_paths};
}