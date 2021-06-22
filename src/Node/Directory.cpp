//
// Created by Elyas EL IDRISSI on 22/06/2021.
//

#include "Node/File.h"

#include "Node/Directory.h"

Directory::Directory() : Node(Node::Type::DIRECTORY)
{
	void();
}

Directory::Directory(const boost::filesystem::path &path) : Node(Node::Type::DIRECTORY, path)
{
	void();
}

size_t Directory::get_size()
{
	size_t size = 0;
	boost::system::error_code ec;

	for (auto & p : boost::filesystem::recursive_directory_iterator(get_path(), ec))
	{
		const boost::filesystem::path & path = p.path();
		if (Node::get_guessed_type(path) == Node::FILE) {File f(path) ; size += f.get_size() ;}
	}

	return size;
}