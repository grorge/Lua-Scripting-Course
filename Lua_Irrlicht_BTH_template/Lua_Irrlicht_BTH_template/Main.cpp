#pragma comment(lib, "Irrlicht.lib")
#ifdef _DEBUG
#pragma comment(lib, "LuaLibd.lib")
#else
#pragma comment(lib, "Lualib.lib")
#endif

#include <lua.hpp>
#include <Windows.h>
#include <iostream>
#include <thread>
#include "lua.hpp"
#include <irrlicht.h>

#include "Interfaces.h"

/*
SUPERVIKTIGA PIZZA-KOMMENTAREN
Hawwai		2
Red Devil	2
La Mafia	1
Lambada		1
*/

std::string luaReturn;

void ConsoleThread(lua_State* L) {
	char command[1000];
	while(GetConsoleWindow()) {
		memset(command, 0, 1000);
		std::cin.getline(command, 1000);
		if (luaL_loadstring(L, command) || lua_pcall(L, 0, 0, 0))
		{
			//luaReturn = lua_tostring(L, -1);
			std::cout << lua_tostring(L, -1) << '\n';

		}

	}
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


	Interface intf(driver, smgr, guienv);

	Vertex vertexs[3] = 
	{ 
		Vertex(15, -10, 10) ,
		Vertex(-15, -10, 10) , 
		Vertex(0, 10, 10)
	};

	irr::core::vector3d<irr::s32> pos1 = { 1, 1, 1 };
	irr::core::vector3d<irr::s32> pos2 = { -1, -6, -1};

	intf.camera(irr::core::vector3df(30, -20, -60), irr::core::vector3df(0, 0, 0));

	intf.addBox(pos1, 5);
	intf.addBox({ -1, -15, -1 }, 5);
	intf.addMesh(vertexs);

	std::cout << intf.getNodes();

	//Object* testObj = new Object(smgr->getRootSceneNode(), smgr, 1337, vertexs, ARRAYSIZE(vertexs));

	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);

	while(device->run()) {
		driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));

		if (luaReturn == "addMesh")
		{

			luaReturn = "";
		}

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();		
	}

	device->drop();

	conThread.join();
	return 0;
}