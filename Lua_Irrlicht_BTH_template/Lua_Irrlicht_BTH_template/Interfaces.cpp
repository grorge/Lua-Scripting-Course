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

	/*
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
	*/
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
		posVectorsPart.push_back(posVectors[0 + nrOfTriangles*3]);
		posVectorsPart.push_back(posVectors[1 + nrOfTriangles*3]);
		posVectorsPart.push_back(posVectors[2 + nrOfTriangles*3]);

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

int Interface::addBox(irr::core::vector3df pos, float size)
{
	std::string generatedName = "name" + std::to_string( this->IDs);
	
	return addBox(pos, size, generatedName);
}

int Interface::addBox(irr::core::vector3df pos, float size, std::string name)
{
	size *= 0.1;

	Vertex v[24] =
	{
		// Front
		Vertex(pos.X - 10.0f * size, pos.Y - 10.0f * size, pos.Z - 10.0f * size) ,
		Vertex(pos.X - 10.0f * size, pos.Y + 10.0f * size, pos.Z - 10.0f * size),
		Vertex(pos.X + 10.0f * size, pos.Y + 10.0f * size, pos.Z - 10.0f * size),
		Vertex(pos.X + 10.0f * size, pos.Y - 10.0f * size, pos.Z - 10.0f * size),

		// Back
		Vertex(pos.X - 10.0f * size, pos.Y - 10.0f * size, pos.Z + 10.0f * size),
		Vertex(pos.X + 10.0f * size, pos.Y - 10.0f * size, pos.Z + 10.0f * size),
		Vertex(pos.X + 10.0f * size, pos.Y + 10.0f * size, pos.Z + 10.0f * size),
		Vertex(pos.X - 10.0f * size, pos.Y + 10.0f * size, pos.Z + 10.0f * size),

		// Top
		Vertex(pos.X - 10.0f * size, pos.Y + 10.0f * size, pos.Z - 10.0f * size),
		Vertex(pos.X - 10.0f * size, pos.Y + 10.0f * size, pos.Z + 10.0f * size),
		Vertex(pos.X + 10.0f * size, pos.Y + 10.0f * size, pos.Z + 10.0f * size),
		Vertex(pos.X + 10.0f * size, pos.Y + 10.0f * size, pos.Z - 10.0f * size),

		// Bottom
		Vertex(pos.X - 10.0f * size, pos.Y - 10.0f * size, pos.Z - 10.0f * size),
		Vertex(pos.X + 10.0f * size, pos.Y - 10.0f * size, pos.Z - 10.0f * size),
		Vertex(pos.X + 10.0f * size, pos.Y - 10.0f * size, pos.Z + 10.0f * size),
		Vertex(pos.X - 10.0f * size, pos.Y - 10.0f * size, pos.Z + 10.0f * size),

		// Left
		Vertex(pos.X - 10.0f * size, pos.Y - 10.0f * size, pos.Z + 10.0f * size),
		Vertex(pos.X - 10.0f * size, pos.Y + 10.0f * size, pos.Z + 10.0f * size),
		Vertex(pos.X - 10.0f * size, pos.Y + 10.0f * size, pos.Z - 10.0f * size),
		Vertex(pos.X - 10.0f * size, pos.Y - 10.0f * size, pos.Z - 10.0f * size),

		// Right
		Vertex(pos.X + 10.0f * size, pos.Y - 10.0f * size, pos.Z - 10.0f * size),
		Vertex(pos.X + 10.0f * size, pos.Y + 10.0f * size, pos.Z - 10.0f * size),
		Vertex(pos.X + 10.0f * size, pos.Y + 10.0f * size, pos.Z + 10.0f * size),
		Vertex(pos.X + 10.0f * size, pos.Y - 10.0f * size, pos.Z + 10.0f * size),
	};
		
	Box* testObj;
	testObj = new Box(this->smgr->getRootSceneNode(), this->smgr, this->IDs, v, 24, name);
	
	this->nodes.push_back(testObj);

	this->IDs++;



	return int();
}

int Interface::getNodes()
{
	//Create a table to contain all tables with information
	lua_newtable(this->L);

	for (int i = 0; i < this->nodes.size(); i++)
	{
		//Create a table to add all the fields and elements to
		lua_newtable(this->L);
				
		std::string name = "NaN mesh";
		std::string type = "NaN OBJ";
		if (dynamic_cast<Box*>(this->nodes[i]))
		{
			name = dynamic_cast<Box*>(this->nodes[i])->getName();
			type = dynamic_cast<Box*>(this->nodes[i])->printType();
		}
		else if (dynamic_cast<Object*>(this->nodes[i]))
		{
			name = "NONAME MESH";
			type = dynamic_cast<Object*>(this->nodes[i])->printType();
		}

		//This method set the fields at random. cant control the order

		
		lua_pushstring(L, name.c_str());
		//Set the next field
		lua_setfield(this->L, -2, "name");

		
		lua_pushnumber(L, this->nodes[i]->getID());
		//Set the next field
		lua_setfield(this->L, -2, "ID");


		//Push string that will be added to the field
		lua_pushstring(L, type.c_str());
		//Set the next field
		lua_setfield(this->L, -2, "type");
		

		lua_rawseti(this->L, 1, i+1);
	}
	
	return 1;
}

int Interface::camera(irr::core::vector3df pos, irr::core::vector3df target)
{
	this->cam = smgr->addCameraSceneNodeFPS();
	this->cam->setPosition(pos);
	this->cam->setTarget(target);
	
	return 1;
}

int Interface::snapshot(std::string filename)
{
	driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));	
	smgr->drawAll();
	guienv->drawAll();
	driver->endScene();

	irr::video::IImage* screenshot = this->driver->createScreenShot();
	this->driver->writeImageToFile(screenshot, filename.c_str());

	FILE* file;
	fopen_s(&file, filename.c_str(), "r");
	if (!file)
		luaL_argcheck(this->L,0, 1, "Error: file could not be opened");
	else
		fclose(file);
	
	return 1;
}
