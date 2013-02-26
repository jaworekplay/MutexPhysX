#pragma once
#include "MutexPhysX.h"

class CVehicle : public CMutex
{
private:
	physx::PxVehicleWheelsSimData* m_wheelSimData;
	physx::PxVehicleDriveSimData4W m_driveData;
	physx::PxVehicleChassisData m_chassisData;
public:
	CVehicle()
	{
		//required to initiate the vehicle SDK - yes vehicles have their own SDK -_-
		PxInitVehicleSDK( *CMutex::mPhysX );
	}
	~CVehicle() {PxCloseVehicleSDK();}

	virtual void createVehicle4WSimulationData( 
		const PxF32 chassisMass,
		physx::PxConvexMesh* chassisConvexMesh,
		const PxF32 wheelMass,
		physx::PxConvexMesh** wheelConvexMeshes,
		const PxVec3* wheelCentreOffsets,
		physx::PxVehicleWheelsSimData& wheelsData,
		physx::PxVehicleDriveSimData4W& driveData,
		physx::PxVehicleChassisData& chassisData )
	{
		//extract the chassis AABB dimensions from the chassis convex mesh
		const PxVec3 chassisDims;
	}
};