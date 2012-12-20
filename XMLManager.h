#include <irrlicht.h>
#include <driverChoice.h>

using namespace irr;
using namespace core;
class CXMLManager
{
private:
	CXMLManager(const CXMLManager& other);
	CXMLManager& operator=(const CXMLManager& other);
	core::map< core::stringw, core::stringw> managerSettings;
	core::stringw settingFile;
	irr::IrrlichtDevice* device;
public:
	core::map< core::stringw, s32 > driverOptions;//driver for the engine
	core::map< core::stringw, core::dimension2du> resolution;
public:
	//Constructor
	CXMLManager(const core::stringw& setting_file): settingFile(setting_file),device(0)
	{
		device = createDevice(irr::video::EDT_NULL);
		driverOptions.insert(L"Software",	irr::video::EDT_SOFTWARE);
		driverOptions.insert(L"OpenGL",		irr::video::EDT_OPENGL);
		driverOptions.insert(L"DirectX9",	irr::video::EDT_DIRECT3D9);

		resolution.insert(L"1024x768",		irr::core::dimension2du(1024,768) );
		resolution.insert(L"1280x1024",		irr::core::dimension2du(1280,1024) );

		//setting default settings fo the manager
		managerSettings.insert(L"Driver",	L"OpenGL");
		managerSettings.insert(L"Resolution", L"1024x768");
		managerSettings.insert(L"Fullscreen", L"0");
	}
	//Destructor
	~CXMLManager()
	{
		if( device )
		{
			device->closeDevice();
			device->drop();
		}
	}
	virtual bool load()
	{
		if( !device )
			return false;
		irr::io::IXMLReader* xml = device->getFileSystem()->createXMLReader( settingFile );
		if( !xml )
			return false;
		const core::stringw settingTag(L"setting"); //tag responsible for settings in the xml file
		stringw currentSection;
		const stringw videoTag(L"Video");

		while( xml->read() )
		{
			switch( xml->getNodeType() )
			{
			case io::EXN_ELEMENT:
				if( currentSection.size() == 0 && videoTag.equals_ignore_case( xml->getNodeName() ) )
					currentSection = videoTag;
				else if( currentSection.equals_ignore_case( videoTag ) && settingTag.equals_ignore_case( xml->getNodeName() ) )
				{
					stringw key = xml->getAttributeValueSafe(L"name");
					if( key.size() == 0 )
						managerSettings[key] = xml->getAttributeValueSafe(L"value");
				}
				//to do:
				//add other nodes typ names and try to get APEX file to work;
				//read more about the xml handling on irrlicht
				break;
			case io::EXN_ELEMENT_END:
				currentSection = L"";
				break;
			}//end of switch
		}//end of while
		xml->drop();
		return true;
	}//end of bool load()
};//end of class