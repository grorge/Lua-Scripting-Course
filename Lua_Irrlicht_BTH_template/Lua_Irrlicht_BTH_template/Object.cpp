#include "Object.h"


Object::Object(irr::scene::ISceneNode * parent, irr::scene::ISceneManager * smgr, irr::s32 ID,
	irr::core::array<irr::core::vector3df> verts)
	: irr::scene::ISceneNode(parent, smgr, ID)
{
	this->mater.Wireframe = false;
	this->mater.Lighting = false;

	int nrOfPoint = verts.size();

	for (int i = 0; i < nrOfPoint; i++)
	{
		this->verts[i] = irr::video::S3DVertex(verts[i].X, verts[i].Y, verts[i].Z,
			0, 0, 1, irr::video::SColor(255, 0, 255, 255), 0, 1);
	}

	this->bbox.reset(this->verts[0].Pos);
	for (int i = 1; i < nrOfPoint; ++i)
	{
		this->bbox.addInternalPoint(this->verts[i].Pos);
	}
}

Object::~Object()
{
}

void Object::OnRegisterSceneNode()
{
	this->SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();
}

void Object::render()
{
	irr::u16 indices[] = { 0,1,2 };
	irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

	driver->setMaterial(this->mater);
	driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
	driver->drawVertexPrimitiveList(&this->verts[0], 1, &indices[0], 1, irr::video::EVT_STANDARD, irr::scene::EPT_TRIANGLES, irr::video::EIT_16BIT);
}

Box::Box(irr::scene::ISceneNode * parent, irr::scene::ISceneManager * smgr, irr::s32 ID, 
	Vertex* verts, int nrOfPoints, std::string name)
	: irr::scene::ISceneNode(parent, smgr, ID)
{
	this->name = name;

	this->mater.Wireframe = false;
	this->mater.Lighting = false;

	irr::core::vector3di normal = { 0, 0, 1};
	int tu = 0;
	int tv = 0;

	for (int i = 0; i < nrOfPoints; i++)
	{
		if (i < 4)
		{
			normal = { 0, 0, 1 };

			if (i % 4 == 0)
			{
				tu = 0;
				tv = 1;
			}
			else if (i % 4 == 1)
			{
				tu = 0;
				tv = 0;
			}
			else if (i % 4 == 2)
			{
				tu = 1;
				tv = 0;
			}
			else
			{
				tu = 1;
				tv = 1;
			}
		}
		else if (i < 8)
		{
			normal = { 0, 0, -1 };

			if (i % 4 == 0)
			{
				tu = 1;
				tv = 1;
			}
			else if (i % 4 == 1)
			{
				tu = 0;
				tv = 1;
			}
			else if (i % 4 == 2)
			{
				tu = 0;
				tv = 0;
			}
			else
			{
				tu = 1;
				tv = 0;
			}
		}
		else if (i < 12)
		{
			normal = { 0, 1, 0 };

			if (i % 4 == 0)
			{
				tu = 0;
				tv = 1;
			}
			else if (i % 4 == 1)
			{
				tu = 0;
				tv = 0;
			}
			else if (i % 4 == 2)
			{
				tu = 1;
				tv = 0;
			}
			else
			{
				tu = 1;
				tv = 1;
			}
		}
		else if (i < 16)
		{
			normal = { 0, -1, 0 };

			if (i % 4 == 0)
			{
				tu = 1;
				tv = 1;
			}
			else if (i % 4 == 1)
			{
				tu = 0;
				tv = 1;
			}
			else if (i % 4 == 2)
			{
				tu = 0;
				tv = 0;
			}
			else
			{
				tu = 1;
				tv = 0;
			}
		}
		else if (i < 20)
		{
			normal = { -1, 0, 0 };

			if (i % 4 == 0)
			{
				tu = 0;
				tv = 1;
			}
			else if (i % 4 == 1)
			{
				tu = 0;
				tv = 0;
			}
			else if (i % 4 == 2)
			{
				tu = 1;
				tv = 0;
			}
			else
			{
				tu = 1;
				tv = 1;
			}
		}
		else if (i < 24)
		{
			normal = { 1, 0, 0 };

			if (i % 4 == 0)
			{
				tu = 0;
				tv = 1;
			}
			else if (i % 4 == 1)
			{
				tu = 0;
				tv = 0;
			}
			else if (i % 4 == 2)
			{
				tu = 1;
				tv = 0;
			}
			else
			{
				tu = 1;
				tv = 1;
			}
		}


		this->verts[i] = irr::video::S3DVertex(
			verts[i].vertexes.X, verts[i].vertexes.Y, verts[i].vertexes.Z,
			normal.X, normal.Y, normal.Z, 
			irr::video::SColor(255 * rand(), 255 * rand(), 255 * rand(), 255 * rand()), 
			tu, tv);
	}

	this->bbox.reset(this->verts[0].Pos);
	for (int i = 1; i < nrOfPoints; ++i)
	{
		this->bbox.addInternalPoint(this->verts[i].Pos);
	}
}

Box::~Box()
{
}

void Box::OnRegisterSceneNode()
{
	this->SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();
}

void Box::render()
{

	irr::u16 indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	//irr::u16 indices[] = { 0,1,2 };
	irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

	driver->setMaterial(this->mater);
	driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
	driver->drawVertexPrimitiveList(&this->verts[0], 24, &indices[0], 12, irr::video::EVT_STANDARD, irr::scene::EPT_TRIANGLES, irr::video::EIT_16BIT);
}
