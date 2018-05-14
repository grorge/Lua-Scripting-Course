#pragma once
#ifndef INTERFACES_H
#define INTERFACES_H

#include <Windows.h>
#include <iostream>
#include <lua.hpp>
#include "lua.hpp"

#include "Object.h"


class Interface
{
public:
	Interface();
	Interface(irr::video::IVideoDriver* driver, irr::scene::ISceneManager* smgr, irr::gui::IGUIEnvironment* guienv, lua_State* L_state);
	~Interface();

	int loadScene(std::string filename);

	int addMesh(irr::core::array<irr::core::vector3df> posVectors);
	int updatepos(irr::core::vector3df pos);
	int getpos();
	int addBox(irr::core::vector3df pos, float size);
	int addBox(irr::core::vector3df pos, float size, std::string name);

	int getNodes();
	int camera(irr::core::vector3df pos, irr::core::vector3df target);
	int snapshot(std::string filename);
	
	int addTexture(irr::video::SColor color, std::string name);
	int bind(std::string node, std::string texture);
	
	irr::scene::ICameraSceneNode* getCam() { return this->cam; 	};
	void setCam(irr::scene::ICameraSceneNode* input) { this->cam = input; };

	std::string takeShot() { 
		std::string temp = this->screenshot;
		this->screenshot == "";
		return temp;	
	};

private:
	lua_State* L;

	irr::video::IVideoDriver* driver = nullptr;
	irr::scene::ISceneManager* smgr = nullptr;
	irr::gui::IGUIEnvironment* guienv = nullptr;

	irr::core::array<irr::scene::ISceneNode*> nodes;
	irr::scene::ICameraSceneNode* cam;
	//irr::core::array<Object*> meshes;

	int IDs = 100;

	std::string screenshot = "";
};



#endif // !INTERFACES_H
