#pragma once
#include <irrlicht.h>
#include <PxPhysicsAPI.h>
#include "MutexPhysX.h"

using namespace irr;
using namespace physx;

class CMutex;

class CCustomNode : public irr::scene::ISceneNode
{
private:
	irr::core::aabbox3d<f32> box;
	int numberOfVerticies;
	video::S3DVertex Vertices[4];
	video::SMaterial material;
public:
	CCustomNode( scene::ISceneNode* parent,
		scene::ISceneManager* smgr,
		int numOfVertices = 4,
		s32 id = -1)
		: scene::ISceneNode( parent,smgr,id), numberOfVerticies(numOfVertices)
	{
		material.Wireframe = false;
		material.Lighting  = false;

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
	virtual void OnRegisterSceneNde()
	{
		if( IsVisible )
			SceneManager->registerNodeForRendering(this);
		ISceneNode::OnRegisterSceneNode();
	}
	virtual void render()
	{
		irr::u16 indices[] = {0,2,3  ,2,1,3  ,1,0,3,  2,0,1 };
		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(material);
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawVertexPrimitiveList( &Vertices[0],4, &indices[0],4);
	}

	virtual const core::aabbox3d<irr::f32>& getBoundingBox() const { return box;}
	virtual irr::u32 getMaterialCount() const { return 1;}
	virtual video::SMaterial& getMaterial( irr::u32 i ) { return material;}
};

class CPhysXNode
{
private:
	//------------------PHYSX STUFF-----------
	physx::PxRigidDynamic*	pxActor;
	physx::PxTransform		pose;
	//------------------IRRLICHT STUFF--------
	irr::scene::ISceneNode* irrActor;
	irr::core::vector3df	pos;
	CMutex* mutex;
	//------------------CUSTOM NODE-----------
	CCustomNode* irrCustom;
public:
	/*Constructor*/
	CPhysXNode( physx::PxRigidDynamic* physicsActor, irr::scene::ISceneNode* irrlichtActor )
	{
		pxActor = physicsActor;
		irrActor = irrlichtActor;
	}
	CPhysXNode( physx::PxRigidDynamic* physicsActor, CCustomNode* customNode )
	{
		pxActor = physicsActor;
		irrCustom = customNode;
		irrActor = NULL;
	}
	//Destructor
	~CPhysXNode(){}
	//Description
	//After creation call this function to
	//update positions of the objects
	//Function has to be called in the main game loop!
	virtual void updatePos()
	{
		pose = pxActor->getGlobalPose();
		pos.X = pose.p.x;
		pos.Y = pose.p.y;
		pos.Z = pose.p.z;
		/*if( typeid( irrActor ).name() == "class irr::scene::ISceneNode *" )*/
			irrActor->setPosition(pos);
		/*else
			irrCustom->setPosition(pos);
*/
		PxMat33 mat = PxMat33::PxMat33( pxActor->getGlobalPose().q ); //this code is from google code thing, because we use Rigid Dynamic already we don't need to static cast ;)
		irr::core::matrix4 irrM;
		irr::f32 fM[16];
		fM[0] = mat.column0.x;
		fM[1] = mat.column0.y;
		fM[2] = mat.column0.z;
		fM[4] = mat.column1.x;
		fM[5] = mat.column1.y;
		fM[6] = mat.column1.z;
		fM[8] = mat.column2.x;
		fM[9] = mat.column2.y;
		fM[10] = mat.column2.z;
		irrM.setM( fM );
		/*if( typeid( irrActor ).name() == "class irr::scene::ISceneNode *" )*/
			irrActor->setRotation( irrM.getRotationDegrees() );
		/*else
			irrCustom->setRotation( irrM.getRotationDegrees() );*/
	}
	irr::scene::ISceneNode* getIrrNode() const { return irrActor; }
	physx::PxRigidDynamic* getPhysXActor() const { return pxActor; }
	CCustomNode* getCustomNode() const { return irrCustom; }
};

class CPhysXCloth
{
	//these are initial steps to use cloth in our game
        //include them in the main class and return actor or node- preferrably node to aneble irrlicht rendering
        //can't wait to see that working
        //one issue I have, how on earth are we going to translate the cloth PhysX object to a renderable in Irrlicht object ?
        //Create cloth
      /*PxClothMeshDesc meshDesc;
	meshDesc.setToDefault();
	
	//Fill the geometry
	int w = 8, h=7;
	float hw = w / 2.0f;
	float hh = h / 2.0f;
	d = 0.2f;
	int numX = (int)(w / d) + 1;    
	int numY = (int)(h / d) + 1;    
	meshDesc.points.count= (numX+1) * (numY+1);
	meshDesc.triangles.count= numX*numY*2;
	meshDesc.points.stride= sizeof(PxVec3);  
	meshDesc.triangles.stride= 3*sizeof(PxU32);  
	meshDesc.points.data= (PxVec3*)malloc(sizeof(PxVec3)*meshDesc.points.count);    
	meshDesc.triangles.data= (PxU32*)malloc(sizeof(PxU32)*meshDesc.triangles.count*3);    
	meshDesc.edgeFlags = 0;
	
	//Fill the geometry
	int i,j;    
	PxVec3 *p = (PxVec3*)meshDesc.points.data;   
	
	pos.resize(meshDesc.points.count);
 	normal.resize(meshDesc.points.count);
	indices.resize(meshDesc.triangles.count*3);

	for (i = 0; i <= numY; i++) {        
	   for (j = 0; j <= numX; j++) {            
		  p->x = d*j-hw;
		  p->y = float(h);
		  p->z = d*i;             
		  p++;   
	   }    
	}   

	memcpy(&pos[0].x, (meshDesc.points.data), sizeof(PxVec3)*meshDesc.points.count);
	

	//Fill the topology
	PxU32 *id = (PxU32*)meshDesc.triangles.data;  
	for (i = 0; i < numY; i++) {        
	   for (j = 0; j < numX; j++) {            
		  PxU32 i0 = i * (numX+1) + j;            
		  PxU32 i1 = i0 + 1;            
		  PxU32 i2 = i0 + (numX+1);            
		  PxU32 i3 = i2 + 1;            
		  if ((j+i)%2) {                
			*id++ = i0; *id++ = i2; *id++ = i1;                
			*id++ = i1; *id++ = i2; *id++ = i3;            
		  } else {                
			*id++ = i0; *id++ = i2; *id++ = i3;                
			*id++ = i0; *id++ = i3; *id++ = i1;            
		  }      
	   }    
	}
	
	memcpy(&indices[0], meshDesc.triangles.data, sizeof(PxU32)*meshDesc.triangles.count*3);

	//Make sure everything is fine so far
	if(!(meshDesc.isValid()))
	   cerr<<"Mesh invalid."<<endl;

	//Start cooking of fibres
	PxCookingParams cp; 
	PxCooking* cooking = PxCreateCooking(PX_PHYSICS_VERSION, &(gPhysicsSDK->getFoundation()), cp);
	MemoryWriteBuffer buf;
	bool status = cooking->cookClothFabric(meshDesc,sceneDesc.gravity, buf);
	if(!status) {
	   cerr<<"Problem cooking mesh.\nExiting ..."<<endl;
	   exit(1);
	}
	    
	PxClothFabric* fabric=gPhysicsSDK->createClothFabric(MemoryReadBuffer(buf.data));
	
	PxTransform tr;
	tr.p = PxVec3(0,10,0); tr.q = PxQuat::createIdentity();

	PxClothParticle* points=(PxClothParticle*)malloc(sizeof(PxClothParticle)*meshDesc.points.count);
	p = (PxVec3*)meshDesc.points.data;  
	for(size_t i=0;i<meshDesc.points.count;i++) {
	   points[i].pos = *p;
	   //Fixing the top corner points
	   if(i==0 || i==numX) 
		  points[i].invWeight =0;
	   else 
		  points[i].invWeight = 1.f;
	   p++;
	}
	
	PxClothCollisionData cd;
	cd.setToDefault();
	
	cd.numSpheres=1;
	cd.pairIndexBuffer=0;
	
	box_collider.pos= PxVec3(0.0f,2.0f,0.0f);
	box_collider.radius=1;
	cd.spheres=&box_collider;
	
	cloth = gPhysicsSDK->createCloth(tr,*fabric,points, cd, PxClothFlag::eSWEPT_CONTACT);
	
 
	if(cloth) {	
		PxClothPhaseSolverConfig bendCfg;	 
		bendCfg.solverType= PxClothPhaseSolverConfig::eFAST;
		bendCfg.stiffness = 1;
		bendCfg.stretchStiffness = 0.5; 

		cloth->setPhaseSolverConfig(PxClothFabricPhaseType::eBENDING,		bendCfg) ;	
		cloth->setPhaseSolverConfig(PxClothFabricPhaseType::eSTRETCHING,	bendCfg) ;	
		cloth->setPhaseSolverConfig(PxClothFabricPhaseType::eSHEARING,		bendCfg) ;	
		cloth->setPhaseSolverConfig(PxClothFabricPhaseType::eSTRETCHING_HORIZONTAL, bendCfg) ;
	 
	   cloth->setDampingCoefficient(0.125f);	   
	   gScene->addActor(*cloth); 
	}
	else
	   cerr<<"Cannot create cloth"<<endl;	

        void RenderCloth() 
        {
          	glEnableClientState(GL_VERTEX_ARRAY);
          	glEnableClientState(GL_NORMAL_ARRAY);
          
          	glVertexPointer(3, GL_FLOAT, sizeof(PxVec3), &(pos[0].x));
          	glNormalPointer(GL_FLOAT, sizeof(PxVec3), &(normal[0].x));
          
          	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, &indices[0]);
          
          	glDisableClientState(GL_NORMAL_ARRAY);
          	glDisableClientState(GL_VERTEX_ARRAY);
        }	
        
           //update the cloth data
		PxClothReadData* pData = cloth->lockClothReadData();
		PxClothParticle* pParticles = const_cast<PxClothParticle*>(pData->particles);
		
		//update the positions
		for(size_t i=0;i<pos.size();i++) {

			pos[i] = pParticles[i].pos;
			if(pos[i].y<0) 
                        {
				pos[i].y=0;
				pParticles[i].pos.y=0;
			}
		}
		pData->unlock();

		//update normals
		for(size_t i=0;i<indices.size();i+=3) 
                {
			PxVec3 p1 = pos[indices[i]];
			PxVec3 p2 = pos[indices[i+1]];
			PxVec3 p3 = pos[indices[i+2]];
			PxVec3 n  = (p2-p1).cross(p3-p1);

			normal[indices[i]]    += n/3.0f ; 
			normal[indices[i+1]]  += n/3.0f ; 
			normal[indices[i+2]]  += n/3.0f ; 			
		}

		for(size_t i=0;i<normal.size();i++) 
                {
			PxVec3& n  = normal[i];
			n= n.getNormalized();
 		}*/
};