//
// Created by Elyas EL IDRISSI on 26/06/2021.
//

#include <chrono>

#include <boost/format.hpp>
#include <boost/system/error_code.hpp>
#include "spdlog/spdlog.h"

#include "Node/Node.h"
#include "Node/File.h"
#include "Node/Directory.h"
#include "Utils/string.h"
#include "Utils/Chrono.h"

#include "Profiler/Profiler.h"

using namespace boost::system::errc;

ProfilerResult Profiler::profile(const boost::filesystem::path & path1, const boost::filesystem::path & path2)
{
	std::vector<ProfilerResultElement> result_elements;

	for(const auto & path : {path1, path2})
	{
		std::vector<boost::filesystem::path> valid_paths;
		std::vector<boost::filesystem::path> invalid_paths;

		spdlog::info("[profiling : {}] start of profiling", Utils::quoted(path.string()));
		Chrono profiling_chrono(true);

		directory_verification(path);
		iterate(path, valid_paths, invalid_paths);
		remove_duplicated_paths(path, valid_paths, invalid_paths);

		profiling_chrono.stop();
		spdlog::info("[profiling : {}] end of profiling (done in {:.6f} second(s))", Utils::quoted(path.string()), profiling_chrono.get_result());

		result_elements.push_back({std::move(valid_paths), std::move(invalid_paths)});
	}

	return {std::move(result_elements.at(0)), std::move(result_elements.at(1))};
}

void Profiler::directory_verification(const boost::filesystem::path & directory_path)
{
	spdlog::info("[profiling : {}] start of directory verification", Utils::quoted(directory_path.string()));
	Chrono directory_verification_chrono(true);

	//path not refers to a directory
	if(Node::get_guessed_type(directory_path) != Node::DIRECTORY)
	{
		directory_verification_chrono.stop();
		std::string message = (boost::format("[profiling : %1%] end of directory verification, directory %2% is invalid (done in %3$.6f second(s))") % directory_path % directory_path % directory_verification_chrono.get_result()).str();
		spdlog::error(message);
		throw std::runtime_error(message);
	}

	Directory root_directory(directory_path);

	//root_directory to profile hasn't user read and write permissions
	if(!root_directory.get_has_permissions(Node::USER, {Node::READ, Node::WRITE}))
	{
		directory_verification_chrono.stop();
		std::string message = (boost::format("[profiling : %1%] end of directory verification, directory %2% is invalid because it doesn't have read and write rights (done in %3$.6f second(s))") % directory_path % directory_path % directory_verification_chrono.get_result()).str();
		spdlog::error(message);
		throw std::runtime_error(message);
	}

	directory_verification_chrono.stop();
	spdlog::info("[profiling : {}] end of directory verification, directory {} is valid (done in {:.6f} second(s))", Utils::quoted(directory_path.string()), Utils::quoted(directory_path.string()), directory_verification_chrono.get_result());
}

void Profiler::iterate(const boost::filesystem::path & directory_path, std::vector<boost::filesystem::path> & valid_paths, std::vector<boost::filesystem::path> & invalid_paths)
{
	spdlog::info("[profiling : {}] start of profiling iteration", Utils::quoted(directory_path.string()));
	Chrono profiling_iteration_chrono(true);

	boost::system::error_code ec, no_error;
	boost::filesystem::recursive_directory_iterator node_it (directory_path, ec), node_end;

	//iterate over all directory_path descendants
	while(node_it != node_end)
	{
		Node::Type node_it_type = Node::get_guessed_type(node_it->path());

		//node refers to an access denied node
		if(node_it_type == Node::NODE_NOT_FOUND)
		{
			invalid_paths.push_back(node_it->path());
			node_it.no_push();
			spdlog::warn("[profiling : {}] can't add node path {} to the profile : {}", Utils::quoted(directory_path.string()), Utils::quoted(node_it->path().string()), "Not found");
		}
		//node refers to an access denied node
		if(node_it_type == Node::ACCESS_DENIED)
		{
			invalid_paths.push_back(node_it->path());
			node_it.no_push();
			spdlog::warn("[profiling : {}] can't add node path {} to the profile : {}", Utils::quoted(directory_path.string()), Utils::quoted(node_it->path().string()), make_error_code(permission_denied).message());
		}
		//node refers to an unsupported node type
		else if(node_it_type == Node::NOT_SUPPORTED)
		{
			invalid_paths.push_back(node_it->path());
			node_it.no_push();
			spdlog::warn("[profiling : {}] can't add node path {} to the profile : {}", Utils::quoted(directory_path.string()), Utils::quoted(node_it->path().string()), make_error_code(not_supported).message());
		}
		//node refers to a symlink
		else if(node_it_type == Node::SYMLINK)
		{
			invalid_paths.push_back(node_it->path());
			node_it.no_push();
			spdlog::warn("[profiling : {}] can't add symlink file path {} to the profile : {}", Utils::quoted(directory_path.string()), Utils::quoted(node_it->path().string()), make_error_code(not_supported).message());
		}
		//node refers to a file
		else if(node_it_type == Node::FILE)
		{
			File file(node_it->path());

			//file has user read and write permissions
			if(file.get_has_permissions(Node::USER, {Node::READ, Node::WRITE}))
			{
				valid_paths.push_back(file.get_path());
				spdlog::info("[profiling : {}] add file path {} to the profile", Utils::quoted(directory_path.string()), Utils::quoted(file.get_path().string()));
			}
			//file hasn't user read and write permissions
			else
			{
				invalid_paths.push_back(file.get_path());
				node_it.no_push();
				spdlog::warn("[profiling : {}] can't add file path {} to the profile : {}", Utils::quoted(directory_path.string()), Utils::quoted(file.get_path().string()), make_error_code(permission_denied).message());
			}
		}
		//node refers to a directory
		else if(node_it_type == Node::DIRECTORY)
		{
			Directory directory(node_it->path());

			//try to access to directory's child nodes
			boost::system::error_code ec_tmp;
			boost::filesystem::recursive_directory_iterator node_tmp_it(directory.get_path(), ec_tmp);

			//can't access to directory child nodes
			if(ec_tmp == no_error)
			{
				valid_paths.push_back(directory.get_path());
				spdlog::info("[profiling : {}] add directory path {} to the profile", Utils::quoted(directory_path.string()), Utils::quoted(directory.get_path().string()));
			}
			//can access to directory child nodes
			else
			{
				invalid_paths.push_back(directory.get_path());
				spdlog::warn("[profiling : {}] can't add directory path {} to the profile : {}", Utils::quoted(directory_path.string()), Utils::quoted(directory.get_path().string()), ec_tmp.message());
				//go to directory sibling node
				node_it.no_push();
			}
		}

		node_it.increment(ec);
	}

	profiling_iteration_chrono.stop();
	spdlog::info("[profiling : {}] end of profiling iteration ({} valid and {} invalid node(s) found in {:.6f} second(s))", Utils::quoted(directory_path.string()), valid_paths.size(), invalid_paths.size(), profiling_iteration_chrono.get_result());
}

void Profiler::remove_duplicated_paths(const boost::filesystem::path & path, std::vector<boost::filesystem::path> & valid_paths, std::vector<boost::filesystem::path> & invalid_paths)
{
	spdlog::info("[profiling : {}] start of duplicated path removal", Utils::quoted(path.string()));
	Chrono duplicated_removal_chrono(true);

	//remove all potentially duplicated paths
	std::vector<std::reference_wrapper<std::vector<boost::filesystem::path>>> paths_vector = {valid_paths, invalid_paths};
	size_t size_before_removal = 0, size_after_removal = 0, nbr_removed_path = 0;
	for(auto & paths : paths_vector)
	{
		size_before_removal = paths.get().size();

		std::sort(paths.get().begin(), paths.get().end());
		auto last = std::unique(paths.get().begin(), paths.get().end());
		paths.get().erase(last, paths.get().end());
		paths.get().shrink_to_fit();

		size_after_removal = paths.get().size();
		nbr_removed_path += size_before_removal - size_after_removal;
	}

	duplicated_removal_chrono.stop();
	spdlog::info("[profiling : {}] end of duplicated path removal ({} path(s) removed in {:.6f} second(s))", Utils::quoted(path.string()), nbr_removed_path, duplicated_removal_chrono.get_result());
}
