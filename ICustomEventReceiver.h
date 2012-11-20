#ifndef _IEVENTRECEIVER_H_
#define _IEVENTRECEIVER_H_
#include <irrlicht.h>
#include <iostream>
#include <sstream>

//for security number of keys in the keyboard and mouse has to be predefined
#define NUMBER_OF_KEYS irr::KEY_KEY_CODES_COUNT
#define NUMBER_OF_MOUSE_BUTTONS 3
#define NUMBER_OF_GUI_ELEMENTS 21

enum EKeyboardButtonState
{
	Button_UP,
	Button_DOWN,
	Button_PRESSED,
	Button_RELEASED
};

enum EMouseButton
{
	MButton_LEFT,
	MButton_RIGHT,
	MButton_MIDDLE
};
enum EMouseButtonState
{
	MButton_UP,
	MButton_DOWN,
	MButton_PRESSED,
	MButton_RELEASED
};

struct EMouseInfo
{
	irr::s32 x, 
		y, 
		lx, 
		ly, 
		cx, 
		cy;

	irr::f32 wheelPos, 
		lWheelPos;
};
//set of variables for event handling process
enum EProcessEventState
{
	Event_STARTED,
	Event_ENDED
};
//GUI events accept or not
enum EGUIElementsStatus
{
	GUI_TRUE,
	GUI_FALSE
};
class ICustomEventReceiver : public irr::IEventReceiver
{
public:
	//Constructor & Destructor
	ICustomEventReceiver();
	//Maybe later will make this virtual ?? hmmmm...
	~ICustomEventReceiver();
	//Member Functions
	bool getEventCallerByElement( irr::gui::EGUI_EVENT_TYPE );
	int getEventCallerOfMenuByID();
	int getEventCallerByID();

	//keyboard events functions
	bool isKeyUp( irr::EKEY_CODE key );
	bool isKeyDown( irr::EKEY_CODE key );
	bool isKeyPressed( irr::EKEY_CODE key );
	bool isKeyReleased( irr::EKEY_CODE key );

	//Mouse Event Functions
	bool isMouseButtonUp( EMouseButton mb );
	bool isMouseButtonDown( EMouseButton mb );
	bool isMouseButtonPressed( EMouseButton mb );
	bool isMouseButtonReleased( EMouseButton mb );

	//Processing Functions
	void startEventProcess();
	void endEventProcess();

	int getDeltaMousePosX();
	int getDeltaMousePosY();
	virtual bool OnEvent( const irr::SEvent& event );
protected:
	EKeyboardButtonState m_EKeys[NUMBER_OF_KEYS];
	EMouseButtonState	m_EMouse[NUMBER_OF_MOUSE_BUTTONS];
	EGUIElementsStatus	m_EGUIElementStatus[NUMBER_OF_GUI_ELEMENTS];
	EMouseInfo			m_EMouseData;
	EProcessEventState	m_EProcessState;
	irr::IrrlichtDevice* device;

	irr::gui::IGUIContextMenu* m_pGUIMenu;
	irr::s32 m_s32MenuItemSelectedID;
	irr::s32 m_s32GeneralCallerID;

	int m_nDeltaMouseX,
		m_nDeltaMouseY;
	bool m_bMouseHasMoved;

	//template for converting everything to string even string string
	template <class T>
	inline std::string ToString( const T& t )
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}
};
#endif