#pragma once
#ifndef INTERFACES_H
#define INTERFACES_H

#include <irrlicht.h>
#include <SColor.h>
#include <vector3d.h>
#include <string>
#include <Windows.h>
#include <iostream>

#include <SMesh.h>

struct Vertex {
	Vertex(int x, int y, int z) : vertexes(x, y, z)
	{
		//vertexes.push_back(x);
		//vertexes.push_back(y);
		//vertexes.push_back(z);
	}

	irr::core::vector3d<int> vertexes;
};

class Interface
{
public:
	Interface(irr::video::IVideoDriver* driver, irr::scene::ISceneManager* smgr, irr::gui::IGUIEnvironment* guienv);
	~Interface();

	std::wstring loadScene(std::wstring filename);

	std::wstring addMesh(Vertex* verts);
	std::wstring addBox(irr::core::vector3d<irr::s32> pos, int size);
	std::wstring addBox(irr::core::vector3d<irr::s32> pos, int size, std::wstring name);

	std::wstring getNodes();
	std::wstring camera();
	std::wstring snapshot(std::wstring filename);
	
	std::wstring addTexture(irr::video::SColor color, std::string name);
	std::wstring bind(std::string node, std::string texture);
	
private:
	irr::video::IVideoDriver* driver = nullptr;
	irr::scene::ISceneManager* smgr = nullptr;
	irr::gui::IGUIEnvironment* guienv = nullptr;

	int IDs = 100;
};

class Object : public irr::scene::ISceneNode
{
public:
	Object(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* smgr, irr::s32 ID, 
		Vertex* verts, int nrOfPoints);
	~Object();

	// All SceneNodes have thier checks if they are ready for rendering
	virtual void OnRegisterSceneNode();

	virtual void render();

	virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const
	{
		return this->bbox;
	}

	virtual irr::u32 getMaterialCount() const
	{
		return 1;
	}

	virtual irr::video::SMaterial& getMaterial(irr::u32 i)
	{
		return this->mater;
	}

private:
	//pos - normal - color - texturecoord	
	irr::video::S3DVertex verts[3];
	irr::video::SMaterial mater;
	irr::core::aabbox3d<irr::f32> bbox;
};


#endif // !INTERFACES_H
