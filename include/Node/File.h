//
// Created by Elyas EL IDRISSI on 22/06/2021.
//

#ifndef SYNKER_FILE_H
#define SYNKER_FILE_H

#include "Node/Node.h"

/**
 * class 'File' inherits from class 'Node'
 * So, it's a specification of the class 'Node' and it represent a regular file.
 */
class File : public Node
{

public:

	File();
	~File() = default;
	explicit File(const boost::filesystem::path& path);

	size_t get_size() override;

private:



};

#endif //SYNKER_FILE_H
