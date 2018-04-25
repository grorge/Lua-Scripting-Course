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
#include "Regex.h"

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

void testRegex(Regex* reg, const char* text)
{
	std::string str = text;
	std::cout << text << ": " << reg->match(text) << "/" << str.length() << std::endl;
}

bool checkString(const char* text);

bool isTable(const char* text);

bool isValue(const char* text);

bool isBracet(const char* text);

bool isString(const char* text);

bool isVarible(const char* text);

bool isDigit(const char* text);



int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	CharClass digit("0123456789");
	CharClass nonzero("123456789");
	CharClass hex("0123456789abcdefABCDEF");
	//[a-zA-Z]
	CharClass chars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	CharClass alpha("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	CharClass dot(".");
	CharClass comma(",");
	CharClass semi(";");
	CharClass commasemi(",;");
	CharClass startT("{");
	CharClass endT("}");
	CharClass squBstr("[");
	CharClass squBend("]");

	//([0-9]*[a-zA-Z]*)*
	Star alphaKeyStar(
		&alpha
	);
	Seq stringAlpha({
		new CharClass("\""),
		&alphaKeyStar,
		new CharClass("\"")
	});

	// (0x)[0-9a-fA-F]*
	Seq	hexLiteral({
		new Seq({
			new CharClass("0"),
			new CharClass("x")
		}),
		new Star( &hex)
	});

	// ["["]([0-9]*[a-zA-Z]*)*["]"]
	Seq bracetKey({
		&squBstr,
		&alphaKeyStar,
		&squBend
	});
	Seq bracetString({
		&squBstr,
		&stringAlpha,
		&squBend
	});

	// ([1-9][0-9]*)[.][0-9]*
	Seq number({ 
		new Seq({&nonzero, new Star(&digit) }), 
		&dot, 
		new Star(&digit)
	});

	Seq table({});

	// (alphakey)*(table)*(bracetKey)*
	Seq value({
		&alphaKeyStar,
		new Star(&table),
		new Star(&bracetKey),
		new Star(&bracetString),
		new Star(&stringAlpha)
	});

	Seq valueNoneDigit({
		new Star(&chars),
		new Star(&table),
		new Star(&bracetKey),
		new Star(&bracetString),
		new Star(&stringAlpha)
		});

	Seq declare({
		&valueNoneDigit,
		new CharClass("="),
		&value
	});


	

	// value=value
	Seq field({
		new Star(&value),
		new Star(&declare)
	});

	table = Seq({
		&startT,
		new Star(&field),
		&endT
	});

	std::cout << "RegEx TEST:\n\n";
		
		std::cout << "--AplhaKey:\n";
		testRegex(&alphaKeyStar, "3124.235");
		testRegex(&alphaKeyStar, "fjaygfbsaf632uar2174g4");

		
		std::cout << "--Bracket:\n";
		testRegex(&bracetKey, "[asgdsdakgsdagsadgsadghelia213af1]");


		std::cout << "--Hex:\n";
		testRegex(&hexLiteral, "0x1");
		testRegex(&hexLiteral, "0xfed");
		testRegex(&hexLiteral, "0xCBA9");
		testRegex(&hexLiteral, "0x00000000");
		
		testRegex(&hexLiteral, "12ef");
		testRegex(&hexLiteral, "0y7");		
		testRegex(&hexLiteral, "0xG900");		


		std::cout << "--Tables:\n";
		testRegex(&table, "{}");
		testRegex(&table, "{1,2;3}");
		testRegex(&table, "{1,2;3,}");
		testRegex(&table, "{easyas=\"abc\";2;2,[\"hello\"]=\"world\",[3]=4}");
		testRegex(&table, "{{1,2,3},data={0x77}}");

		testRegex(&table, "{{}");
		testRegex(&table, "{;}");
		testRegex(&table, "{1,,}");
		testRegex(&table, "{34=7}");
		testRegex(&table, "{alpha=beta=gamma}");

		isTable("{{1,2,3},data={0x77}}");


	std::cout << "\nEND OF TEST\n" << std::endl;

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



bool checkString(const char* text)
{

	Regex* table1;
	Regex* table2;
	Regex* value;
	Regex* declare;
	Regex* semicomma;
	Regex* none;
	Regex* bracet;
	Regex* table3;
	Regex* character;
	Regex* string;
	Regex* digit;

	std::string str(text);

	// Is it a table?
	if (isTable(text))
	{
		// Remove wings
		str.pop_back();
		str.erase(0, 1);;





	}




	// Is it a value?


	//std::list<Regex*> tempList;
	//int traversed = 0;

	//// Put together table2 return -1 if it fails
	//bool succses = false;
	//// case value
	//if ((traversed = value->match(str)) > 0)
	//{
	//	tempList.push_back(value);
	//	succses = true;
	//} // case declration
	//else if ((traversed = declare->match(str)) > 0)
	//{
	//	tempList.push_back(declare);
	//	succses = true;
	//	
	//} // case table1
	//else if ((traversed = table1->match(str)) > 0)
	//{
	//	tempList.push_back(table1);
	//	succses = true;
	//}

	//if (succses)
	//{
	//	if (semicomma->match(str) > 0)
	//	{
	//		tempList.push_back(semicomma);
	//	}
	//	else if (none->match(str) > 0)
	//	{
	//		tempList.push_back(none);
	//	}
	//	else
	//	{
	//		return -1;
	//	}
	//}
	//else
	//{
	//	return -1;
	//}

	//// Table är nu en som kan göra match för att validera 
	//table2 = new Seq(tempList);

	return true;
}

bool isTable(const char* text)
{
	std::string str(text);
	bool retValue = false;

	if (str.find_first_of("{") == 0 && str.find_last_of("}") == str.length() - 1)
	{
		retValue = true/*checkString(str.c_str())*/;
	}

	return retValue;
}

bool isValue(const char* text)
{
	std::string str(text);
	bool retValue = false;

	if (isBracet(text) || isString(text) || isDigit(text) || isVarible(text))
	{
		retValue = true;
	}

	return retValue;
}

bool isBracet(const char* text)
{
	std::string str(text);
	bool retValue = false;

	if (str.find_first_of("[") == 0 && str.find_last_of("]") == str.length() - 1)
	{
		// Remove wings
		str.pop_back();
		str.erase(0, 1);;

		if (isString(str.c_str()) || isDigit(str.c_str()))
		{
			retValue = true/*checkString(str.c_str())*/;
		}
	}


	return retValue;
}

bool isString(const char* text)
{
	std::string str(text);
	bool retValue = false;


	if (str.find_first_of("\"") == 0 && str.find_last_of("\"") == str.length() - 1)
	{
		// Remove wings
		str.pop_back();
		str.erase(0, 1);;


		if (str.find("\"") == str.npos)
		{
			retValue = true/*checkString(str.c_str())*/;
		}

		retValue = true/*checkString(str.c_str())*/;
	}

	return retValue;
}

bool isVarible(const char* text)
{
	std::string str(text);
	bool retValue = false;
	
	CharClass alpha("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

	Star key(&alpha);

	if (key.match(text) == str.length())
	{
		retValue = true;
	}

	return retValue;
}

bool isDigit(const char* text)
{
	std::string str(text);
	bool retValue = false;

	CharClass digit("0123456789");

	Star key(&digit);

	if (key.match(text) == str.length())
	{
		retValue = true;
	}

	return retValue;
}