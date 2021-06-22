//
// Created by Elyas EL IDRISSI on 22/06/2021.
//

#ifndef SYNKER_DIRECTORY_H
#define SYNKER_DIRECTORY_H

#include "Node/Node.h"

/**
 * class 'Directory' inherits from class 'Node'
 * So, it's a specification of the class 'Node' and it represent a directory.
 */
class Directory : public Node
{

public:

	Directory();
	~Directory() = default;
	explicit Directory(const boost::filesystem::path& path);
	size_t get_size() override;

private:



};

#endif //SYNKER_DIRECTORY_H
