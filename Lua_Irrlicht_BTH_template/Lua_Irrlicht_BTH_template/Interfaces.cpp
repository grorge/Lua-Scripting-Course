#include "Interfaces.h"

#include <math.h>
#include <IImage.h>

Interface::Interface()
{
}

Interface::Interface(irr::video::IVideoDriver * driver, irr::scene::ISceneManager * smgr, irr::gui::IGUIEnvironment * guienv, lua_State* L_state)
{
	this->L = L_state;

	this->driver = driver;
	this->smgr = smgr;
	this->guienv = guienv;

	this->IDs = 100;


	//Default modle for Labb 1
	irr::scene::IAnimatedMesh* mesh = smgr->getMesh("../Meshes/sydney.md2");
	irr::scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);
	if (node)
	{
		node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		node->setMD2Animation(irr::scene::EMAT_STAND);
		node->setMaterialTexture(0, driver->getTexture("../Meshes/sydney.bmp"));
	}
	node->setID(this->IDs);

	this->nodes.push_back(node);

	this->IDs++;
}

Interface::~Interface()
{
}

int Interface::addMesh(irr::core::array<irr::core::vector3df> posVectors)
{
	Object* testObj;
	
	int nrOfTriangles = 0;

	irr::core::array<irr::core::vector3df> posVectorsPart;

	int totalTriangles = std::floor(posVectors.size() / 3);

	for (int i = 0; i < totalTriangles; i++)
	{
		posVectorsPart.push_back(posVectors[0 + nrOfTriangles]);
		posVectorsPart.push_back(posVectors[1 + nrOfTriangles]);
		posVectorsPart.push_back(posVectors[2 + nrOfTriangles]);

		testObj = new Object(this->smgr->getRootSceneNode(), this->smgr, this->IDs, posVectorsPart);

		this->nodes.push_back(testObj);

		this->IDs++;

		posVectorsPart.clear();
		nrOfTriangles++;
	}

	
	
	return 1;
}

int Interface::updatepos(irr::core::vector3df pos)
{
	this->nodes[0]->setPosition(pos);

	return int();
}

int Interface::getpos()
{

	lua_newtable(this->L);

	// Pushes the number in the second arg to the top of hte stack
	lua_pushnumber(this->L, this->nodes[0]->getPosition().X);
	/* (lua_state - where is the stack the table is - where in the table the data is stored )*/
	// The data is the data that is ontop of the stack
	lua_rawseti(this->L, -2, 1); 
	lua_pushnumber(this->L, this->nodes[0]->getPosition().Y);
	lua_rawseti(this->L, -2, 2);
	lua_pushnumber(this->L, this->nodes[0]->getPosition().Z);
	lua_rawseti(this->L, -2, 3);


	// Retunerar antalet värden som retuneras till Lua
	// Den säger till lua att den har lagt 1 värde på stacken
	return 1;
}

int Interface::addBox(irr::core::vector3d<irr::s32> pos, int size)
{
	std::string generatedName = "name" + std::to_string( this->IDs);
	
	return addBox(pos, size, generatedName);
}

int Interface::addBox(irr::core::vector3d<irr::s32> pos, int size, std::string name)
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



	return int();
}

int Interface::getNodes()
{
	std::string returnTable = "Boxes: \n";

	lua_newtable(this->L);

	for (int i = 0; i < this->nodes.size(); i++)
	{
		std::string name = "NaN mesh";
		if (dynamic_cast<Box*>(this->nodes[i]))
		{
			name = dynamic_cast<Box*>(this->nodes[i])->getName();
		}

		returnTable = std::to_string( this->nodes[i]->getID()) + "	" + name 
			+ "\n";
		
		//const char* toTable = returnTable.c_str();

		// Pushes the number in the second arg to the top of hte stack
		lua_pushstring(this->L, returnTable.c_str());
		/* (lua_state - where is the stack the table is - where in the table the data is stored )*/
		// The data is the data that 
		lua_rawseti(this->L, -2, i+1);
	}


	

	return 1;
}

int Interface::camera(irr::core::vector3df pos, irr::core::vector3df target)
{
	smgr->addCameraSceneNode(0, pos, target);
	
	return int();
}

int Interface::snapshot(std::string filename)
{
	irr::video::IImage* screenshot = this->driver->createScreenShot();
	
	this->driver->writeImageToFile(screenshot, filename.c_str());

	return 1;
}
