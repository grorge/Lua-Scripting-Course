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

	std::string loadScene(std::string filename);

	std::string addMesh(Vertex* verts);
	std::string updatepos(irr::core::vector3df pos);
	int getpos();
	std::string addBox(irr::core::vector3d<irr::s32> pos, int size);
	std::string addBox(irr::core::vector3d<irr::s32> pos, int size, std::string name);

	std::string getNodes();
	std::string camera(irr::core::vector3df pos, irr::core::vector3df target);
	std::string snapshot(std::string filename);
	
	std::string addTexture(irr::video::SColor color, std::string name);
	std::string bind(std::string node, std::string texture);
	
private:
	lua_State* L;

	irr::video::IVideoDriver* driver = nullptr;
	irr::scene::ISceneManager* smgr = nullptr;
	irr::gui::IGUIEnvironment* guienv = nullptr;

	irr::core::array<irr::scene::ISceneNode*> nodes;
	//irr::core::array<Object*> meshes;

	int IDs = 100;
};



#endif // !INTERFACES_H
