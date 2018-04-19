#pragma comment(lib, "Irrlicht.lib")
#ifdef _DEBUG
#pragma comment(lib, "LuaLibd.lib")
#else
#pragma comment(lib, "Lualib.lib")
#endif

#include <Windows.h>
#include <iostream>
#include <thread>
#include <irrlicht.h>

#include "Interfaces.h"

// Global interface to call functions from the Lua terminal
Interface intf;

void ConsoleThread(lua_State* L) {
	char command[1000];
	while(GetConsoleWindow()) {
		memset(command, 0, 1000);
		std::cin.getline(command, 1000);
		
		if (luaL_loadstring(L, command) || lua_pcall(L, 0, 0, 0))
		{
			// H�mtar en str�ng fr�n stacken med index -1 som �r �verst i stacken
			// ett felmedelande �r alltid en str�ng
			std::cout << lua_tostring(L, -1) << '\n';

			// Felmedelandet ligger kvar p� stacken det m�ste tas bort f�r att inte f� minnesl�ckor
			// det kan ocks� ge fel n�r man jobbar med stacken ifall den inte �r tom
			lua_pop(L, 1);
		}

	}
}


static int l_addBox(lua_State *L) {
	
	int returnValue = 1;
	
	// createes the vector to hold the position
	irr::core::vector3df vector;
	
	// check if its a table
	if (lua_istable(L, 1) && lua_isnumber(L, 2))
	{
		int check = 0;
		// pushes the first element in the table to the stack
		lua_rawgeti(L, 1, 1);
		// checks if its a number
		check += lua_isnumber(L, -1);
		// adds the cumber to the x0 var
		vector.X = luaL_checknumber(L, -1);
		// pops the element
		lua_pop(L, 1);

		// pushes the second element in the table to the stack
		lua_rawgeti(L, 1, 2);
		// checks if its a number
		check += lua_isnumber(L, -1);
		// adds the cumber to the x0 var
		vector.Y = luaL_checknumber(L, -1);
		// pops the element
		lua_pop(L, 1);

		// pushes the third element in the table to the stack
		lua_rawgeti(L, 1, 3);
		// checks if its a number
		check += lua_isnumber(L, -1);
		// adds the cumber to the x0 var
		vector.Z = luaL_checknumber(L, -1);
		// pops the element
		lua_pop(L, 1);

		if (check == 3)
		{
			float s = luaL_checknumber(L, 2);

			//check for a name
			if (lua_isstring(L, 3))
			{
				std::string boxName = luaL_checkstring(L, 3);

				returnValue = intf.addBox({ vector.X, vector.Y, vector.Z }, s, boxName);
			}
			else // no name case
			{
				returnValue = intf.addBox({ vector.X, vector.Y, vector.Z }, s);
			}
		}
		else
		{
			// Not number in vector
			// This is done by lua_checknumber()
		}
	}
	else
	{
		// wrong input
		lua_pushstring(L, "wrong input");
	}

	
	return returnValue;  /* number of results */
}

// Interpets the Lua into a vector for Irrlicht
static int l_addMesh(lua_State *L) {

	int nrOfTables = 0;

	// sees the number of elements in stack
	nrOfTables = lua_gettop(L);


	// It is one table on the stack
	if (nrOfTables == 1 && lua_istable(L, 1))
	{
		// Array with all the posints to draw
		irr::core::array<irr::core::vector3df> posVectors;

		// To hold the number of tablesadded
		int nrOfPositions = 1;

		// pushes first table in hte input table
		lua_rawgeti(L, 1, nrOfPositions);
		while (lua_istable(L, -1) == 1)
		{
			// createes the vector to hold the position
			irr::core::vector3df vector;

			nrOfTables = lua_gettop(L);
			// pushes the first element in the table to the stack
			lua_rawgeti(L, -1, 1);
			// checks if its a number
			lua_isnumber(L, -1);
			// adds the cumber to the x0 var
			vector.X = luaL_checknumber(L, -1);
			// pops the element
			lua_pop(L, 1);

			// pushes the second element in the table to the stack
			lua_rawgeti(L, -1, 2);
			// checks if its a number
			lua_isnumber(L, -1);
			// adds the cumber to the x0 var
			vector.Y = luaL_checknumber(L, -1);
			// pops the element
			lua_pop(L, 1);

			// pushes the third element in the table to the stack
			lua_rawgeti(L, -1, 3);
			// checks if its a number
			lua_isnumber(L, -1);
			// adds the cumber to the x0 var
			vector.Z = luaL_checknumber(L, -1);
			// pops the element
			lua_pop(L, 1);

			posVectors.push_back(vector);

			// Pops the table in the first place from the stack
			lua_pop(L, 1);

			nrOfPositions++;
			// pushes the table on the first place in the table
			lua_rawgeti(L, 1, nrOfPositions);
		}

		if (posVectors.size() % 3 > 0)
		{
			// post a mesage saying it will only draw in pairs of 3
			// Not a crash
		}


		intf.addMesh(posVectors);
	}
	else
	{
		// wrong in input
			// first input is not a table
			// not 1 argument on stack
	}
	
	return 1;  /* number of results */
}

static int l_listNodes(lua_State *L) {
	//intf.getNodes();
	return intf.getNodes();;  /* number of results */
}

static int l_camera(lua_State *L) {
	// createes the vector to hold the position
	irr::core::vector3df vectorPos;
	// createes the vector to hold the position
	irr::core::vector3df vectorDest;

	// It is one table on the stack
	if (lua_istable(L, 1))
	{

		// pushes the first element in the table to the stack
		lua_rawgeti(L, 1, 1);
		// checks if its a number
		lua_isnumber(L, -1);
		// adds the cumber to the x0 var
		vectorPos.X = luaL_checknumber(L, -1);
		// pops the element
		lua_pop(L, 1);

		// pushes the second element in the table to the stack
		lua_rawgeti(L, 1, 2);
		// checks if its a number
		lua_isnumber(L, -1);
		// adds the cumber to the x0 var
		vectorPos.Y = luaL_checknumber(L, -1);
		// pops the element
		lua_pop(L, 1);

		// pushes the third element in the table to the stack
		lua_rawgeti(L, 1, 3);
		// checks if its a number
		lua_isnumber(L, -1);
		// adds the cumber to the x0 var
		vectorPos.Z = luaL_checknumber(L, -1);
		// pops the element
		lua_pop(L, 1);
	}
	// It is one table on the stack
	if (lua_istable(L, 2))
	{

		// pushes the first element in the table to the stack
		lua_rawgeti(L, 2, 1);
		// checks if its a number
		lua_isnumber(L, -1);
		// adds the cumber to the x0 var
		vectorDest.X = luaL_checknumber(L, -1);
		// pops the element
		lua_pop(L, 1);

		// pushes the second element in the table to the stack
		lua_rawgeti(L, 2, 2);
		// checks if its a number
		lua_isnumber(L, -1);
		// adds the cumber to the x0 var
		vectorDest.Y = luaL_checknumber(L, -1);
		// pops the element
		lua_pop(L, 1);

		// pushes the third element in the table to the stack
		lua_rawgeti(L, 2, 3);
		// checks if its a number
		lua_isnumber(L, -1);
		// adds the cumber to the x0 var
		vectorDest.Z = luaL_checknumber(L, -1);
		// pops the element
		lua_pop(L, 1);
	}


	intf.camera(vectorPos, vectorDest);
	return 1;  /* number of results */
}

static int l_updatepos(lua_State *L) {
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
	float z = luaL_checknumber(L, 3);

	intf.updatepos({ x, y, z });
	return 0;  /* number of results */
}

static int l_getpos(lua_State *L) {

	//intf.getpos();
	return intf.getpos();  /* number of results */
}

static int l_snapshot(lua_State *L) {


	std::string fileName = luaL_checkstring(L, -1);

	std::string filePath = "C:/Users/maxjo/Source/Repos/Lua-Scripting-Course/Lua_Irrlicht_BTH_template/Lua_Irrlicht_BTH_template/" + fileName;

	return intf.snapshot(filePath); 
}

static int l_loadScene(lua_State *L) {

	std::string fileName = luaL_checkstring(L, -1);

	std::string filePath = "C:/Users/maxjo/Source/Repos/Lua-Scripting-Course/Lua_Irrlicht_BTH_template/Lua_Irrlicht_BTH_template/" + fileName;

	int error = luaL_loadfile(L, filePath.c_str());
	if (error) {
		/* If something went wrong, error message is at the top of */
		/* the stack */
		std::cout << lua_tostring(L, -1) << std::endl;
	}
	error = lua_pcall(L, 0, 0, 0);
	if (error) {
		/* If something went wrong, error message is at the top of */
		/* the stack */
		std::cout << lua_tostring(L, -1) << std::endl;
	}

	return intf.getpos();  /* number of results */
}


int main()
{
	lua_State* L = luaL_newstate();

	luaL_openlibs(L);

		std::thread conThread(ConsoleThread, L);

	irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, true, 0);
	if(!device)
		return 1;

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");
	irr::video::IVideoDriver* driver	= device->getVideoDriver();
	irr::scene::ISceneManager* smgr		= device->getSceneManager();
	irr::gui::IGUIEnvironment* guienv	= device->getGUIEnvironment();


	intf = Interface(driver, smgr, guienv, L);

	// BINDING CFUNCTIONS TO LUA CALLS
	lua_pushcfunction(L, l_addBox);
	lua_setglobal(L, "addBox");
	lua_pushcfunction(L, l_addMesh);
	lua_setglobal(L, "addMesh");
	lua_pushcfunction(L, l_listNodes);
	lua_setglobal(L, "getNodes");
	lua_pushcfunction(L, l_camera);
	lua_setglobal(L, "camera");

	lua_pushcfunction(L, l_updatepos);
	lua_setglobal(L, "updatepos");
	lua_pushcfunction(L, l_getpos);
	lua_setglobal(L, "getpos");
	lua_pushcfunction(L, l_loadScene);
	lua_setglobal(L, "loadScene");
	lua_pushcfunction(L, l_snapshot);
	lua_setglobal(L, "snapshot");



	// HARDCODED C WORLD
	//intf.camera(irr::core::vector3df(30, -20, -60), irr::core::vector3df(0, 0, 0));
	
	//bad vertex system
	//Vertex vertexs[3] =
	//{ 
	//	Vertex(20, -20, 10) ,
	//	Vertex(-20, -20, 10) , 
	//	Vertex(0, 20, 10)
	//};

	irr::core::vector3df pos1 = { 20, -20, 10 };
	irr::core::vector3df pos2 = { -20, -20, 10 };

	
	intf.addBox(pos1, 5);
	intf.addBox(pos2, 5);
	//intf.addMesh(vertexs);

	for (int i = 0; i < 10; i++)
	{
		//intf.addBox({(float)i, (float)i, (float)i}, i);
	}



	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);

	// Run the program once so the testfile can work preperly
	driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));


	smgr->drawAll();
	guienv->drawAll();

	driver->endScene();


	// Loads the lua testfile
	int error = luaL_loadfile(L, "C:/Users/maxjo/Source/Repos/Lua-Scripting-Course/Lua_Irrlicht_BTH_template/Lua_Irrlicht_BTH_template/testfile.lua");
	if (error) {
		/* If something went wrong, error message is at the top of */
		/* the stack */
		std::cout << lua_tostring(L, -1) << std::endl;
	}
	error = lua_pcall(L, 0, 0, 0);
	if (error) {
		/* If something went wrong, error message is at the top of */
		/* the stack */
		std::cout << lua_tostring(L, -1) << std::endl;
	}

	while(device->run()) {
		driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));


		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();		
	}

	device->drop();

	conThread.join();
	return 0;
}

