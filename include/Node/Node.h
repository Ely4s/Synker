//
// Created by Elyas EL IDRISSI on 22/06/2021.
//

#ifndef SYNKER_NODE_H
#define SYNKER_NODE_H

#include <string>
#include <vector>
#include <cstddef>

#include <boost/filesystem.hpp>

/**
 * class 'Node' represents an element of the filesystem,
 * which can be a file or a directory (these two only as for now)
 */
class Node
{
	public:

	enum Type {NODE, FILE, DIRECTORY, SYMLINK, ACCES_DENIED};
	enum PathType {DEFAULT, ABSOLUTE, RELATIVE};
	enum PermissionType {READ, WRITE, EXECUTE};
	enum PermissionOwner {USER, GROUP, OTHER};

	Node() = default;
	explicit Node(Type type);
	Node(Type type, const boost::filesystem::path & path);
	virtual ~Node() = default;

	static Type get_guessed_type(const boost::filesystem::path & path);

	boost::filesystem::path get_path(PathType path_type = DEFAULT);
	boost::filesystem::path get_absolute_path();
	boost::filesystem::path get_relative_path();
	void set_path(const boost::filesystem::path & path);

	static bool get_is_exists(const boost::filesystem::path & path);
	bool get_is_exists();

	std::string get_filename();
	std::string get_extention();

	const Type & get_type();
	std::string get_type_str();

	virtual size_t get_size() = 0;

	time_t get_last_write_date_time();
	time_t get_creation_date_time();
	std::string get_last_write_date_str();
	std::string get_creation_date_str();

	bool get_has_permissions(PermissionOwner owner, const std::vector<PermissionType> & types);

	bool get_is_hidden();

	virtual std::string to_string();

protected:

	void set_type(const Node::Type & type);

private:

	Type type = NODE;
	boost::filesystem::path path;
};

#endif //SYNKER_NODE_H