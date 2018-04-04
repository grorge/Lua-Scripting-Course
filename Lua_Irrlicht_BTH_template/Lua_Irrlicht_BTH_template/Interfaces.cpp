#include "Interfaces.h"

Interface::Interface(irr::video::IVideoDriver * driver, irr::scene::ISceneManager * smgr, irr::gui::IGUIEnvironment * guienv)
{
	this->driver = driver;
	this->smgr = smgr;
	this->guienv = guienv;

	this->IDs = 100;
}

Interface::~Interface()
{
}

std::wstring Interface::addMesh(Vertex * verts)
{
	Object* testObj;
	testObj = new Object(this->smgr->getRootSceneNode(), this->smgr, this->IDs, verts, 3);
	this->IDs++;
	
	return L"Simple Triangle in the plane.";
}

std::wstring Interface::addBox(irr::core::vector3d<irr::s32> pos, int size)
{
	std::wstring generatedName = L"name" + this->IDs;
	
	return addBox(pos, size, generatedName);
}

std::wstring Interface::addBox(irr::core::vector3d<irr::s32> pos, int size, std::wstring name)
{
	return std::wstring();
}

Object::Object(irr::scene::ISceneNode * parent, irr::scene::ISceneManager * smgr, irr::s32 ID, 
	Vertex* verts, int nrOfPoint)
	: irr::scene::ISceneNode(parent, smgr, ID)
{
	this->mater.Wireframe = false;
	this->mater.Lighting = false;

	for (int i = 0; i < nrOfPoint; i++)
	{
		this->verts[i] = irr::video::S3DVertex(verts[i].vertexes.X, verts[i].vertexes.Y, verts[i].vertexes.Z,
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

Box::Box(irr::scene::ISceneNode * parent, irr::scene::ISceneManager * smgr, irr::s32 ID, Vertex * verts, int nrOfPoints)
	: irr::scene::ISceneNode(parent, smgr, ID)
{
	this->mater.Wireframe = false;
	this->mater.Lighting = false;

	for (int i = 0; i < nrOfPoints; i++)
	{
		this->verts[i] = irr::video::S3DVertex(verts[i].vertexes.X, verts[i].vertexes.Y, verts[i].vertexes.Z,
			0, 0, 1, irr::video::SColor(255, 0, 255, 255), 0, 1);
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
	irr::u16 indices[] = { 0,1,2 };
	irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

	driver->setMaterial(this->mater);
	driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
	driver->drawVertexPrimitiveList(&this->verts[0], 1, &indices[0], 1, irr::video::EVT_STANDARD, irr::scene::EPT_TRIANGLES, irr::video::EIT_16BIT);
}
