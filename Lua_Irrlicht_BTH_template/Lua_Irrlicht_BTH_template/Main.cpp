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

//bool checkString(const char* text);

bool isTable(const char* text);

bool isElem(const char* text);

bool isValue(const char* text);

bool isBracet(const char* text);

bool isString(const char* text);

bool isVarible(const char* text);

bool isDigit(const char* text);

bool isHex(const char* text);

bool isDeclare(const char* text);



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
		isTable("{}");
		isTable("{1,2;3}");
		isTable("{1,2;3,}");
		isTable("{easyas=\"abc\";2;2,[\"hello\"]=\"world\",[3]=4}");
		isTable("{{1,2,3},data={0x77}}");

		isTable("{{}");
		isTable("{;}");
		isTable("{1,,}");
		isTable("{34=7}");
		isTable("{alpha=beta=gamma}");

		isTable("{{1,2,3},data={0x77}}");
		isDeclare("hej=afsaf");

	std::cout << "\nEND OF TEST\n" << std::endl;


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

bool isTable(const char* text)
{
	std::string str(text);
	bool retValue = false;

	if (str.find_first_of("{") == 0 && str.find_last_of("}") == str.length() - 1)
	{
		// Remove wings
		str.pop_back();
		str.erase(0, 1);

		retValue = true;
		int lastComma = 0;
		int i = 0;
		int tabStrted = 0;
		while (i < str.length())
		{
			if (str[i] == '{')
			{
				tabStrted++;
			}
			else if (str[i] == '}')
			{
				tabStrted--;
			}
			else if (tabStrted == 0)
			{
				if ((str[i] == ',' || str[i] == ';')/* && i != lastComma + 1*/)
				{
					// Found a element
					std::string element(str);


					element.erase(i, str.length());
					if (lastComma > 0)
					{
						element.erase(0, lastComma+1);
					}
					


					if (!isElem(element.c_str()))
					{
						retValue = false;
					}
						
					lastComma = i;


				}
				else if (i + 1 == str.length())
				{
					i++;

					// Found a element
					std::string element(str);


					element.erase(i, str.length());
					if (lastComma > 0)
					{
						element.erase(0, lastComma + 1);
					}



					if (!isElem(element.c_str()))
					{
						retValue = false;
					}

				}
			}


			i++;
		}

		if (tabStrted != 0)
		{
			retValue = false;
		}

		//retValue = true;//checkString(str.c_str());
	}

	return retValue;
}

bool isElem(const char * text)
{
	std::string str(text);
	bool retValue = false;

	if (isValue(text) || isDeclare(text) || isHex(text) || isTable(text))
	{
		retValue = true;
	}

	if (str == "")
	{
		retValue = false;
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
	
	CharClass letter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	CharClass alpha("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

	Seq startOnLetter({
		&letter,
		new Star(&alpha)
		});

	if (startOnLetter.match(text) == str.length())
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

bool isHex(const char * text)
{
	std::string str(text);
	bool retValue = false;

	CharClass digit("0123456789");

	Seq	key({
		new Seq({
			new CharClass("0"),
			new CharClass("x")
			}),
		new Star(new CharClass("0123456789abcdefABCDEF"))
		});

	if (key.match(text) == str.length())
	{
		retValue = true;
	}

	return retValue;
}

bool isDeclare(const char * text)
{
	std::string str(text);
	bool retValue = false;

	int n = str.find_first_of("=");
	// only 1 =
	if (n == str.find_last_of("=") && n != -1)
	{
		std::string firstHalf(text);
		std::string secondHalf(text);

		firstHalf.erase(n,str.length());
		secondHalf.erase(0, n + 1);

		if (
			(isBracet(firstHalf.c_str()) || isString(firstHalf.c_str()) || /*isDigit(firstHalf.c_str()) || */isVarible(firstHalf.c_str()))
			&& 
			(isString(secondHalf.c_str()) || isDigit(secondHalf.c_str()) || isTable(secondHalf.c_str()) || isVarible(secondHalf.c_str()))
			)
		{
			retValue = true;
		}
	}

	

	return retValue;
}
