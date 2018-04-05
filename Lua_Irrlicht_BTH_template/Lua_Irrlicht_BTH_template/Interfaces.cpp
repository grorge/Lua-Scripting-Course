#include "Interfaces.h"

#include <math.h>

Interface::Interface()
{
}

Interface::Interface(irr::video::IVideoDriver * driver, irr::scene::ISceneManager * smgr, irr::gui::IGUIEnvironment * guienv)
{
	this->driver = driver;
	this->smgr = smgr;
	this->guienv = guienv;

	this->IDs = 100;
}

Interface::~Interface()
{
}

std::string Interface::addMesh(Vertex * verts)
{
	Object* testObj;
	testObj = new Object(this->smgr->getRootSceneNode(), this->smgr, this->IDs, verts, 3);

	this->nodes.push_back(testObj);

	this->IDs++;
	
	return "Simple Triangle in the plane.";
}

std::string Interface::addBox(irr::core::vector3d<irr::s32> pos, int size)
{
	std::string generatedName = "name" + std::to_string( this->IDs);
	
	return addBox(pos, size, generatedName);
}

std::string Interface::addBox(irr::core::vector3d<irr::s32> pos, int size, std::string name)
{
	Vertex v[24] =
	{
		// Front
		Vertex(pos.X - 1.0f * size, pos.Y - 1.0f * size, pos.Z - 1.0f * size) ,
		Vertex(pos.X - 1.0f * size, pos.Y + 1.0f * size, pos.Z - 1.0f * size),
		Vertex(pos.X + 1.0f * size, pos.Y + 1.0f * size, pos.Z - 1.0f * size),
		Vertex(pos.X + 1.0f * size, pos.Y - 1.0f * size, pos.Z - 1.0f * size),

		// Back
		Vertex(pos.X - 1.0f * size, pos.Y - 1.0f * size, pos.Z + 1.0f * size),
		Vertex(pos.X + 1.0f * size, pos.Y - 1.0f * size, pos.Z + 1.0f * size),
		Vertex(pos.X + 1.0f * size, pos.Y + 1.0f * size, pos.Z + 1.0f * size),
		Vertex(pos.X - 1.0f * size, pos.Y + 1.0f * size, pos.Z + 1.0f * size),

		// Top
		Vertex(pos.X - 1.0f * size, pos.Y + 1.0f * size, pos.Z - 1.0f * size),
		Vertex(pos.X - 1.0f * size, pos.Y + 1.0f * size, pos.Z + 1.0f * size),
		Vertex(pos.X + 1.0f * size, pos.Y + 1.0f * size, pos.Z + 1.0f * size),
		Vertex(pos.X + 1.0f * size, pos.Y + 1.0f * size, pos.Z - 1.0f * size),

		// Bottom
		Vertex(pos.X - 1.0f * size, pos.Y - 1.0f * size, pos.Z - 1.0f * size),
		Vertex(pos.X + 1.0f * size, pos.Y - 1.0f * size, pos.Z - 1.0f * size),
		Vertex(pos.X + 1.0f * size, pos.Y - 1.0f * size, pos.Z + 1.0f * size),
		Vertex(pos.X - 1.0f * size, pos.Y - 1.0f * size, pos.Z + 1.0f * size),

		// Left
		Vertex(pos.X - 1.0f * size, pos.Y - 1.0f * size, pos.Z + 1.0f * size),
		Vertex(pos.X - 1.0f * size, pos.Y + 1.0f * size, pos.Z + 1.0f * size),
		Vertex(pos.X - 1.0f * size, pos.Y + 1.0f * size, pos.Z - 1.0f * size),
		Vertex(pos.X - 1.0f * size, pos.Y - 1.0f * size, pos.Z - 1.0f * size),

		// Right
		Vertex(pos.X + 1.0f * size, pos.Y - 1.0f * size, pos.Z - 1.0f * size),
		Vertex(pos.X + 1.0f * size, pos.Y + 1.0f * size, pos.Z - 1.0f * size),
		Vertex(pos.X + 1.0f * size, pos.Y + 1.0f * size, pos.Z + 1.0f * size),
		Vertex(pos.X + 1.0f * size, pos.Y - 1.0f * size, pos.Z + 1.0f * size),
	};
		
	Box* testObj;
	testObj = new Box(this->smgr->getRootSceneNode(), this->smgr, this->IDs, v, 24, name);

	this->nodes.push_back(testObj);

	this->IDs++;



	return std::string();
}

std::string Interface::getNodes()
{
	std::string returnTable = "Boxes: \n";

	for (int i = 0; i < this->nodes.size(); i++)
	{
		std::string name = "NaN";
		if(dynamic_cast<Box*>(this->nodes[i]))
			name = dynamic_cast<Box*>(this->nodes[i])->getName();
		returnTable += std::to_string( this->nodes[i]->getID()) + "	" + name 
			+ "\n";
	}



	return returnTable;
}

std::string Interface::camera(irr::core::vector3df pos, irr::core::vector3df target)
{
	smgr->addCameraSceneNode(0, pos, target);
	
	return std::string();
}
