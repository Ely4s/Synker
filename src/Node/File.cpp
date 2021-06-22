//
// Created by Elyas EL IDRISSI on 22/06/2021.
//

#include "Node/File.h"

File::File() : Node(Node::Type::FILE)
{
	void();
}

File::File(const boost::filesystem::path & path) : Node(Node::Type::FILE, path)
{
	void();
};

size_t File::get_size()
{
	return boost::filesystem::file_size(get_path());
}