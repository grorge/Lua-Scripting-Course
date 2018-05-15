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


class MyEventReceiver : public irr::IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const irr::SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EMIE_MOUSE_MOVED)
		{
			this->mousePosition.X = event.MouseInput.X;
			this->mousePosition.Y = event.MouseInput.Y;
		}

		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(irr::EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}

	MyEventReceiver()
	{
		for (irr::u32 i = 0; i<irr::KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

	irr::core::position2di getMousePosition()
	{
		return mousePosition;
	};

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[irr::KEY_KEY_CODES_COUNT];

	irr::core::position2di mousePosition;
};


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
		luaL_dostring(L, "Error: Wrong arguments')");
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
			if (lua_isnumber(L, -1))
			{
				// adds the cumber to the x0 var
				vector.X = luaL_checknumber(L, -1);

				// pops the element
				lua_pop(L, 1);

				// pushes the second element in the table to the stack
				lua_rawgeti(L, -1, 2);
				// checks if its a number
				if (lua_isnumber(L, -1))
				{
					// adds the cumber to the x0 var
					vector.Y = luaL_checknumber(L, -1);

					// pops the element
					lua_pop(L, 1);

					// pushes the third element in the table to the stack
					lua_rawgeti(L, -1, 3);
					// checks if its a number
					if (lua_isnumber(L, -1))
					{
						// adds the cumber to the x0 var
						vector.Z = luaL_checknumber(L, -1);


						// pops the element
						lua_pop(L, 1);

						posVectors.push_back(vector);

						if (lua_rawgeti(L, -1, 4))
						{
							luaL_dostring(L, "print('Non-Fatal Error: number of components - removes after third arg')");
						}

						// Pops the table in the first place from the stack
						lua_pop(L, 1);


						std::string temp = "print('next vert')";
						luaL_dostring(L, temp.c_str());

						nrOfPositions++;
						// pushes the table on the first place in the table
						lua_rawgeti(L, 1, nrOfPositions);
					}
					else
					{
						luaL_dostring(L, "print('Fatal Error: non-numeric coordinates')");
						// Pops the table in the first place from the stack
						lua_pop(L, 1);
						break;
					}
				}
				else
				{
					luaL_dostring(L, "print('Fatal Error: non-numeric coordinates')");
					// Pops the table in the first place from the stack
					lua_pop(L, 1);
					break;
				}
				
			}
			else
			{
				luaL_dostring(L, "print('Fatal Error: non-numeric coordinates')");
				// Pops the table in the first place from the stack
				lua_pop(L, 1);
				break;
			}				
			
		}

		if (posVectors.size() % 3 > 0)
		{
			// post a mesage saying it will only draw in pairs of 3
			// Not a crash
			luaL_dostring(L, "print('Non-Fatal Error: not a valid number of vertices')");
		}


		intf.addMesh(posVectors);
	}
	else
	{
		// wrong in input
			// first input is not a table
			// not 1 argument on stack
		luaL_dostring(L, "print('Fatal Error: Input should be 1 table containing meshes')");
	}
	

	std::string temp = "print('next Mesh')";
	luaL_dostring(L, temp.c_str());

	return 1;  /* number of results */
}

static int l_listNodes(lua_State *L) {
	//intf.getNodes();
	return intf.getNodes();  /* number of results */
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
		if(lua_isnumber(L, -1))		
			vectorPos.X = luaL_checknumber(L, -1);
		else if (lua_isnil(L, -1))
			luaL_dostring(L, "print('Fatal Error: target has wrong number of coordinates')");
		else
			luaL_dostring(L, "print('Fatal Error: cordinate is not a number')");
		// pops the element
		lua_pop(L, 1);

		// pushes the second element in the table to the stack
		lua_rawgeti(L, 1, 2);
		// checks if its a number
		if (lua_isnumber(L, -1))
			vectorPos.Y = luaL_checknumber(L, -1);
		else if (lua_isnil(L, -1))
			luaL_dostring(L, "print('Fatal Error: target has wrong number of coordinates')");
		else
			luaL_dostring(L, "print('Fatal Error: cordinate is not a number')");
		// pops the element
		lua_pop(L, 1);

		// pushes the third element in the table to the stack
		lua_rawgeti(L, 1, 3);
		// checks if its a number
		if (lua_isnumber(L, -1))
			vectorPos.Z = luaL_checknumber(L, -1);
		else if (lua_isnil(L, -1))
			luaL_dostring(L, "print('Fatal Error: target has wrong number of coordinates')");
		else
			luaL_dostring(L, "print('Fatal Error: cordinate is not a number')");
		// pops the element
		lua_pop(L, 1);
	}
	// It is one table on the stack
	if (lua_istable(L, 2))
	{

		// pushes the first element in the table to the stack
		lua_rawgeti(L, 2, 1);
		// checks if its a number
		if (lua_isnumber(L, -1))
			vectorDest.X = luaL_checknumber(L, -1);
		else if (lua_isnil(L, -1))
			luaL_dostring(L, "print('Fatal Error: target has wrong number of coordinates')");
		else
			luaL_dostring(L, "print('Fatal Error: cordinate is not a number')");
		// pops the element
		lua_pop(L, 1);

		// pushes the second element in the table to the stack
		lua_rawgeti(L, 2, 2);
		// checks if its a number
		if (lua_isnumber(L, -1))
			vectorDest.Y = luaL_checknumber(L, -1);
		else if (lua_isnil(L, -1))
			luaL_dostring(L, "print('Fatal Error: target has wrong number of coordinates')");
		else
			luaL_dostring(L, "print('Fatal Error: cordinate is not a number')");
		// pops the element
		lua_pop(L, 1);

		// pushes the third element in the table to the stack
		lua_rawgeti(L, 2, 3);
		// checks if its a number
		if (lua_isnumber(L, -1))
			vectorDest.Z = luaL_checknumber(L, -1);
		else if (lua_isnil(L, -1))
			luaL_dostring(L, "print('Fatal Error: target has wrong number of coordinates')");
		else
			luaL_dostring(L, "print('Fatal Error: cordinate is not a number')");
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


	MyEventReceiver eventRec;

	irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, true, &eventRec);
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

	intf.setCam(smgr->addCameraSceneNodeFPS());

	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);

	// Run the program once so the testfile can work preperly
	driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));


	smgr->drawAll();
	guienv->drawAll();

	driver->endScene();


	// Loads the lua testfile
	//int error = luaL_loadfile(L, "C:/Users/maxjo/Source/Repos/Lua-Scripting-Course/Lua_Irrlicht_BTH_template/Lua_Irrlicht_BTH_template/errorcheck.lua");
	//if (error) {
	//	/* If something went wrong, error message is at the top of */
	//	/* the stack */
	//	std::cout << lua_tostring(L, -1) << std::endl;
	//}
	//error = lua_pcall(L, 0, 0, 0);
	//if (error) {
	//	/* If something went wrong, error message is at the top of */
	//	/* the stack */
	//	std::cout << lua_tostring(L, -1) << std::endl;
	//}

	
	while(device->run()) {
		if (!device->isWindowFocused())
		{
			Sleep(1);
			continue;
		}

		driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));
		

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();		
	}

	device->drop();

	conThread.join();
	return 0;
}

