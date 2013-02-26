#pragma once
#include "ICustomEventReceiver.h"
#include <stdio.h>

ICustomEventReceiver::ICustomEventReceiver()
{
	//keyboard keys events
	for( irr::u32 i = 0; i < NUMBER_OF_KEYS; ++i )
		m_EKeys[i] = Button_UP;
	//m_EMouse buttons events
	for( irr::u32 j = 0; j < NUMBER_OF_MOUSE_BUTTONS; ++j )
		m_EMouse[j] = MButton_UP;

	m_EMouseData.x = m_nDeltaMouseY = 0;

	m_bMouseHasMoved = false;
	m_s32GeneralCallerID = m_s32MenuItemSelectedID = 0;
	m_pGUIMenu = NULL;

	m_EGUIElementStatus[ irr::gui::EGET_ELEMENT_FOCUS_LOST ] = m_EGUIElementStatus[ irr::gui::EGET_ELEMENT_FOCUSED ];
	m_EGUIElementStatus[ irr::gui::EGET_ELEMENT_HOVERED ] = m_EGUIElementStatus[ irr::gui::EGET_ELEMENT_LEFT ];
	m_EGUIElementStatus[ irr::gui::EGET_ELEMENT_CLOSED ] = m_EGUIElementStatus[ irr::gui::EGET_BUTTON_CLICKED ];
	m_EGUIElementStatus[ irr::gui::EGET_SCROLL_BAR_CHANGED ] = m_EGUIElementStatus[ irr::gui::EGET_CHECKBOX_CHANGED ];
	m_EGUIElementStatus[ irr::gui::EGET_LISTBOX_CHANGED ] = m_EGUIElementStatus[ irr::gui::EGET_LISTBOX_SELECTED_AGAIN ];
	m_EGUIElementStatus[ irr::gui::EGET_FILE_SELECTED ] = m_EGUIElementStatus[ irr::gui::EGET_FILE_CHOOSE_DIALOG_CANCELLED ];
	m_EGUIElementStatus[ irr::gui::EGET_MESSAGEBOX_YES ] = m_EGUIElementStatus[ irr::gui::EGET_MESSAGEBOX_NO ];
	m_EGUIElementStatus[ irr::gui::EGET_MESSAGEBOX_OK ] = m_EGUIElementStatus[ irr::gui::EGET_MESSAGEBOX_CANCEL ];
	m_EGUIElementStatus[ irr::gui::EGET_EDITBOX_ENTER ] = m_EGUIElementStatus[ irr::gui::EGET_TAB_CHANGED];
	m_EGUIElementStatus[ irr::gui::EGET_MENU_ITEM_SELECTED ] = m_EGUIElementStatus[ irr::gui::EGET_COMBO_BOX_CHANGED ];
	m_EGUIElementStatus[ irr::gui::EGET_SPINBOX_CHANGED ] = GUI_FALSE; // set all gui elemenets to false
}
ICustomEventReceiver::~ICustomEventReceiver(){}

bool ICustomEventReceiver::isKeyDown( irr::EKEY_CODE key )
{
	if( m_EKeys[key] == Button_DOWN )
		return true;
	return false;
}
bool ICustomEventReceiver::isKeyUp( irr::EKEY_CODE key )
{
	if( m_EKeys[key] == Button_UP )
		return true;
	return false;
}

bool ICustomEventReceiver::isKeyPressed( irr::EKEY_CODE key )
{
	if( m_EKeys[key] == Button_PRESSED )
		return true;
	return false;
}

bool ICustomEventReceiver::isKeyReleased( irr::EKEY_CODE key )
{
	if( m_EKeys[key] == Button_RELEASED )
		return true;
	return false;
}

bool ICustomEventReceiver::isMouseButtonUp( EMouseButton mb )
{
	if( m_EMouse[mb] == MButton_UP )
		return true;
	return false;
}

bool ICustomEventReceiver::isMouseButtonDown( EMouseButton mb )
{
	if( m_EMouse[mb] == MButton_DOWN )
		return true;
	return false;
}

bool ICustomEventReceiver::isMouseButtonPressed( EMouseButton mb )
{
	if( m_EMouse[mb] == MButton_PRESSED )
		return true;
	return false;
}

bool ICustomEventReceiver::isMouseButtonReleased( EMouseButton mb )
{
	if( m_EMouse[mb] == MButton_RELEASED )
		return true;
	return false;
}

int ICustomEventReceiver::getDeltaMousePosX() { return m_nDeltaMouseX; }
int ICustomEventReceiver::getDeltaMousePosY() { return m_nDeltaMouseY; }

bool ICustomEventReceiver::getEventCallerByElement( irr::gui::EGUI_EVENT_TYPE ev)
{
	if( m_EGUIElementStatus[ ev ] == GUI_TRUE )
		return true;
	return false;
}

// This function will be used to get the ID of a gui element.
// This is a general function for getting the ID.
// It works for a lot of gui events but not all.
// Like getting the ID of the context menu wont work with this function
// Instead, use this function: getEventCallerOfMenuByID()
int ICustomEventReceiver::getEventCallerByID()
{
	return m_s32GeneralCallerID;
}

// meant for event: EGET_MENU_ITEM_SELECTED
// because IGUIContextMenu does not have the function: getID()
// in this line: event.GUIEvent.Caller->getID()
// So I had to make a custome one for the EGET_MENU_ITEM_SELECTED events.
int ICustomEventReceiver::getEventCallerOfMenuByID()
{
	return m_s32MenuItemSelectedID;
}

// This is used so that the Key States will not be changed during execution of your Main game loop.
// Place this function at the END of your Main Loop.
void ICustomEventReceiver::startEventProcess()
{
	int i = 0;
	m_EProcessState = Event_STARTED;

	//keyb key states
	for( i; i < irr::KEY_KEY_CODES_COUNT; ++i )
	{
		if( m_EKeys[i] == Button_RELEASED )
			m_EKeys[i] = Button_UP;
		if( m_EKeys[i] == Button_PRESSED )
			m_EKeys[i] = Button_DOWN;
	}
	//m_EMouse kbutton states
	for( i = 0; i < NUMBER_OF_MOUSE_BUTTONS; ++i )
	{
		if( m_EMouse[i] == MButton_RELEASED )
			m_EMouse[i] = MButton_DOWN;
		if( m_EMouse[i] == MButton_RELEASED )
			m_EMouse[i] = MButton_DOWN;
	}
	m_EGUIElementStatus[ irr::gui::EGET_ELEMENT_FOCUS_LOST ] = m_EGUIElementStatus[ irr::gui::EGET_ELEMENT_FOCUSED ];
	m_EGUIElementStatus[ irr::gui::EGET_ELEMENT_HOVERED ] = m_EGUIElementStatus[ irr::gui::EGET_ELEMENT_LEFT ];
	m_EGUIElementStatus[ irr::gui::EGET_ELEMENT_CLOSED ] = m_EGUIElementStatus[ irr::gui::EGET_BUTTON_CLICKED ];
	m_EGUIElementStatus[ irr::gui::EGET_SCROLL_BAR_CHANGED ] = m_EGUIElementStatus[ irr::gui::EGET_CHECKBOX_CHANGED ];
	m_EGUIElementStatus[ irr::gui::EGET_LISTBOX_CHANGED ] = m_EGUIElementStatus[ irr::gui::EGET_LISTBOX_SELECTED_AGAIN ];
	m_EGUIElementStatus[ irr::gui::EGET_FILE_SELECTED ] = m_EGUIElementStatus[ irr::gui::EGET_FILE_CHOOSE_DIALOG_CANCELLED ];
	m_EGUIElementStatus[ irr::gui::EGET_MESSAGEBOX_YES ] = m_EGUIElementStatus[ irr::gui::EGET_MESSAGEBOX_NO ];
	m_EGUIElementStatus[ irr::gui::EGET_MESSAGEBOX_OK ] = m_EGUIElementStatus[ irr::gui::EGET_MESSAGEBOX_CANCEL ];
	m_EGUIElementStatus[ irr::gui::EGET_EDITBOX_ENTER ] = m_EGUIElementStatus[ irr::gui::EGET_TAB_CHANGED];
	m_EGUIElementStatus[ irr::gui::EGET_MENU_ITEM_SELECTED ] = m_EGUIElementStatus[ irr::gui::EGET_COMBO_BOX_CHANGED ];
	m_EGUIElementStatus[ irr::gui::EGET_SPINBOX_CHANGED ] = GUI_FALSE; // set all gui elemenets to false
}

// This is used so that the Key States will not be changed during execution of your Main game loop.
// Place this at the very START of your Main Loop
void ICustomEventReceiver::endEventProcess()
{
	m_EProcessState = Event_ENDED;
}

bool ICustomEventReceiver::OnEvent( const irr::SEvent& event )
{
	switch( event.EventType )
	{
	case irr::EET_KEY_INPUT_EVENT:
		{
			if( m_EProcessState == Event_STARTED )
			{
				if( event.KeyInput.PressedDown )
				{
					//if Key was not down before
					if( m_EKeys[ event.KeyInput.Key] != Button_DOWN )
						m_EKeys[event.KeyInput.Key] = Button_DOWN; //set the key to pressed value
					else
						m_EKeys[event.KeyInput.Key] = Button_PRESSED;
					//printf("Key pressed\n");
				break;
				}
				else
				{
					//if the key is down
					if( m_EKeys[event.KeyInput.Key] != Button_UP )
						m_EKeys[event.KeyInput.Key] = Button_UP; //set the key to released value
					else
						m_EKeys[event.KeyInput.Key] = Button_RELEASED;
					//printf("Key released\n");
					break;
				}
			}
		}
		break;
	case irr::EET_MOUSE_INPUT_EVENT:
		{
			switch( event.MouseInput.Event )
			{
				case irr::EMIE_MOUSE_MOVED:
				{
					m_nDeltaMouseX = event.MouseInput.X - m_EMouseData.lx;
					m_nDeltaMouseY = event.MouseInput.Y - m_EMouseData.ly;

					m_EMouseData.x = event.MouseInput.X;
					m_EMouseData.y = event.MouseInput.Y;

					m_EMouseData.lx = m_EMouseData.x;
					m_EMouseData.ly = m_EMouseData.y;

					m_bMouseHasMoved = true;
					break;
				}
				case irr::EMIE_MOUSE_WHEEL:
				{
					m_EMouseData.lWheelPos = m_EMouseData.wheelPos;
					m_EMouseData.wheelPos += event.MouseInput.Wheel;
					break;
				}
				case irr::EMIE_LMOUSE_PRESSED_DOWN:
				{
					if(m_EMouse[MButton_LEFT] == MButton_UP || m_EMouse[MButton_LEFT] == MButton_RELEASED)
						m_EMouse[MButton_LEFT] = MButton_PRESSED;
					else
						m_EMouse[MButton_LEFT] = MButton_DOWN;
					break;
				}

				// Left m_EMouse Button Released
				case irr::EMIE_LMOUSE_LEFT_UP:
				{
					if(m_EMouse[MButton_LEFT] != MButton_UP)
						m_EMouse[MButton_LEFT] = MButton_RELEASED;
					break;
				}

				// Middle m_EMouse Button Pressed
				case irr::EMIE_MMOUSE_PRESSED_DOWN:
				{
					if(m_EMouse[MButton_MIDDLE] == MButton_UP || m_EMouse[MButton_MIDDLE] == MButton_RELEASED)
						m_EMouse[MButton_MIDDLE] = MButton_PRESSED;
					else
						m_EMouse[MButton_MIDDLE] = MButton_DOWN;
					break;
				}

				// Middle m_EMouse Button Released
				case irr::EMIE_MMOUSE_LEFT_UP:
				{
					if (m_EMouse[MButton_MIDDLE] != MButton_UP)
						m_EMouse[MButton_MIDDLE] = MButton_RELEASED;
					break;
				}

				// Right m_EMouse Button Pressed
				case irr::EMIE_RMOUSE_PRESSED_DOWN:
				{
					if (m_EMouse[MButton_RIGHT] == MButton_UP || m_EMouse[MButton_RIGHT] == MButton_RELEASED)
						m_EMouse[MButton_RIGHT] = MButton_PRESSED;

					else
						m_EMouse[MButton_RIGHT] = MButton_DOWN;
					break;
				}

				// Right m_EMouse Button Released
				case irr::EMIE_RMOUSE_LEFT_UP:
				{
					if(m_EMouse[MButton_RIGHT] != MButton_UP)
						m_EMouse[MButton_RIGHT] = MButton_RELEASED;
					break;
				}
			}
		}
		/*case irr::EET_GUI_EVENT:
		{
			m_s32GeneralCallerID = event.GUIEvent.Caller->getID();
			switch( event.GUIEvent.EventType )
			{
			case irr::gui::EGET_ELEMENT_FOCUS_LOST:
			case irr::gui::EGET_ELEMENT_FOCUSED:
			case irr::gui::EGET_ELEMENT_HOVERED:
			case irr::gui::EGET_ELEMENT_LEFT:
			case irr::gui::EGET_ELEMENT_CLOSED:
			case irr::gui::EGET_BUTTON_CLICKED:
			case irr::gui::EGET_SCROLL_BAR_CHANGED:
			case irr::gui::EGET_CHECKBOX_CHANGED:
			case irr::gui::EGET_LISTBOX_CHANGED:
			case irr::gui::EGET_LISTBOX_SELECTED_AGAIN:
			case irr::gui::EGET_FILE_SELECTED:
			case irr::gui::EGET_FILE_CHOOSE_DIALOG_CANCELLED:
			case irr::gui::EGET_MESSAGEBOX_YES:
			case irr::gui::EGET_MESSAGEBOX_NO:
			case irr::gui::EGET_MESSAGEBOX_OK:
			case irr::gui::EGET_MESSAGEBOX_CANCEL:
			case irr::gui::EGET_EDITBOX_ENTER:
			case irr::gui::EGET_TAB_CHANGED:
			case irr::gui::EGET_COMBO_BOX_CHANGED:
			case irr::gui::EGET_SPINBOX_CHANGED:
				m_EGUIElementStatus[ event.GUIEvent.EventType ] = GUI_TRUE;
			break;
			case irr::gui::EGET_MENU_ITEM_SELECTED:
				{
					m_EGUIElementStatus[ irr::gui::EGET_MENU_ITEM_SELECTED ] = GUI_TRUE;
					m_pGUIMenu = (irr::gui::IGUIContextMenu*)event.GUIEvent.Caller;
					m_s32MenuItemSelectedID = m_pGUIMenu->getItemCommandId( m_pGUIMenu->getSelectedItem() );
			break;
				}
			}*/
		default:
		break;
	}
	return false;
}