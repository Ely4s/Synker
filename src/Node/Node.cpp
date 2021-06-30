//
// Created by Elyas EL IDRISSI on 22/06/2021.
//

#include "Node/Node.h"

Node::Node(Node::Type type)
{
	set_type(type);
}

Node::Node(Node::Type type, const boost::filesystem::path & path) : Node(type)
{
	set_path(path);
}

bool Node::get_is_exists(const boost::filesystem::path & path)
{
	return boost::filesystem::exists(path);
}

bool Node::get_is_exists()
{
	return Node::get_is_exists(get_path());
}

Node::Type Node::get_guessed_type(const boost::filesystem::path &path)
{
	try
	{
		if (boost::filesystem::is_regular_file(path))	{return Node::Type::FILE;}
		if (boost::filesystem::is_directory(path))   	{return Node::Type::DIRECTORY;}
		if (boost::filesystem::is_symlink(path))		{return Node::Type::SYMLINK;}
		if (boost::filesystem::is_other(path))   		{return Node::Type::NOT_SUPPORTED;}

		return Node::Type::NODE_NOT_FOUND;
	}
	catch(const std::exception & e)
	{
		return Node::Type::ACCESS_DENIED;
	}
}

boost::filesystem::path Node::get_path(PathType path_type)
{
	switch (path_type)
	{
		case ABSOLUTE : return get_absolute_path();
		case RELATIVE : return get_relative_path();
		default       : return Node::path;
	}
}

boost::filesystem::path Node::get_absolute_path()
{
	return  boost::filesystem::canonical(Node::path);
}

boost::filesystem::path Node::get_relative_path()
{
	return  boost::filesystem::relative(Node::path);
}

void Node::set_path(const boost::filesystem::path & path)
{
	Node::path = path;
};

std::string Node::get_filename()
{
	return get_path().filename().string();
}

std::string Node::get_extention()
{
	return get_path().extension().string();
}

const Node::Type & Node::get_type()
{
	return Node::type;
}

std::string Node::get_type_str()
{
	switch (get_type())
	{
		case FILE          		: return "File";
		case DIRECTORY     		: return "Directory";
		case SYMLINK       		: return "Symlink";
		case NOT_SUPPORTED 		: return "not supported";
		case NODE_NOT_FOUND    	: return "node not found";
		case ACCESS_DENIED 		: return "access denied";
		default            		: return "Node";
	}
}

void Node::set_type(const Node::Type & type)
{
	Node::type = type;
}

time_t Node::get_creation_date_time()
{
	return boost::filesystem::creation_time(get_path());
}

time_t Node::get_last_write_date_time()
{
	return boost::filesystem::last_write_time(get_path());
}

std::string Node::get_creation_date_str()
{
	time_t time = get_creation_date_time();

	struct tm local_time{};
	localtime_r(&time, &local_time);
	char date_buffer[256];
	strftime(date_buffer, sizeof(date_buffer), "%d/%m/%Y %H:%M:%S", &local_time);

	return std::string(date_buffer);
}

std::string Node::get_last_write_date_str()
{
	time_t time = get_last_write_date_time();

	struct tm local_time{};
	localtime_r(&time, &local_time);
	char date_buffer[256];
	strftime(date_buffer, sizeof(date_buffer), "%d/%m/%Y %H:%M:%S", &local_time);

	return std::string(date_buffer);
}

bool Node::get_has_permissions(Node::PermissionOwner owner, const std::vector<PermissionType> & types)
{
	for(auto & type : types)
	{
		std::string permissions = std::bitset<9>(static_cast<int>(status(get_path()).permissions())).to_string();
		int permission_i = type + (owner * 3);

		if(permissions.at(permission_i) != '1') {return false ;}
	}

	return true;
}

bool Node::get_is_hidden()
{
	return get_path().leaf().c_str()[0] == '.';
}

std::string Node::to_string()
{
	return

	"{\n"
		"\tnode_type = " + get_type_str() + "\n"
		"\tfile_name : " + get_path().filename().string() + "\n"
		"\tsize = " + std::to_string(get_size()) + " byte(s)\n"
		"\tcreation_date = " + get_creation_date_str() + "\n"
		"\tlast_modification_date = " + get_last_write_date_str() + "\n"
		"\tvisibility = " + ((get_is_hidden()) ? "hidden" : "visible") + "\n"
	"}";
}
