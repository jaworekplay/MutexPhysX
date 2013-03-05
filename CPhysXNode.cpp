#pragma once
#include "CPhysXNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "os.h"
#include "CShadowVolumeSceneNode.h"
#include "IAnimatedMeshMD3.h"
#include "CSkinnedMesh.h"
#include "IDummyTransformationSceneNode.h"
#include "IBoneSceneNode.h"
#include "IMaterialRenderer.h"
#include "IMesh.h"
#include "IMeshCache.h"
#include "IAnimatedMesh.h"
#include "quaternion.h"
#include <cmath>
//jaworekplay
//update included in the vc directories->include directories set Irrlicht source folder
namespace custom
{
	namespace scene
	{
		//! constructor
		CPhysXNode::CPhysXNode( irr::scene::IAnimatedMesh* mesh, 
			physx::PxRigidActor* actor,
			ISceneNode* parent, 
			irr::scene::ISceneManager* mgr,
			irr::s32 id,
			const irr::core::vector3df& position, 
			const irr::core::vector3df& rotation,
			const irr::core::vector3df& scale)
		: IAnimatedMeshSceneNode(parent, mgr, id, position, rotation, scale), Mesh(0), Actor(actor),
			BeginFrameTime(0), StartFrame(0), EndFrame(0), FramesPerSecond(25.f / 1000.f ),
			CurrentFrameNr(0.f), JointMode(irr::scene::EJUOR_NONE), JointsUsed(false),
			TransitionTime(0), Transiting(0.f), TransitingBlend(0.f),
			Looping(true), ReadOnlyMaterials(false),
			LoopCallBack(0), PassCount(0), Shadow(0), RenderFromIdentity(0)
		{
			#ifdef _DEBUG
			setDebugName("CPhysXNode");
			#endif

			//BeginFrameTime = timer->getTime(); //os::Timer::getTime();

			setMesh(mesh);
		}


		//! destructor
		CPhysXNode::~CPhysXNode()
		{
			if (Mesh)
				Mesh->drop();

			if (Shadow)
				Shadow->drop();

			//for (u32 i=0; i<JointChildSceneNodes.size(); ++i)
			//	if (JointChildSceneNodes[i])
			//		JointChildSceneNodes[i]->drop();

			if (LoopCallBack)
				LoopCallBack->drop();
		}


		//! Sets the current frame. From now on the animation is played from this frame.
		void CPhysXNode::setCurrentFrame(irr::f32 frame)
		{
			// if you pass an out of range value, we just clamp it
			//CurrentFrameNr = core::clamp ( frame, (f32)StartFrame, (f32)EndFrame );

			//  os::Timer::getTime()
			//BeginFrameTime = timer->getTime() - (s32)((CurrentFrameNr - StartFrame) / FramesPerSecond);

			//beginTransition(); //transit to this frame if enabled
		}


		//! Returns the current displayed frame number.
		irr::f32 CPhysXNode::getFrameNr() const
		{
			return CurrentFrameNr;
		}


		irr::f32 CPhysXNode::buildFrameNr(irr::u32 timeMs)
		{
			if (Transiting!=0.f)
			{
				TransitingBlend = (irr::f32)(timeMs-BeginFrameTime) * Transiting;
				if (TransitingBlend > 1.f)
				{
					Transiting=0.f;
					TransitingBlend=0.f;
				}
			}

			if (StartFrame==EndFrame)
				return (irr::f32)StartFrame; //Support for non animated meshes
			if (FramesPerSecond==0.f)
				return (irr::f32)StartFrame;

			if (Looping)
			{
				// play animation looped

				if (FramesPerSecond > 0.f) //forwards...
				{
					const irr::s32 lenInTime = irr::s32( irr::f32(EndFrame - StartFrame) / FramesPerSecond);
					return StartFrame + ( (timeMs - BeginFrameTime) % lenInTime) *FramesPerSecond;
				}
				else //backwards...
				{
					const irr::s32 lenInTime = irr::s32( irr::f32(EndFrame - StartFrame) / -FramesPerSecond);
					return EndFrame - ( (timeMs - BeginFrameTime) % lenInTime)*-FramesPerSecond;
				}
			}
			else
			{
				// play animation non looped

				irr::f32 frame;

				if (FramesPerSecond > 0.f) //forwards...
				{
					const irr::f32 deltaFrame = floorf( irr::f32 ( timeMs - BeginFrameTime ) * FramesPerSecond );

					frame = StartFrame + deltaFrame;

					if (frame > (irr::f32)EndFrame)
					{
						frame = (irr::f32)EndFrame;
						if (LoopCallBack)
							LoopCallBack->OnAnimationEnd(this);
					}
				}
				else //backwards... (untested)
				{
					const irr::f32 deltaFrame = floorf( irr::f32 ( timeMs - BeginFrameTime ) * -FramesPerSecond );

					frame = EndFrame - deltaFrame;

					if (frame < (irr::f32)StartFrame)
					{
						frame = (irr::f32)StartFrame;
						if (LoopCallBack)
							LoopCallBack->OnAnimationEnd(this);
					}

				}

				return frame;
			}
		}

		//! frame
		void CPhysXNode::OnRegisterSceneNode()
		{
			if (IsVisible)
			{
				// because this node supports rendering of mixed mode meshes consisting of
				// transparent and solid material at the same time, we need to go through all
				// materials, check of what type they are and register this node for the right
				// render pass according to that.
				//jaworekplay
				//update
				//-----changed from isSleeping  to isRigidActor() as there 
				//is no function like that in PhysX 3.x.x
				//possibly change this to something else as  the condition may not be accurate
				if (Actor && !Actor->isRigidActor() )
				{
					
					//physx::PxBounds3 pose = Actor->getWorldBounds();
					//
					//const physx::PxVec3 pos = pose.getCenter();
					//
					//const physx::PxMat33 orient = pose.transform( &orient, pose.getCenter() );
					//
					//irr::core::matrix4 irrMat; // a 4x4 matrix in the irrlicht engine
					//
					//orient.getDeterminant();
					//
					//pos.getNormalized();
					////clear the elements we don't need:
					//irrMat[3] = irrMat[7] = irrMat[11] = 0.0f;
					//irrMat[15] = 1.0f;

					//// with that newly made matrix, let's use it to transform/rotate the node
					//setPosition(irrMat.getTranslation());
					//setRotation(irrMat.getRotationDegrees());
				}

				irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

				PassCount = 0;
				int transparentCount = 0;
				int solidCount = 0;

				// count transparent and solid materials in this scene node
				for (irr::u32 i=0; i<Materials.size(); ++i)
				{
					irr::video::IMaterialRenderer* rnd =
						driver->getMaterialRenderer(Materials[i].MaterialType);

					if (rnd && rnd->isTransparent())
						++transparentCount;
					else
						++solidCount;

					if (solidCount && transparentCount)
						break;
				}

				// register according to material types counted

				if (solidCount)
					SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_SOLID);

				if (transparentCount)
					SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_TRANSPARENT);

				ISceneNode::OnRegisterSceneNode();

				for (irr::u32 j=0; j<JointChildSceneNodes.size(); ++j)
					if (JointChildSceneNodes[j])
						JointChildSceneNodes[j]->OnRegisterSceneNode();
			}
		}



		//! OnAnimate() is called just before rendering the whole scene.
		void CPhysXNode::OnAnimate(irr::u32 timeMs)
		{

			CurrentFrameNr = buildFrameNr ( timeMs );

			if ( Mesh )
			{
				/*
				scene::IMesh *m = Mesh->getMesh(CurrentFrameNr, 255, StartFrame, EndFrame);
				if ( m )
				{
					Box = m->getBoundingBox();
				}*/
		
			}


			IAnimatedMeshSceneNode::OnAnimate ( timeMs );
		}


		/*
			angle = dotproduct ( v(0,1,0), up )
			axis = crossproduct ( v(0,1,0), up )
		*/
		inline void AlignToUpVector(irr::core::matrix4 &m, const irr::core::vector3df &up )
		{
			irr::core::quaternion quatRot( up.Z, 0.f, -up.X, 1 + up.Y );
			quatRot.normalize();
			//jaworekplay
			//to do
			quatRot.getMatrix(m,up);
		}


		//! renders the node.
		void CPhysXNode::render()
		{
			irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

			if (!Mesh || !driver)
				return;


			bool isTransparentPass =
				SceneManager->getSceneNodeRenderPass() == irr::scene::ESNRP_TRANSPARENT;

			++PassCount;

			irr::f32 frame = getFrameNr();

			irr::scene::IMesh* m;

			if (Mesh->getMeshType() != irr::scene::EAMT_SKINNED)
				m = Mesh->getMesh((irr::s32)frame, 255, StartFrame, EndFrame);
			else
			{
				irr::scene::CSkinnedMesh* skinnedMesh = reinterpret_cast<irr::scene::CSkinnedMesh*>(Mesh);

				if (JointMode == EJUOR_CONTROL)//write to mesh
					skinnedMesh->transferJointsToMesh(JointChildSceneNodes);
				else
					skinnedMesh->animateMesh(frame, 1.0f);

				skinnedMesh->skinMesh();

				if (JointMode == irr::scene::EJUOR_READ)//read from mesh
				{
					skinnedMesh->recoverJointsFromMesh(JointChildSceneNodes);

					//---slow---
					for (irr::u32 n=0;n<JointChildSceneNodes.size();++n)
						if (JointChildSceneNodes[n]->getParent()==this)
						{
							JointChildSceneNodes[n]->updateAbsolutePositionOfAllChildren(); //temp, should be an option
						}

				}


				m=skinnedMesh;
			}


			if ( 0 == m )
			{
				#ifdef _DEBUG
					//os::Printer::log("Animated Mesh returned no mesh to render.", Mesh->getDebugName(), ELL_WARNING);
				#endif
			}

			driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);

			//jaworekplay
			//to do
			//trial
			if (Shadow && PassCount==1)
				//Shadow->setMeshToRenderFrom(m);
				Shadow->setShadowMesh(m);

			// for debug purposes only:

			irr::u32 renderMeshes = 1;
			irr::video::SMaterial mat;
			if (DebugDataVisible && PassCount==1)
			{
				// overwrite half transparency
				if ( DebugDataVisible & irr::scene::EDS_HALF_TRANSPARENCY )
				{
					for (irr::u32 g=0; g<m->getMeshBufferCount(); ++g)
					{
						mat = Materials[g];
						mat.MaterialType = irr::video::EMT_TRANSPARENT_ADD_COLOR;
						driver->setMaterial(mat);
						driver->drawMeshBuffer ( m->getMeshBuffer(g) );
					}
					renderMeshes = 0;
				}
			}

			// render original meshes
			if ( renderMeshes )
			{
				for (irr::u32 i=0; i<m->getMeshBufferCount(); ++i)
				{
					irr::video::IMaterialRenderer* rnd = driver->getMaterialRenderer(Materials[i].MaterialType);
					bool transparent = (rnd && rnd->isTransparent());

					// only render transparent buffer if this is the transparent render pass
					// and solid only in solid pass
					if (transparent == isTransparentPass)
					{
						irr::scene::IMeshBuffer* mb = m->getMeshBuffer(i);

						if (RenderFromIdentity)
							driver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4() );
						else if (Mesh->getMeshType() == irr::scene::EAMT_SKINNED)
							driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation * ((irr::scene::SSkinMeshBuffer*)mb)->Transformation);


						driver->setMaterial(Materials[i]);
						driver->drawMeshBuffer(mb);
					}
				}
			}

			driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);

			// for debug purposes only:
			if (DebugDataVisible && PassCount==1)
			{
				mat.Lighting = false;
				driver->setMaterial(mat);

				// show bounding box
				if ( DebugDataVisible & irr::scene::EDS_BBOX_BUFFERS )
				{
					for (irr::u32 g=0; g< m->getMeshBufferCount(); ++g)
					{
						driver->draw3DBox( m->getMeshBuffer(g)->getBoundingBox(),
								irr::video::SColor(0,190,128,128) );
					}
				}

				if ( DebugDataVisible & irr::scene::EDS_BBOX )
					driver->draw3DBox(Box, irr::video::SColor(0,255,255,255));

				// show skeleton
				if ( DebugDataVisible & irr::scene::EDS_SKELETON )
				{
					if (Mesh->getMeshType() == irr::scene::EAMT_SKINNED)
					{

						// draw skeleton


						for (irr::u32 g=0; g < ((irr::scene::ISkinnedMesh*)Mesh)->getAllJoints().size(); ++g)
						{
							irr::scene::ISkinnedMesh::SJoint *joint=((irr::scene::ISkinnedMesh*)Mesh)->getAllJoints()[g];

							for (irr::u32 n=0;n<joint->Children.size();++n)
							{
								driver->draw3DLine(joint->GlobalAnimatedMatrix.getTranslation(), joint->Children[n]->GlobalAnimatedMatrix.getTranslation(),  irr::video::SColor(0,51,66,255));
							}
						}
					}

					// show tag for quake3 models
					if (Mesh->getMeshType() == irr::scene::EAMT_MD3 )
					{
						irr::scene::IAnimatedMesh * arrow =
							SceneManager->addArrowMesh (
									"__tag_show",
									0xFF0000FF, 0xFF000088,
									4, 8, 5.f, 4.f, 0.5f,
									1.f);
						if ( 0 == arrow )
						{
							arrow = SceneManager->getMesh ( "__tag_show" );
						}
						irr::scene::IMesh *arrowMesh = arrow->getMesh ( 0 );

						irr::video::SMaterial material;
						material.Lighting = false;
						driver->setMaterial(material);

						irr::core::matrix4 matr;

						irr::scene::SMD3QuaternionTagList *taglist = ((irr::scene::IAnimatedMeshMD3*)Mesh)->getTagList (	(irr::s32)getFrameNr(),
														255,
														getStartFrame (),
														getEndFrame ()
													);
						if ( taglist )
						{
							for ( irr::u32 ts = 0; ts != taglist->size(); ++ts )
							{
								(*taglist)[ts].setto ( matr );

								driver->setTransform(irr::video::ETS_WORLD, matr );

								for ( irr::u32 a = 0; a != arrowMesh->getMeshBufferCount(); ++a )
									driver->drawMeshBuffer ( arrowMesh->getMeshBuffer ( a ) );
							}
						}
					}
				}

				// show normals
				if ( DebugDataVisible & irr::scene::EDS_NORMALS )
				{
					irr::scene::IAnimatedMesh * arrow = SceneManager->addArrowMesh (
							"__debugnormal", 0xFFECEC00,
							0xFF999900, 4, 8, 1.f, 0.6f, 0.05f,
							0.3f);
					if ( 0 == arrow )
					{
						arrow = SceneManager->getMesh ( "__debugnormal" );
					}
					irr::scene::IMesh *mesh = arrow->getMesh ( 0 );

					// find a good scaling factor

					irr::core::matrix4 m2;

					// draw normals
					for (irr::u32 g=0; g<m->getMeshBufferCount(); ++g)
					{
						const irr::scene::IMeshBuffer* mb = m->getMeshBuffer(g);
						const irr::u32 vSize = irr::video::getVertexPitchFromType(mb->getVertexType());
						const irr::video::S3DVertex* v = ( const irr::video::S3DVertex*)mb->getVertices();
						for ( irr::u32 i=0; i != mb->getVertexCount(); ++i )
						{
							AlignToUpVector ( m2, v->Normal );

							m2.setTranslation(v->Pos);
							m2*=AbsoluteTransformation;

							driver->setTransform(irr::video::ETS_WORLD, m2 );
							for ( irr::u32 a = 0; a != mesh->getMeshBufferCount(); ++a )
								driver->drawMeshBuffer ( mesh->getMeshBuffer ( a ) );

							v = (const irr::video::S3DVertex*) ( (irr::u8*) v + vSize );
						}
					}
					driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
				}

				// show mesh
				if ( DebugDataVisible & irr::scene::EDS_MESH_WIRE_OVERLAY )
				{
					mat.Lighting = false;
					mat.Wireframe = true;
					driver->setMaterial(mat);

					for (irr::u32 g=0; g<m->getMeshBufferCount(); ++g)
					{
						driver->drawMeshBuffer( m->getMeshBuffer(g) );
					}
				}
			}
		}


		//! Returns the current start frame number.
		irr::s32 CPhysXNode::getStartFrame() const
		{
			return StartFrame;
		}

		//! Returns the current start frame number.
		irr::s32 CPhysXNode::getEndFrame() const
		{
			return EndFrame;
		}

		//! sets the frames between the animation is looped.
		//! the default is 0 - MaximalFrameCount of the mesh.
		bool CPhysXNode::setFrameLoop(irr::s32 begin, irr::s32 end)
		{
			const irr::s32 maxFrameCount = Mesh->getFrameCount() - 1;
			if ( end < begin )
			{
				StartFrame = irr::core::s32_clamp(end, 0, maxFrameCount);
				EndFrame = irr::core::s32_clamp(begin, StartFrame, maxFrameCount);
			}
			else
			{
				StartFrame = irr::core::s32_clamp(begin, 0, maxFrameCount);
				EndFrame = irr::core::s32_clamp(end, StartFrame, maxFrameCount);
			}
			setCurrentFrame ( (irr::f32)StartFrame );

			return true;
		}



		//! sets the speed with witch the animation is played
		void CPhysXNode::setAnimationSpeed(irr::f32 framesPerSecond)
		{
			FramesPerSecond = framesPerSecond * 0.001f;
		}



		//! returns the axis aligned bounding box of this node
		const irr::core::aabbox3d<irr::f32>& CPhysXNode::getBoundingBox() const
		{
			return Box;
		}



		//! returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hirachy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.
		irr::video::SMaterial& CPhysXNode::getMaterial(irr::u32 i)
		{
			if ( i >= Materials.size() )
				return ISceneNode::getMaterial(i);

			return Materials[i];
		}



		//! returns amount of materials used by this scene node.
		irr::u32 CPhysXNode::getMaterialCount() const
		{
			return Materials.size();
		}


		//! Creates shadow volume scene node as child of this node
		//! and returns a pointer to it.
		irr::scene::IShadowVolumeSceneNode* CPhysXNode::addShadowVolumeSceneNode(irr::s32 id,
								 bool zfailmethod, irr::f32 infinity)
		{
			if (!SceneManager->getVideoDriver()->queryFeature(irr::video::EVDF_STENCIL_BUFFER))
				return 0;

			if (Shadow)
			{
				//os::Printer::log("This node already has a shadow.", ELL_WARNING);
				return 0;
			}

			//Shadow = new CShadowVolumeSceneNode(this, SceneManager, id,  zfailmethod, infinity);
			return Shadow;
		}


		irr::scene::IBoneSceneNode* CPhysXNode::getJointNode(const irr::c8* jointName)
		{
			if (!Mesh || Mesh->getMeshType() != irr::scene::EAMT_SKINNED)
				return 0;

			checkJoints();


			irr::scene::ISkinnedMesh *skinnedMesh=(irr::scene::ISkinnedMesh*)Mesh;

			irr::s32 number = skinnedMesh->getJointNumber(jointName);

			if (number == -1)
			{
				//os::Printer::log("Joint with specified name not found in skinned mesh.", jointName, ELL_WARNING);
				return 0;
			}

			if ((irr::s32)JointChildSceneNodes.size() <= number)
			{
				//os::Printer::log("Joint was found in mesh, but is not loaded into node", jointName, ELL_WARNING);
				return 0;
			}

			return getJointNode((irr::u32)number);
		}


		irr::scene::IBoneSceneNode* CPhysXNode::getJointNode(irr::u32 jointID)
		{
			if (JointChildSceneNodes.size() <= jointID)
			{
				//os::Printer::log("Joint not loaded into node", ELL_WARNING);
				return 0;
			}

			return JointChildSceneNodes[jointID];
		}


		//! Returns a pointer to a child node, which has the same transformation as
		//! the corrsesponding joint, if the mesh in this scene node is a ms3d mesh.
		irr::scene::ISceneNode* CPhysXNode::getMS3DJointNode(const irr::c8* jointName)
		{
			return  getJointNode(jointName);
		}


		//! Returns a pointer to a child node, which has the same transformation as
		//! the corrsesponding joint, if the mesh in this scene node is a ms3d mesh.
		irr::scene::ISceneNode* CPhysXNode::getXJointNode(const irr::c8* jointName)
		{
			return  getJointNode(jointName);
		}


		//! Removes a child from this scene node.
		//! Implemented here, to be able to remove the shadow properly, if there is one,
		//! or to remove attached childs.
		bool CPhysXNode::removeChild(irr::scene::ISceneNode* child)
		{
			if (child && Shadow == child)
			{
				Shadow->drop();
				Shadow = 0;
				return true;
			}

			if (irr::scene::ISceneNode::removeChild(child))
			{
				if (JointsUsed) //stop weird bugs caused while changing parents as the joints are being created
				{
					for (irr::u32 i=0; i<JointChildSceneNodes.size(); ++i)
					if (JointChildSceneNodes[i] == child)
					{
						JointChildSceneNodes[i] = 0; //remove link to child
						return true;
					}
				}

				return true;
			}

			return false;
		}


		//! Starts a MD2 animation.
		bool CPhysXNode::setMD2Animation(irr::scene::EMD2_ANIMATION_TYPE anim)
		{
			if (!Mesh || Mesh->getMeshType() != irr::scene::EAMT_MD2)
				return false;

			irr::scene::IAnimatedMeshMD2* m = (irr::scene::IAnimatedMeshMD2*)Mesh;

			irr::s32 begin, end, speed;
			m->getFrameLoop(anim, begin, end, speed);

			setAnimationSpeed( irr::f32(speed) );
			setFrameLoop(begin, end);
			return true;
		}


		//! Starts a special MD2 animation.
		bool CPhysXNode::setMD2Animation(const irr::c8* animationName)
		{
			if (!Mesh || Mesh->getMeshType() != irr::scene::EAMT_MD2)
				return false;

			irr::scene::IAnimatedMeshMD2* m = (irr::scene::IAnimatedMeshMD2*)Mesh;

			irr::s32 begin, end, speed;
			if (!m->getFrameLoop(animationName, begin, end, speed))
				return false;

			setAnimationSpeed( (irr::f32)speed );
			setFrameLoop(begin, end);
			return true;
		}


		//! Sets looping mode which is on by default. If set to false,
		//! animations will not be looped.
		void CPhysXNode::setLoopMode(bool playAnimationLooped)
		{
			Looping = playAnimationLooped;
		}


		//! Sets a callback interface which will be called if an animation
		//! playback has ended. Set this to 0 to disable the callback again.
		void CPhysXNode::setAnimationEndCallback(irr::scene::IAnimationEndCallBack* callback)
		{
			if (LoopCallBack)
				LoopCallBack->drop();

			LoopCallBack = callback;

			if (LoopCallBack)
				LoopCallBack->grab();
		}


		//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
		void CPhysXNode::setReadOnlyMaterials(bool readonly)
		{
			ReadOnlyMaterials = readonly;
		}


		//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
		bool CPhysXNode::isReadOnlyMaterials() const
		{
			return ReadOnlyMaterials;
		}


		//! Writes attributes of the scene node.
		void CPhysXNode::serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const
		{
			irr::scene::IAnimatedMeshSceneNode::serializeAttributes(out, options);
			//jaworekplay
			//to do:
			//old
			//out->addString("Mesh", SceneManager->getMeshCache()->getMeshFilename(Mesh));
			//update changed last function to getDebugName()
			//
			//			/
			//DONE!   \/
			out->addString( "Mesh", SceneManager->getMeshCache()->getDebugName() );
			out->addBool("Looping", Looping);
			out->addBool("ReadOnlyMaterials", ReadOnlyMaterials);
			out->addFloat("FramesPerSecond", FramesPerSecond);

			// TODO: write animation names instead of frame begin and ends
		}


		//! Reads attributes of the scene node.
		void CPhysXNode::deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options)
		{
			irr::scene::IAnimatedMeshSceneNode::deserializeAttributes(in, options);

			irr::core::stringc oldMeshStr = SceneManager->getMeshCache()->getMeshFilename(Mesh);
			irr::core::stringc newMeshStr = in->getAttributeAsString("Mesh");

			Looping = in->getAttributeAsBool("Looping");
			ReadOnlyMaterials = in->getAttributeAsBool("ReadOnlyMaterials");
			FramesPerSecond = in->getAttributeAsFloat("FramesPerSecond");

			if (newMeshStr != "" && oldMeshStr != newMeshStr)
			{
				irr::scene::IAnimatedMesh* newAnimatedMesh = SceneManager->getMesh(newMeshStr.c_str());

				if (newAnimatedMesh)
					setMesh(newAnimatedMesh);
			}

			// TODO: read animation names instead of frame begin and ends
		}


		//! Sets a new mesh
		void CPhysXNode::setMesh(irr::scene::IAnimatedMesh* mesh)
		{
			if (!mesh)
				return; // won't set null mesh

			if (Mesh)
				Mesh->drop();

			Mesh = mesh;

			// get materials and bounding box
			Box = Mesh->getBoundingBox();

			irr::scene::IMesh* m = Mesh->getMesh(0,0);
			if (m)
			{
				Materials.clear();

				irr::video::SMaterial mat;
				for (irr::u32 i=0; i<m->getMeshBufferCount(); ++i)
				{
					irr::scene::IMeshBuffer* mb = m->getMeshBuffer(i);
					if (mb)
						mat = mb->getMaterial();

					Materials.push_back(mat);
				}
			}

			// get start and begin time
			setFrameLoop ( 0, Mesh->getFrameCount() );

			// grab the mesh
			if (Mesh)
				Mesh->grab();
		}

		// returns the absolute transformation for a special MD3 Tag if the mesh is a md3 mesh,
		// or the absolutetransformation if it's a normal scenenode
		//jaworekplay
		//to do:
		const irr::scene::SMD3QuaternionTag* CPhysXNode::getMD3TagTransformation( const irr::core::stringc & tagname)
		{
			irr::scene::SMD3QuaternionTag * tag = MD3Special.AbsoluteTagList.get ( tagname );
			if ( tag )
				return tag;
			//update removed dereferenced ptr.
			//old
			//MD3Special.AbsoluteTagList.Container.push_back ( irr::scene::SMD3QuaternionTag ( tagname, AbsoluteTransformation ) );
			//new hopefully good NOT TESTED
			MD3Special.AbsoluteTagList.push_back( irr::scene::SMD3QuaternionTag ( tagname, AbsoluteTransformation ) );
			return MD3Special.AbsoluteTagList.get ( tagname );
		}


		//! updates the absolute position based on the relative and the parents position
		void CPhysXNode::updateAbsolutePosition()
		{
			if ( 0 == Mesh || Mesh->getMeshType() != irr::scene::EAMT_MD3 )
			{
				IAnimatedMeshSceneNode::updateAbsolutePosition();
				return;
			}
			//jaworekplay
			//update changed from instance to pointer
			//and added the reference to the ptr
			irr::scene::SMD3QuaternionTag* par;
			irr::scene::SMD3QuaternionTag& parent = *par;
			if ( Parent && Parent->getType () == irr::scene::ESNT_ANIMATED_MESH)
			{
				//update
				//changed returned value from pointer to dereferenced pointer
				parent = *((irr::scene::IAnimatedMeshSceneNode*) Parent)->getMD3TagTransformation ( MD3Special.Tagname );
			}

			irr::scene::SMD3QuaternionTag relative( RelativeTranslation, RelativeRotation );

			irr::scene::SMD3QuaternionTagList *taglist;
			taglist = ( (irr::scene::IAnimatedMeshMD3*) Mesh )->getTagList ( (irr::s32)getFrameNr(),255,getStartFrame (),getEndFrame () );
			if ( taglist )
			{
				//update
				//old
				//MD3Special.AbsoluteTagList.Container.set_used ( taglist->size () );
				//new
				MD3Special.AbsoluteTagList.set_used ( taglist->size () );
				for ( irr::u32 i=0; i!= taglist->size (); ++i )
				{
					MD3Special.AbsoluteTagList[i].position = parent.position + (*taglist)[i].position + relative.position;
					MD3Special.AbsoluteTagList[i].rotation = parent.rotation * (*taglist)[i].rotation * relative.rotation;
				}
			}
		}

		//! Set the joint update mode (0-unused, 1-get joints only, 2-set joints only, 3-move and set)
		void CPhysXNode::setJointMode(irr::scene::E_JOINT_UPDATE_ON_RENDER mode)
		{
			checkJoints();

			//if (mode<0) mode=0;
			//if (mode>3) mode=3;

			JointMode=mode;
		}


		//! Sets the transition time in seconds (note: This needs to enable joints, and setJointmode maybe set to 2)
		//! you must call animateJoints(), or the mesh will not animate
		void CPhysXNode::setTransitionTime(irr::f32 time)
		{
			if (time != 0.0f)
			{
				checkJoints();
				setJointMode(irr::scene::EJUOR_CONTROL);
				TransitionTime = (irr::u32)irr::core::floor32(time*1000.0f);
			}
		}

		//! render mesh ignoring it's transformation. Used with ragdolls. (culling is unaffected)
		void CPhysXNode::setRenderFromIdentity( bool On )
		{
			RenderFromIdentity=On;
		}



		//! updates the joint positions of this mesh
		void CPhysXNode::animateJoints(bool CalculateAbsolutePositions)
		{
			checkJoints();

			if (Mesh && Mesh->getMeshType() == irr::scene::EAMT_SKINNED )
			{
				if (JointsUsed)
				{
					irr::f32 frame = getFrameNr(); //old?

					irr::scene::CSkinnedMesh* skinnedMesh=reinterpret_cast<irr::scene::CSkinnedMesh*>(Mesh);

					skinnedMesh->animateMesh(frame, 1.0f);

					skinnedMesh->recoverJointsFromMesh( JointChildSceneNodes);

					if (CalculateAbsolutePositions)
					{
						//---slow---
						for (irr::u32 n=0;n<JointChildSceneNodes.size();++n)
							if (JointChildSceneNodes[n]->getParent()==this)
							{
								JointChildSceneNodes[n]->updateAbsolutePositionOfAllChildren(); //temp, should be an option
							}
					}

					//-----------------------------------------
					//		Transition
					//-----------------------------------------

					if (Transiting != 0.f)
					{
						//Check the array is big enough (not really needed)
						if (PretransitingSave.size()<JointChildSceneNodes.size())
						{
							for(irr::u32 n=PretransitingSave.size(); n<JointChildSceneNodes.size(); ++n)
								PretransitingSave.push_back(irr::core::matrix4());
						}

						for (irr::u32 n=0; n<JointChildSceneNodes.size(); ++n)
						{
							//------Position------

							JointChildSceneNodes[n]->setPosition(
									irr::core::lerp(
										PretransitingSave[n].getTranslation(),
										JointChildSceneNodes[n]->getPosition(),
										TransitingBlend));

							//------Rotation------

							//Code is slow, needs to be fixed up

							const irr::core::quaternion RotationStart(PretransitingSave[n].getRotationDegrees()*irr::core::DEGTORAD);
							const irr::core::quaternion RotationEnd(JointChildSceneNodes[n]->getRotation()*irr::core::DEGTORAD);

							irr::core::quaternion QRotation;
							QRotation.slerp(RotationStart, RotationEnd, TransitingBlend);

							irr::core::vector3df tmpVector;
							QRotation.toEuler(tmpVector);
							tmpVector*=irr::core::RADTODEG; //convert from radians back to degrees
							JointChildSceneNodes[n]->setRotation( tmpVector );

							//------Scale------

							//JointChildSceneNodes[n]->setScale(
							//		core::lerp(
							//			PretransitingSave[n].getScale(),
							//			JointChildSceneNodes[n]->getScale(),
							//			TransitingBlend));
						}
					}
				}
			}
		}


		void CPhysXNode::checkJoints()
		{
			if (!Mesh || Mesh->getMeshType() != irr::scene::EAMT_SKINNED)
				return;

			if (!JointsUsed)
			{
				//Create joints for SkinnedMesh

				((irr::scene::CSkinnedMesh*)Mesh)->createJoints(JointChildSceneNodes, this, SceneManager);
				((irr::scene::CSkinnedMesh*)Mesh)->recoverJointsFromMesh(JointChildSceneNodes);

				JointsUsed=true;
				JointMode=irr::scene::EJUOR_READ;
			}
		}

		void CPhysXNode::beginTransition()
		{
			if (!JointsUsed)
				return;

			if (TransitionTime != 0)
			{
				//Check the array is big enough
				if (PretransitingSave.size()<JointChildSceneNodes.size())
				{
					for(irr::u32 n=PretransitingSave.size(); n<JointChildSceneNodes.size(); ++n)
						PretransitingSave.push_back(irr::core::matrix4());
				}


				//Copy the position of joints
				for (irr::u32 n=0;n<JointChildSceneNodes.size();++n)
					PretransitingSave[n]=JointChildSceneNodes[n]->getRelativeTransformation();

				Transiting = irr::core::reciprocal((irr::f32)TransitionTime);
			}
		}
		physx::PxQuat CPhysXNode::getQuatFromRotation( irr::core::vector3df rot )
		{
			float cos_z = cosf( 0.5f * rot.Z * 3.1415f / 180.f );
			float cos_y = cosf( 0.5f * rot.Y * 3.1415f / 180.f );
			float cos_x = cosf( 0.5f * rot.X * 3.1415f / 180.f );

			float sin_z = sinf( 0.5f * rot.Z * 3.1415f / 180.f );
			float sin_y = sinf( 0.5f * rot.Y * 3.1415f / 180.f );
			float sin_x = sinf( 0.5f * rot.X * 3.1415f / 180.f );

			physx::PxQuat q;
			q.w = cos_z*cos_y*cos_x + sin_z*sin_y*sin_x;
			q.x = cos_z*cos_y*sin_x + sin_z*sin_y*cos_x;
			q.y = cos_z*sin_y*cos_x + sin_z*cos_y*sin_x;
			q.z = sin_z*cos_y*cos_x + cos_z*sin_y*sin_x;
			return q;
		}
		irr::core::vector3df CPhysXNode::GetRotationFromPhysX( physx::PxQuat q )
		{
			const float Epsilon = 0.0009765625f;
			const float Threshold = 0.5f - Epsilon;
 
			float yaw;
			float pitch;
			float roll;
 
			float XY = q.x * q.y;
			float ZW = q.z * q.w;
 
			float TEST = XY + ZW;
 
			if (TEST < -Threshold || TEST > Threshold) 
			{
				int sign = TEST>0?1:-1;
				yaw = sign * 2 * (float)atan2(q.x, q.w);
				pitch = sign * 3.14159 / 2.0f;
				roll = 0;
			} else 
			{
				float XX = q.x * q.x;
				float XZ = q.x * q.z;
				float XW = q.x * q.w;
 
				float YY = q.y * q.y;
				float YW = q.y * q.w;
				float YZ = q.y * q.z;
 
				float ZZ = q.z * q.z;
 
				yaw = (float)atan2(2 * YW - 2 * XZ, 1 - 2 * YY - 2 * ZZ);
				pitch = (float)atan2(2 * XW - 2 * YZ, 1 - 2 * XX - 2 * ZZ);
				roll = (float)asin(2 * TEST);
			}
 
			return irr::core::vector3df(pitch* 180.0f / 3.1415f, 
				yaw* 180.0f / 3.1415f, 
				roll* 180.0f / 3.1415f) + 
				irr::core::vector3df(360, 360, 360);
		}
	}//end of scene namespace
}//end of custom namespace