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
	Tree(std::string t, std::string l, int size)
		: tag(t), lexeme(l, size) {}
	void dump(int depth = 0)
	{
		for (int i = 0; i<depth; i++)
			std::cout << "|  ";

		std::cout << tag << ": " << lexeme;
		// Recurse over the children

		for (auto child : this->children)
		{
			std::cout << std::endl;
			child->dump(depth + 1);
		}
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

bool isTable(const char* text, Tree **result);

bool isElem(const char* text, Tree **result);

bool isValue(const char* text, Tree **result);

bool isBracet(const char* text, Tree **result);

bool isString(const char* text, Tree **result);

bool isVarible(const char* text, Tree **result);

bool isDigit(const char* text, Tree **result);

bool isHex(const char* text, Tree **result);

bool isDeclare(const char* text, Tree **result);



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

		Tree* rootTree = new Tree("\nROOT1", "", 0);
		Tree* rootTree2 = new Tree("\nROOT2", "", 0);

		isTable("{}", &rootTree);
		isTable("{1,2;3}", &rootTree);
		isTable("{1,2;3,}", &rootTree);
		isTable("{easyas=\"abc\";2;2,[\"hello\"]=\"world\",[3]=4}", &rootTree);
		
		isTable("{{1,2,3},data={0x77}}", &rootTree2);


		isTable("{{}", &rootTree);
		isTable("{;}", &rootTree);
		isTable("{1,,}", &rootTree);
		isTable("{34=7}", &rootTree);
		isTable("{alpha=beta=gamma}", &rootTree);

		//isTable("{{1,2,3},data={0x77}}", &rootTree);
		//isDeclare("hej=afsaf", &rootTree);

		rootTree->dump();
		rootTree2->dump();

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

bool getElements(const char* text, Tree **treeHead)
{
	std::string str(text);
	int tabStarted = 0;

	// ELEMENT SEMICOMMA TABLE2
	for (int i = 0; text[i] != '\0'; i++)
	{
		if (text[i] == '{')
		{
			tabStarted++;
		}
		else if (text[i] == '}')
		{
			tabStarted--;
		}

		// FINDS SEMICOMMA
		if (tabStarted == 0 && (text[i] == ',' || text[i] == ';'))
		{
			std::string element(str);

			element.erase(i, str.length());
			
			// CHECKS ELEMENT
			if (!isElem(element.c_str(), treeHead))
			{
				return false;
			}
			else
			{
				Tree *child1 = new Tree("SEMICOMMA", std::string(1, text[i]), 0);
				(*treeHead)->children.push_back(child1);

				str.erase(0, i + 1);
				
				Tree *child2 = new Tree("TABLE2", str, 0);
				(*treeHead)->children.push_back(child2);

				return getElements(str.c_str(), &child2);
			}
		}
	}
	if (tabStarted != 0)
	{
		return false;
	}

	// ELEMENT | empty
	if (isElem(str.c_str(), treeHead) || str == "")
	{
		return true;
	}

	return false;
}

bool isTable(const char* text, Tree **result)
{
	std::string str(text);
	bool retValue = false;
	Tree *child1 = new Tree("TABLE", str, 0);

	if (str.find_first_of("{") == 0 && str.find_last_of("}") == str.length() - 1)
	{
		// Remove wings
		str.pop_back();
		str.erase(0, 1);
		
		retValue = getElements(str.c_str(), &child1);
	}

	if (retValue)
	{
		//*result = new Tree("TABLE", nullptr, 0);
		(*result)->children.push_back(child1);
	}

	return retValue;
}

bool isElem(const char * text, Tree **result)
{
	std::string str(text);
	bool retValue = false;
	Tree *child1 = new Tree("ELEMENT", str, 0);

	if (isValue(text, &child1) || isDeclare(text, &child1)
		|| isHex(text, &child1) || isTable(text, &child1))
	{
		retValue = true;

		//*result = new Tree("ELEMENT", nullptr, 0);
		(*result)->children.push_back(child1);
	}

	if (str == "")
	{
		retValue = false;
	}

	return retValue;
}

bool isValue(const char* text, Tree **result)
{
	std::string str(text);
	bool retValue = false;
	Tree *child1 = new Tree("VALUE", str, 0);

	if (isBracet(text, &child1) || isString(text, &child1) || isDigit(text, &child1) || isVarible(text, &child1))
	{
		retValue = true;

		//*result = new Tree("VARIBLE", nullptr, 0);
		(*result)->children.push_back(child1);
	}

	return retValue;
}

bool isBracet(const char* text, Tree **result)
{
	std::string str(text);
	bool retValue = false;
	Tree *child1 = new Tree("BRACKET", str, 0);

	if (str.find_first_of("[") == 0 && str.find_last_of("]") == str.length() - 1)
	{
		// Remove wings
		str.pop_back();
		str.erase(0, 1);;

		if (isString(str.c_str(), &child1) || isDigit(str.c_str(), &child1))
		{
			retValue = true/*checkString(str.c_str())*/;

			//*result = new Tree("BRACKET", nullptr, 0);
			(*result)->children.push_back(child1);
		}
	}


	return retValue;
}

bool isString(const char* text, Tree **result)
{
	std::string str(text);
	bool retValue = false;
	Tree * child1 = new Tree("STRING", str, 0);

	if (str.find_first_of("\"") == 0 && str.find_last_of("\"") == str.length() - 1)
	{
		// Remove wings
		str.pop_back();
		str.erase(0, 1);;


		if (str.find("\"") == str.npos)
		{
			retValue = true/*checkString(str.c_str())*/;

			//*result = new Tree("STRING", nullptr, 0);
			(*result)->children.push_back(child1);
		}

		retValue = true/*checkString(str.c_str())*/;
	}

	return retValue;
}

bool isVarible(const char* text, Tree **result)
{
	std::string str(text);
	bool retValue = false;
	Tree *child1 = new Tree("VARIBLE", str, 0);
	
	CharClass letter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	CharClass alpha("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

	Seq startOnLetter({
		&letter,
		new Star(&alpha)
		});

	if (startOnLetter.match(text) == str.length())
	{
		retValue = true;

		//*result = new Tree("VARIBLE", nullptr, 0);
		(*result)->children.push_back(child1);
	}

	return retValue;
}

bool isDigit(const char* text, Tree **result)
{
	std::string str(text);
	bool retValue = false;
	Tree *child1 = new Tree("DIGIT", str, 0);

	CharClass digit("0123456789");

	Seq key({&digit, new Star(&digit) });

	if (key.match(text) == str.length())
	{
		retValue = true;

		//*result = new Tree("DIGIT", nullptr, 0);
		(*result)->children.push_back(child1);
	}

	return retValue;
}

bool isHex(const char * text, Tree **result)
{
	std::string str(text);
	bool retValue = false;
	Tree *child1 = new Tree("HEX", str, 0);

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

		//*result = new Tree("HEX", nullptr, 0);
		(*result)->children.push_back(child1);
	}

	return retValue;
}

bool isDeclare(const char * text, Tree **result)
{
	std::string str(text);
	bool retValue = false;
	Tree *child1 = new Tree("DECLARE", str, 0);

	int n = str.find_first_of("=");
	// only 1 =
	if (n == str.find_last_of("=") && n != -1)
	{
		std::string firstHalf(text);
		std::string secondHalf(text);

		firstHalf.erase(n,str.length());
		secondHalf.erase(0, n + 1);

		if (
			(isBracet(firstHalf.c_str(), &child1) || isString(firstHalf.c_str(), &child1) || /*isDigit(firstHalf.c_str()) || */isVarible(firstHalf.c_str(), &child1))
			&& 
			(isString(secondHalf.c_str(), &child1) || isDigit(secondHalf.c_str(), &child1) || isTable(secondHalf.c_str(), &child1) || isVarible(secondHalf.c_str(), &child1))
			)
		{
			retValue = true;

			//*result = new Tree("DECLARE", nullptr, 0);
			(*result)->children.push_back(child1);
		}
	}

	

	return retValue;
}
