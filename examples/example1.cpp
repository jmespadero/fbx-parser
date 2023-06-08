#include "../fbx.hpp"
#include <iostream>

void printRecord(const Fbx::Record * record, size_t level = 0)
{
    std::cout << std::string(level * 3, ' ') << "Rec: " << record->name() << std::endl;

    for (auto p : record->properties())
    {
        std::cout << std::string((level + 1) * 3, ' ') << "Prop - " << p->code() << ": " << p->string().c_str() << std::endl;
    }
    for (auto r : *record)
    {
        printRecord(r, level + 1);
    }
}

int main (int argc, char *argv[])
{
    Fbx::Record file;
    auto versionCheck = [](std::string magic, uint32_t version)
    {
        if (magic != "Kaydara FBX Binary  ")
        {
            throw std::runtime_error("Invalid magic string.");
        }
        if (version < 7100)
        {
            throw std::runtime_error("I'm not interested in version less than 7100.");
        }
    };

	// Set default input mesh filename
	std::string filename("../models/blender-default.fbx");
	if (argc > 1)
		filename = std::string(argv[1]);

	// Read a fbx file from disk
	std::cout << "Reading file: " << filename << std::endl;
    file.read(filename, versionCheck);
    
	// Print file schema to console
    printRecord(&file);
    
	// Write file to disk
	std::cout << "Writting file: out-model.fbx" << std::endl;
    file.write("out-model.fbx");

    return 0;
}
