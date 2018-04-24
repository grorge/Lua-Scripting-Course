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
	Star(Regex* c) :
		operand(c) {}
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
	// std::string contents;
	std::list<Regex*> cells;
	virtual int match(char const *text);
	Seq(std::list<Regex*> c) :
		cells(c) {}
};
int Seq::match(char const *text)
{
	int chars, consumed = 0;
	for (auto c : cells) {
		if ((chars = c->match(text)) < 0)
		{
			return -1;
		}
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


int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	CharClass digit("0123456789");
	CharClass nonzero("123456789");
	CharClass hex("0123456789abcdefABCDEF");
	CharClass string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	CharClass dot(".");
	CharClass comma(",");
	CharClass semi(";");
	CharClass startT("{");
	CharClass endT("}");

	// ([1-9][0-9]*)[.][0-9]*
	Seq number({ 
		new Seq({&nonzero, new Star(&digit) }), 
		&dot, 
		new Star(&digit)
	});

	std::cout << "RegEx TEST:\n" << 
		
		digit.match("3")	<< std::endl <<
		number.match("3124.235")	<< std::endl <<
		number.match(".235") << std::endl <<
		number.match("3124235") << std::endl <<
		number.match("3124.") << std::endl <<
		
		"END OF TEST" << std::endl;

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



	// HARDCODED C WORLD
	intf.camera(irr::core::vector3df(30, -20, -60), irr::core::vector3df(0, 0, 0));
	





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

