#include "../fbx.hpp"
#include "../simplem/SimpleMesh.hpp"
#include <iostream>

void printRecord(const Fbx::Record * record, size_t level = 0)
{
    std::cout << std::string(level * 3, ' ') << "Rec: " << record->name() << std::endl;

    for (auto p : record->properties())
    {
        std::cout << std::string((level + 1) * 3, ' ') << "Prop - " << p->code() << ": " << p->string() << std::endl;
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

    file.read(filename, versionCheck);

	std::vector<SimpleMesh>meshes;

    const auto objects = *file.find("Objects");
    for (const Fbx::Record *r : *objects)
    {
        std::cout << "************** " << r->name() << " **************\n";
		if (r->name() == "Geometry")
		{
		   SimpleMesh m;

		   //Parse Vertices
		   for (const Fbx::Property *p : (*(r->find("Vertices")))->properties())
		   {
               //std::cout << "****" << "Prop - Vertices " << p->code() << ": " << p->string() << ": " << p->size() << std::endl;
               const std::vector<Fbx::Property::Value> &xyz = p->array();
               const size_t numVertex = xyz.size() / 3;

			   m.coordinates.resize(numVertex);
               
               switch (p->code())  
               {
					case 'd':
					   for (size_t i=0; i < numVertex; i++)
					      m.coordinates[i].set(xyz[3*i].float64, xyz[3*i+1].float64, xyz[3*i+2].float64);
						break;
					case 'f':
					   for (size_t i=0; i < numVertex; i++)
					      m.coordinates[i].set(xyz[3*i].float32, xyz[3*i+1].float32, xyz[3*i+2].float32);
						break;
					default:
						throw std::runtime_error("Unknow vertex array type: "+p->code() );
				}//switch (p->code())
	       }//for (const Fbx::Property *p : (*(r->find("Vertices")))->properties())
	       
	       //Parse PolygonVertexIndex
		   for (const Fbx::Property *p : (*(r->find("PolygonVertexIndex")))->properties())
		   {
               //std::cout << "****" << "Prop - PolygonVertexIndex " << p->code() << ": " << p->string() << ": " << p->size() << std::endl;
               //const std::vector<Fbx::Property::Value> &idx = p->array();
			   //const size_t numIdx = idx.size();

 			   std::vector<int>index_aux;
			   index_aux.reserve(128);
               switch (p->code())  
               {
					case 'i':
						for (const Fbx::Property::Value &v : p->array())
						{
							if (v.integer32 >= 0)
							{
								//std::cout << v.integer32 << " ";
								index_aux.push_back(v.integer32);
								size_t j = index_aux.size();
								if (j >= 3)
								{
								m.triangles.emplace_back(index_aux[j-3],index_aux[j-2],index_aux[j-1]);
								//change orientation of shared edge with next vertex
								index_aux[j-2]=index_aux[j-3];
								}
							}
							else
							{
								//std::cout << -v.integer32-1 << " ";
								index_aux.push_back(-v.integer32-1);
								size_t j = index_aux.size();
								
								if (j >= 3)
									m.triangles.emplace_back(index_aux[j-3],index_aux[j-2],index_aux[j-1]);
								//Reset face index
							    index_aux.resize(0);
							}
					    }//for

						break;
					case 'l':
						for (const Fbx::Property::Value &v : p->array())
						{
							if (v.integer64 >= 0)
							{
								//std::cout << v.integer32 << " ";
								index_aux.push_back(v.integer64);
								size_t j = index_aux.size();
								if (j >= 3)
								{
								m.triangles.emplace_back(index_aux[j-3],index_aux[j-2],index_aux[j-1]);
								//change orientation of shared edge with next vertex
								index_aux[j-2]=index_aux[j-3];
								}
							}
							else
							{
								//std::cout << -v.integer32-1 << " ";
								index_aux.push_back(-v.integer64-1);
								size_t j = index_aux.size();
								
								if (j >= 3)
									m.triangles.emplace_back(index_aux[j-3],index_aux[j-2],index_aux[j-1]);
								//Reset face index
							    index_aux.resize(0);
							}
					    }//for

						break;
					default:
						throw std::runtime_error("Unknow PolygonVertexIndex array type: "+p->code() );
				}//switch (p->code())
	       }//for (const Fbx::Property *p : (*(r->find("PolygonVertexIndex")))->properties())

	       //Parse LayerElementUV
		   /*
		   for (const Fbx::Property *p : (*(r->find("LayerElementUV")))->properties())
		   {
               std::cout << "****" << "Prop - LayerElementUV " << p->code() << ": " << p->string() << ": " << p->size() << std::endl;
               //const std::vector<Fbx::Property::Value> &idx = p->array();
			   //const size_t numIdx = idx.size();
	       }//for (const Fbx::Property *p : (*(r->find("PolygonVertexIndex")))->properties())
		   */

		meshes.push_back(m);
	       
	   }//if (r->name() == "Geometry")
	   
    }//for (const Fbx::Record *r : *objects)

    std::cout << meshes.size() << " meshes\n";
	int nm=0;
	for (const SimpleMesh &m : meshes)
	{
		std::string filename("mesh"+std::to_string(nm++)+".ply");
	    std::cout << " * " << filename << " : " << m.numVertex() << " vertex " << m.numTriangles() << " triangles\n";
		m.writeFilePLY(filename, false);
	}
    //file.write("../bin/out-model.fbx");

    return 0;
}
