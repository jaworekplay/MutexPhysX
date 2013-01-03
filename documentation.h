/*

*****************************************************************************
*****************************************************************************
**	MUTEX PROJECT O.M.A GAME ENGINE DOCUMENTATION:
*****************************************************************************
**	PLEASE USE // <YOURNAME> TO REFER TO YOUR COMMENTS
**	PLEASE ADD AND DO NOT DELETE, THANK YOU.

**	HOPEFULLY BY LOOKING AT THIS WE CAN MORE CLEARLY SEE DEVELOPMENT PROGRESS
*****************************************************************************


*****************************************************************************
**	FOREWORDS AND FUTURE PLANS	
11/11/12 -	We need to have working nodes for Cylinders (so we can get working bullet-casing)																	// Leo
			I would also like to get a GUI set up that allows us to Spawn physX objects at run-time with parameters such as Density and Weight etc.				// Leo
			Lukasz, can you write about how you have implemented the code for PhysX and create a "Cheat sheet" of all interfaces and at least a few methods.	// Leo
*****************************************************************************


*****************************************************************************
**	REVISIONS
V0.001	 - 	*Corrected the irrlicht Sphere node to match the physical size of the PhysX Sphere node.															// Leo
*****************************************************************************			

jaworekplay UPDATE 15/11/2012
**Added some comments to the code																																//Lukasz
**Implementing cylinders																																		//Lukasz
**
V0.001	-	Implemented the XML reader to use it for mesh creation from the APEX mesh.																			//Lukasz
- Attempt to create a convex shape in the physx - It won't be visible in the Irrlicht but it will in the PVD.													//Lukasz
- Our application needs to be rearranged to allow us to use the XML reader, I will do this only if you guys are OK with it. More info on the GitHub.			//Lukasz
- Added some base for the audio.																																//Lukasz
- Added MeshImport for the APEX meshes and some functionality to use with Irrlicht																				//Lukasz
*/