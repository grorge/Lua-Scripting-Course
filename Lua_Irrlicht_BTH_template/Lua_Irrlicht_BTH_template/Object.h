#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include <irrlicht.h>
#include <SColor.h>
#include <vector3d.h>
#include <string>

struct Vertex {
	Vertex(int x, int y, int z) : vertexes(x, y, z)
	{
		//vertexes.push_back(x);
		//vertexes.push_back(y);
		//vertexes.push_back(z);
	}

	irr::core::vector3d<int> vertexes;
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

class Box : public irr::scene::ISceneNode
{
public:
	Box(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* smgr, irr::s32 ID,
		Vertex* verts, int nrOfPoints, std::string name);
	~Box();

	// All SceneNodes have thier checks if they are ready for rendering
	virtual void OnRegisterSceneNode();

	virtual void render();

	std::string getName() { return this->name;	};

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
	irr::video::S3DVertex verts[24];
	irr::video::SMaterial mater;
	irr::core::aabbox3d<irr::f32> bbox;

	std::string name = std::string();
};

#endif // !OBJECT_H

