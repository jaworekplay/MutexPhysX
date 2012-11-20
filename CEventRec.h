#pragma once
#include <irrlicht.h>

class CEvRec : public irr::IEventReceiver
{
public:
	virtual bool OnEvent( const irr::SEvent& event )
	{
		switch( event.EventType )
		{
		case irr::EET_KEY_INPUT_EVENT:
			Keyboard.KeyCodes[ event.KeyInput.Key ] = event.KeyInput.PressedDown;
		case irr::EET_MOUSE_INPUT_EVENT:
			printf("Key pressed.\n");
			if( event.MouseInput.isLeftPressed() )
				Mouse.MouseButtons[0] = true;
			else if( event.MouseInput.isRightPressed() )
				Mouse.MouseButtons[1] = true;
			else if( event.MouseInput.isMiddlePressed() )
				Mouse.MouseButtons[2] = true;
		default:
			Mouse.reset();
			Keyboard.reset();
		}
		return false;
	}
	//keyboard input
	class CKeyboard
	{
	public:
		bool IsKeyDown( irr::EKEY_CODE key )
		{
			return KeyCodes[key];
		}
		void reset()
		{
			for( int i = 0; i < irr::KEY_KEY_CODES_COUNT; ++i )
				KeyCodes[i] = false;
		}
		bool KeyCodes[ irr::KEY_KEY_CODES_COUNT ];
	}Keyboard;
	//mouse input
	class CMouse
	{
	public:
		bool isLeftClicked( irr::EMOUSE_INPUT_EVENT click )
		{
			return MouseButtons[0];
		}
		bool isRightClicked( irr::EMOUSE_INPUT_EVENT click )
		{
			return MouseButtons[1];
		}
		bool isMiddleClicked( irr::EMOUSE_INPUT_EVENT click )
		{
			return MouseButtons[2];
		}
		bool MouseButtons[3];
		void reset()
		{
			for( int i = 0; i < 3; ++i )
				MouseButtons[i] = false;
		}
	}Mouse;
	//GUI input
	class GUI
	{
	public:

	}gui;
	union
	{
		CKeyboard Keyboard;
		CMouse Mouse;
	};
};