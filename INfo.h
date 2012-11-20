// 
// 
////PxCapsuleControllerDesc ControllerDesc <- You have to declare some more information for it
//ControllerDesc.radius           =  radius; // Set your radius here
//    ControllerDesc.height           = height; // Set your height here
//    ControllerDesc.upDirection  = PxVec3(0,1,0); // This is our upDirection - 0,1,0 means upwoards the Y axis
//    ControllerDesc.slopeLimit       = 0.5f; //slopeLimit - for disable climbing walls
//    ControllerDesc.contactOffset        = skinWidth;    // Capsule is flying above ground - set its skinWidth for tuning for ex. collision issuses on stairs
//    ControllerDesc.stepOffset = stepOffset; // Almost same effect as above
//    ControllerDesc.position = PxExtendedVec3(position.x,position.y,position.z); // Position of the capsule
//    // ControllerDesc.behaviorCallback = // you dont need it
//    ControllerDesc.scaleCoeff=1; // dont modify it
//    ControllerDesc.interactionMode = PxCCTInteractionMode::eINCLUDE; // Interact with other controllers ( player capsul )
//    ControllerDesc.climbingMode = PxCapsuleClimbingMode::eEASY; // set the climbing mode to easy - its better for the start
//    ControllerDesc.material = mPhysic->mCollisionMaterial.mDefault; // You should set a material for the capsule - eg. friction ...
//        ControllerDesc.userData = new CPhysicActorUserData(this,mType); // I use a void* pointer for storing all kind of stuff that I need
//    
//// PxCapsuleController*mController <- save this in your own object class ( eg. player class )
// // PxRigidDynamic* mActor <- this too
// 
//        mController = (PxCapsuleController *)mPhysic->getManager()->createController(*mGameManager->mPhysic->getSDK(),mPhysic->getScene(), ControllerDesc);
//// Here Im casting to a PxCapsuleController and create it. You know that mPhysic is my own Physics class - same as mGameManager.
//// You cant just copy and paste the whole code. You have to change many lines if you want to that this will work. ( Even compile ! )
// 
//        mActor = mController->getActor(); // Here we are going to get our Actor from the Controller - its important for me because I will do something with it.
//        mController->setPosition(PxExtendedVec3(mPosition.x,mPosition.y,mPosition.z)); // Set the capsule position ( in the world )
// 
//    //mActor->userData = new CPhysicActorUserData(this,mType); You dont need it
//        //mPhysic->addUserDataToShapes(mActor,true);
// 
//// std::list<CObject*> mObjects; <- Add this to your gamemanager ( or where ever you want to update all objects )
// 
//        mGameManager->mObjectManager->mObjects.push_back(this); // This is a very very important step - add the actor to the list which contains all objects for
//// updating positions / rotations. change >>this<< to your own base class object.
// 
//// This is the function how you can update the list with the elements ( player capsule ... boxes ... everything ) - But you have to learn OO programming - so that function wont work for you
//// because you dont use the CObject class ( which is my base class for all entitys , items, objects ... ) 
// 
//bool CObjectManager::update()
//    {
//        for(std::list<CObject*>::iterator Iterator = mObjects.begin(); Iterator != mObjects.end();) 
//        {
//        CObject* obj =  Iterator.operator *(); // You have to change CObject everywhere you find it with your own baseclass ..
// 
//        obj->update(); // update the object.
// 
//        if (obj->isDeleteable()) 
//            {   
//            std::list<CObject*>::iterator it  = Iterator;
//            Iterator = mObjects.erase(it);  
//            mObjects.remove(obj);
//            obj = NULL; // DELETE THE OBJECT 
//            continue;
//            }
//        ++Iterator;
//        }
//    return true;
//    };
// 
////Here is some code from CObject::update() <- that function calls updatePosition()
// 
//bool CObject::updatePositions()
//    {
//  //  if (mUseInternalUpdate) // You dont need this line
//        {
//        if (mSceneNode && mActor) // Check if variables are valid
//            {
//            PxTransform pos = mActor->getGlobalPose(); // get the Position
//            mSceneNode->setPosition(CConvert::toOgre(pos.p)); // Set the position ( Converted to Ogre format )
//            mSceneNode->setOrientation(CConvert::toOgre(pos.q)); // Set Orientation ( rotation ) - converted to Ogre format 
//            return true;
//            }
//        }
//    return false;     
//    };
// 
// 
// 