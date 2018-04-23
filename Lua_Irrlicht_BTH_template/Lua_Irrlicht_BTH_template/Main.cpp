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
#include <list>

#include "Interfaces.h"

class Tree
{
public:
	std::string       lexeme, tag;
	std::list<Tree*> children;
	Tree(std::string t, char *l, int size)
		: tag(t), lexeme(l, size) {}
	void dump(int depth = 0)
	{
		for (int i = 0; i<depth; i++)
			std::cout << "  ";
		// Recurse over the children
	}
};

class Regex
{
public:
	virtual int match(char const *)
		= 0;
};
class CharClass : public Regex
{
public:
	std::string contents;
	virtual int match(char const *text);
	CharClass(std::string c) :
		contents(c) {}
};
int CharClass::match(char const *text)
{
	return contents.find(*text) == std::string::npos ? -1 : 1;
}

class Star : public Regex
{
public:
	std::string contents;
	Regex* operand;
	virtual int match(char const *text);
	Star(std::string c) :
		contents(c) {}
};
int Star::match(char const *text)
{
	int n, consumed = 0;
	while ((n = operand->match(text)) > 0)
	{
		consumed += n;
		text += n;
	}
	return consumed;
}

class Seq : public Regex
{
public:
	std::string contents;
	std::list<Regex*> cells;
	virtual int match(char const *text);
	Seq(std::string c) :
		contents(c) {}
};
int Seq::match(char const *text)
{
	int chars, consumed = 0;
	for (auto c : cells) {
		if ((chars = c->match(text)) < 0) return -1;
		consumed += chars;
		text += chars;
	}
	return consumed;
}

// Global interface to call functions from the Lua terminal
Interface intf;

void ConsoleThread(lua_State* L) {
	char command[1000];
	while(GetConsoleWindow()) {
		memset(command, 0, 1000);
		std::cin.getline(command, 1000);
		if (luaL_loadstring(L, command) || lua_pcall(L, 0, 0, 0))
		{
			std::cout << lua_tostring(L, -1) << '\n';

			lua_pop(L, 1);
		}

	}
}


static int l_addBox(lua_State *L) {
	int x = luaL_checknumber(L, 1);
	int y = luaL_checknumber(L, 2);
	int z = luaL_checknumber(L, 3);
	intf.addBox({ x, y, z }, 5);
	return 1;  /* number of results */
}

static int l_listNodes(lua_State *L) {
	std::cout << intf.getNodes();
	return 1;  /* number of results */
}

static int l_camera(lua_State *L) {
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
	float z = luaL_checknumber(L, 3);

	//int tx = luaL_checknumber(L, 4);
	//int ty = luaL_checknumber(L, 5);
	//int tz = luaL_checknumber(L, 6);
	intf.camera({ x, y, z }, { 0, 0, 0 });
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

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);


	// Error blir 7 och sen 2 ????????????????????????????????????
	int error = luaL_loadfile(L, "../testfile.lua");
	error = lua_pcall(L, 0, 0, 0);


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
	/*
	pushar en pekare til funktionen till stacken
	sen sätter addBox som en global i Lua som binder den till det som ligger överst i stacken
	*/
	lua_pushcfunction(L, l_addBox);
	lua_setglobal(L, "addBox");
	lua_pushcfunction(L, l_listNodes);
	lua_setglobal(L, "listNodes");
	lua_pushcfunction(L, l_camera);
	lua_setglobal(L, "camera");

	lua_pushcfunction(L, l_updatepos);
	lua_setglobal(L, "updatepos");
	lua_pushcfunction(L, l_getpos);
	lua_setglobal(L, "getpos");



	// HARDCODED C WORLD
	intf.camera(irr::core::vector3df(30, -20, -60), irr::core::vector3df(0, 0, 0));
	
	//bad vertex system
	Vertex vertexs[3] =
	{ 
		Vertex(20, -20, 10) ,
		Vertex(-20, -20, 10) , 
		Vertex(0, 20, 10)
	};

	irr::core::vector3d<irr::s32> pos1 = { 20, -20, 10 };
	irr::core::vector3d<irr::s32> pos2 = { -20, -20, 10 };

	
	intf.addBox(pos1, 5);
	intf.addBox(pos2, 5);
	intf.addMesh(vertexs);




	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);

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

