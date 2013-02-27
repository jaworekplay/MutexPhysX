#pragma once
#include "MutexPhysX.h"

using namespace physx;
using namespace irr;
using namespace core;

//For now this class will only display the pyramid based on the irrlicht data
class CCustomNode : public irr::scene::ISceneNode
{
private:
	irr::core::aabbox3d<f32> box;
	int numberOfVerticies;
	video::S3DVertex Vertices[5];
	video::SMaterial material;
private:
	CMutex* m_physxBase;
public:
	CCustomNode( scene::ISceneNode* parent,
		scene::ISceneManager* smgr,
		int numOfVertices = 4,
		s32 id = -1)
		: scene::ISceneNode( parent,smgr,id), numberOfVerticies(numOfVertices)
	{
		material.Wireframe = true;
		material.Lighting  = false;

		vector3df I(0,0,20),II(-20,0,0),III(0,0,-20),IV(20,0,0),V(0,30,0);
		vector3df In(I),IIn(II),IIIn(III),IVn(IV),Vn(V);
		In.normalize();IIn.normalize();IIIn.normalize();IVn.normalize();Vn.normalize();

		Vertices[0] = video::S3DVertex(0,0,10, 1,1,0,
                video::SColor(255,0,255,255), 0, 1);
        Vertices[1] = video::S3DVertex(10,0,-10, 1,0,0,
                video::SColor(255,255,0,255), 1, 1);
        Vertices[2] = video::S3DVertex(0,20,0, 0,1,1,
                video::SColor(255,255,255,0), 1, 0);
        Vertices[3] = video::S3DVertex(-10,0,-10, 0,0,1,
                video::SColor(255,0,255,0), 0, 0);

		box.reset(Vertices[0].Pos);
		for( int i = 0; i < numberOfVerticies; ++i )
			box.addInternalPoint(Vertices[i].Pos);
	}
	virtual void OnRegisterSceneNode()
	{
		if( IsVisible )
			SceneManager->registerNodeForRendering(this);
		ISceneNode::OnRegisterSceneNode();
	}
	virtual void render()
	{//						 I     II     III    IV      V
		u16 indices[] = {  0,2,3, 2,1,3, 1,0,3, 2,0,1  };
		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(material);
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawVertexPrimitiveList( &Vertices[0],4, &indices[0],4, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
	}

	virtual const core::aabbox3d<irr::f32>& getBoundingBox() const { return box;}
	virtual irr::u32 getMaterialCount() const { return 1;}
	virtual video::SMaterial& getMaterial( irr::u32 i ) { return material;}
};