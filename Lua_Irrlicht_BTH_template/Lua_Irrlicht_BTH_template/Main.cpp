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
			// Hämtar en sträng från stacken med index -1 som är överst i stacken
			// ett felmedelande är alltid en sträng
			std::cout << lua_tostring(L, -1) << '\n';

			// Felmedelandet ligger kvar på stacken det måste tas bort för att inte få minnesläckor
			// det kan också ge fel när man jobbar med stacken ifall den inte är tom
			lua_pop(L, 1);
		}

	}
}


static int l_addBox(lua_State *L) {
	int x = luaL_checknumber(L, 1);
	int y = luaL_checknumber(L, 2);
	int z = luaL_checknumber(L, 3);

	int s = luaL_checknumber(L, 4);

	std::string boxName = luaL_checkstring(L, 5);

	intf.addBox({ x, y, z }, s, boxName);
	return 1;  /* number of results */
}

static int l_addMesh(lua_State *L) {

	int nrOfTables = 0;
	int hej = 0;
	int isthis4 = 0;

	int x0 = 0;
	int y0 = 0;
	int z0 = 0;

	int x1 = 0;
	int y1 = 0;
	int z1 = 0;

	int x2 = 0;
	int y2 = 0;
	int z2 = 0;

	// ses the number of elements in stack
	nrOfTables = lua_gettop(L);

	// It is one table on the stack
	if (nrOfTables == 1 && lua_istable(L, 1))
	{
		// pushes the table on the first place in the table
		isthis4 = lua_rawgeti(L, 1, 1);
		if (lua_istable(L, -1) == 1)
		{
			nrOfTables = lua_gettop(L);
			// pushes the first element in the table to the stack
			isthis4 = lua_rawgeti(L, -1, 1);
			// checks if its a number
			hej = lua_isnumber(L, -1);
			// adds the cumber to the x0 var
			x0 = luaL_checknumber(L, -1);
			// pops the element
			lua_pop(L, 1);
			nrOfTables = lua_gettop(L);
		}
		// Pops the table in the first place from the stack
		lua_pop(L, 1);

		// pushes the table on the second place in the table
		isthis4 = lua_rawgeti(L, 1, 2);
		if (lua_istable(L, -1) == 1)
		{
			isthis4 = lua_rawgeti(L, -1, 2);
			nrOfTables = lua_gettop(L);
			hej = lua_isnumber(L, -1);
			lua_pop(L, 2);
		}

		// pushes the table on the third place in the table
		isthis4 = lua_rawgeti(L, 1, 3);
		if (lua_istable(L, -1) == 1)
		{
			isthis4 = lua_rawgeti(L, -1, 2);
			nrOfTables = lua_gettop(L);
			hej = lua_isnumber(L, -1);
			lua_pop(L, 2);
		}
	}
	nrOfTables = lua_gettop(L);
	
	lua_pushnumber(L, 2);
	isthis4 = lua_gettable(L, -2);




	intf.addMesh({ x0, y0, z0 }, { x1, y1, z1 }, { x2, y2, z2 });
	return 1;  /* number of results */
}

static int l_listNodes(lua_State *L) {
	//intf.getNodes();
	return intf.getNodes();;  /* number of results */
}

static int l_camera(lua_State *L) {
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
	float z = luaL_checknumber(L, 3);

	float tx = luaL_checknumber(L, 4);
	float ty = luaL_checknumber(L, 5);
	float tz = luaL_checknumber(L, 6);
	intf.camera({ x, y, z }, { tx, ty, tz });
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
	intf.camera(irr::core::vector3df(30, -20, -60), irr::core::vector3df(0, 0, 0));
	
	//bad vertex system
	//Vertex vertexs[3] =
	//{ 
	//	Vertex(20, -20, 10) ,
	//	Vertex(-20, -20, 10) , 
	//	Vertex(0, 20, 10)
	//};

	irr::core::vector3d<irr::s32> pos1 = { 20, -20, 10 };
	irr::core::vector3d<irr::s32> pos2 = { -20, -20, 10 };

	
	intf.addBox(pos1, 5);
	intf.addBox(pos2, 5);
	//intf.addMesh(vertexs);




	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);

	// Loads the lua testfile
	int error = luaL_loadfile(L, "C:/Users/maxjo/Source/Repos/Lua-Scripting-Course/Lua_Irrlicht_BTH_template/Lua_Irrlicht_BTH_template/testfile.lua");
	if (error) {
		/* If something went wrong, error message is at the top of */
		/* the stack */
		std::cout << lua_tostring(L, -1) << std::endl;
	}
	error = lua_pcall(L, 0, 0, 0);

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

