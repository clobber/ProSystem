// ----------------------------------------------------------------------------
//   ___  ___  ___  ___       ___  ____  ___  _  _
//  /__/ /__/ /  / /__  /__/ /__    /   /_   / |/ /
// /    / \  /__/ ___/ ___/ ___/   /   /__  /    /  emulator
//
// ----------------------------------------------------------------------------
// Copyright 2005 Greg Stanton
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
// ----------------------------------------------------------------------------
// Input.cpp
// ----------------------------------------------------------------------------
#include <dinput.h> 
#include "Input.h"
#include "Console.h"
#define INPUT_SOURCE "Input.cpp"
#define INPUT_KEY_SIZE 100
#define INPUT_JOY_SIZE 20

byte input_keys[17] = {DIK_RIGHT, DIK_LEFT, DIK_DOWN, DIK_UP, DIK_X, DIK_Z, DIK_L, DIK_J, DIK_K, DIK_I, DIK_M, DIK_N, DIK_F2, DIK_F3, DIK_F4, DIK_F5, DIK_F6};
byte input_values[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
byte input_devices[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static LPDIRECTINPUT input_dinput = NULL;
static LPDIRECTINPUTDEVICE2 input_joystick[128] = {NULL,NULL};
static LPDIRECTINPUTDEVICE input_keyboard = NULL;
extern const std::string input_keyTitles[ ];
extern const byte input_keyValues[ ];
extern const std::string input_joyTitles[ ];
extern const byte input_joyValues[ ];
HINSTANCE input_hInstance;
static int current_device;

BOOL CALLBACK InitJoysticksCallback(const DIDEVICEINSTANCE*     
                                       pdidInstance, VOID* pContext);

// ----------------------------------------------------------------------------
// InitializeKeys
// ----------------------------------------------------------------------------
static void input_InitializeKeys(HWND hCombo, int value) {
  if ( !value ) {
    for(int index = 0; index < INPUT_KEY_SIZE; index++) {
      SendMessage(hCombo, CB_INSERTSTRING, index, (LPARAM)input_keyTitles[index].c_str( ));
      SendMessage(hCombo, CB_SETITEMDATA, index, input_keyValues[index]);
    }
  }
  else {
    for(int index = 0; index < INPUT_JOY_SIZE; index++) {
      SendMessage(hCombo, CB_INSERTSTRING, index, (LPARAM)input_joyTitles[index].c_str( ));
      SendMessage(hCombo, CB_SETITEMDATA, index, input_joyValues[index]);
    }
  }
}

// ----------------------------------------------------------------------------
// SelectKey
// ----------------------------------------------------------------------------
static void input_SelectKey(HWND hCombo, int value) {
  for(int index = 0; index < INPUT_KEY_SIZE; index++) {
    if(SendMessage(hCombo, CB_GETITEMDATA, index, 0) == value) {
      SendMessage(hCombo, CB_SETCURSEL, index, 0);
      return;
    }
  }
}

// ----------------------------------------------------------------------------
// AddDevice
// ----------------------------------------------------------------------------
static void input_AddDevice(HWND hCombo, char *string, unsigned int value) {

  int index;

  index = SendMessage(hCombo, CB_INSERTSTRING, -1, (LPARAM)string);
  SendMessage(hCombo, CB_SETITEMDATA, index, value);
}

// ----------------------------------------------------------------------------
// SelectDevice
// ----------------------------------------------------------------------------
static void input_SelectDevice(HWND hCombo, int value) {

  int count = SendMessage(hCombo, CB_GETCOUNT, 0, 0);

  for(int index = 0; index < count; index++) {
    if(SendMessage(hCombo, CB_GETITEMDATA, index, 0) == value) {
      SendMessage(hCombo, CB_SETCURSEL, index, 0);
      return;
    }
  }
}

// ----------------------------------------------------------------------------
// HasKeyValue
// ----------------------------------------------------------------------------
static bool input_HasKeyValue(int value) {
  for(int index = 0; index < 17; index++) {
    if(input_keys[index] == value) {
      return true;
    }
  }
  return false;
}

// ----------------------------------------------------------------------------
// InitJoysticksListControl
// ----------------------------------------------------------------------------
BOOL CALLBACK InitJoysticksListControl(const DIDEVICEINSTANCE*     
                                       pdidInstance, VOID* pContext) {

	// Add to Our Device List
	input_AddDevice (GetDlgItem((HWND)pContext, IDC_COMBO_DEVICE_UP), 
                       (char *)pdidInstance->tszInstanceName, ++current_device );
	input_AddDevice (GetDlgItem((HWND)pContext, IDC_COMBO_DEVICE_DOWN), 
                       (char *)pdidInstance->tszInstanceName, current_device );
	input_AddDevice (GetDlgItem((HWND)pContext, IDC_COMBO_DEVICE_LEFT), 
                       (char *)pdidInstance->tszInstanceName, current_device );
	input_AddDevice (GetDlgItem((HWND)pContext, IDC_COMBO_DEVICE_RIGHT), 
                       (char *)pdidInstance->tszInstanceName, current_device );
	input_AddDevice (GetDlgItem((HWND)pContext, IDC_COMBO_DEVICE_BUTTON1), 
                       (char *)pdidInstance->tszInstanceName, current_device );
	input_AddDevice (GetDlgItem((HWND)pContext, IDC_COMBO_DEVICE_BUTTON2), 
                       (char *)pdidInstance->tszInstanceName, current_device );

    return DIENUM_CONTINUE;
}

// ----------------------------------------------------------------------------
// InitJoysticksListConsole
// ----------------------------------------------------------------------------
BOOL CALLBACK InitJoysticksListConsole(const DIDEVICEINSTANCE*     
                                       pdidInstance, VOID* pContext) {
	// Add to Our Device List
	input_AddDevice (GetDlgItem((HWND)pContext, IDC_COMBO_DEVICE_RESET), 
                       (char *)pdidInstance->tszInstanceName, ++current_device );
	input_AddDevice (GetDlgItem((HWND)pContext, IDC_COMBO_DEVICE_SELECT), 
                       (char *)pdidInstance->tszInstanceName, current_device );
	input_AddDevice (GetDlgItem((HWND)pContext, IDC_COMBO_DEVICE_PAUSE), 
                       (char *)pdidInstance->tszInstanceName, current_device );
	input_AddDevice (GetDlgItem((HWND)pContext, IDC_COMBO_DEVICE_LEFTD), 
                       (char *)pdidInstance->tszInstanceName, current_device );
	input_AddDevice (GetDlgItem((HWND)pContext, IDC_COMBO_DEVICE_RIGHTD), 
                       (char *)pdidInstance->tszInstanceName, current_device );

    return DIENUM_CONTINUE;
}

// ----------------------------------------------------------------------------
// InitializeControllerDialog
// ----------------------------------------------------------------------------
static input_InitializeControllerDialog(HWND hDialog, byte controller) {
  std::string title = "Controller " + common_Format(controller);
  HWND hCombo;
  int i;
  int value,index;
  int id_key[] = {3,2,1,0,4,5};
  int id[] = {IDC_COMBO_CONTROLLER_UP,IDC_COMBO_CONTROLLER_DOWN,
              IDC_COMBO_CONTROLLER_LEFT,IDC_COMBO_CONTROLLER_RIGHT,
              IDC_COMBO_CONTROLLER_BUTTON1,IDC_COMBO_CONTROLLER_BUTTON2};
  int id_device[] = {IDC_COMBO_DEVICE_UP,IDC_COMBO_DEVICE_DOWN,
                     IDC_COMBO_DEVICE_LEFT,IDC_COMBO_DEVICE_RIGHT,
                     IDC_COMBO_DEVICE_BUTTON1,IDC_COMBO_DEVICE_BUTTON2};

  SetWindowText(hDialog, title.c_str( ));

  controller--;

  // Add Keyboard to Device List
  for ( i = 0; i < 6; ++i )
    input_AddDevice(GetDlgItem(hDialog, id_device[i]), "Keyboard", 0);
 
  // First find joysticks, if any, callback adds to list
  current_device = 0;
  if ( input_dinput != NULL )
	  input_dinput->EnumDevices(DIDEVTYPE_JOYSTICK, InitJoysticksListControl,
	                            hDialog, DIEDFL_ATTACHEDONLY);

  // Select Keyboard or Joystick Device
  for ( i = 0; i < 6; ++i ) {
  	index = SendMessage(GetDlgItem(hDialog, id_device[i]), CB_SETCURSEL, (LPARAM)input_devices[id_key[i] + (6 * controller)], 0);
  	if(index != CB_ERR) {
    	value = SendMessage(GetDlgItem(hDialog, id_device[i]), CB_GETITEMDATA, index, 0);
    	input_devices[id_key[i] + (6 * controller)] = value;
	}
	else {
		index = SendMessage(GetDlgItem(hDialog, id_device[i]), CB_GETCOUNT, 0, 0 );
		SendMessage(GetDlgItem(hDialog, id_device[i]), CB_SETCURSEL, index-1, 0 );
        value = SendMessage(GetDlgItem(hDialog, id_device[i]), CB_GETITEMDATA, index-1, 0);
    	input_devices[id_key[i] + (6 * controller)] = value;
	}
  }
  
  // Initialize parts list based on selected device
  for ( i = 0; i < 6; ++i ) {
  	input_InitializeKeys(hCombo = GetDlgItem(hDialog, id[i]), input_devices[id_key[i] + (6 * controller)]);
  	input_SelectKey(hCombo, input_keys[id_key[i] + (6 * controller)]);
  }
}

// ----------------------------------------------------------------------------
// SetControllerValues
// ----------------------------------------------------------------------------
static void input_SetControllerValues(HWND hDialog, byte controller) {

  int i,value,index;
  int id_key[] = {3,2,1,0,4,5};
  int id[] = {IDC_COMBO_CONTROLLER_UP,IDC_COMBO_CONTROLLER_DOWN,
              IDC_COMBO_CONTROLLER_LEFT,IDC_COMBO_CONTROLLER_RIGHT,
              IDC_COMBO_CONTROLLER_BUTTON1,IDC_COMBO_CONTROLLER_BUTTON2};
  int id_device[] = {IDC_COMBO_DEVICE_UP,IDC_COMBO_DEVICE_DOWN,
                     IDC_COMBO_DEVICE_LEFT,IDC_COMBO_DEVICE_RIGHT,
                     IDC_COMBO_DEVICE_BUTTON1,IDC_COMBO_DEVICE_BUTTON2};
  HWND hCombo;

  controller--;
  
  for ( i = 0; i < 17; ++i ) {
    input_values[i] = 0;
  }

  for ( i = 0; i < 6; ++i ) {
    hCombo = GetDlgItem(hDialog, id[i]);
    index = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    if(index != CB_ERR) {
      value = SendMessage(hCombo, CB_GETITEMDATA, index, 0);
      //if(!input_HasKeyValue(value)) {
        input_keys[id_key[i] + (6 * controller)] = value;
      //}
    }

    hCombo = GetDlgItem(hDialog, id_device[i]);
    index = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    if(index != CB_ERR) {
      value = SendMessage(hCombo, CB_GETITEMDATA, index, 0);
      input_devices[id_key[i] + (6 * controller)] = value;
	}
  }
}

// ----------------------------------------------------------------------------
// SetConsoleValues
// ----------------------------------------------------------------------------
static void input_SetConsoleValues(HWND hDialog) {

  int i,value,index;
  int id[] = {IDC_COMBO_CONSOLE_RESET,IDC_COMBO_CONSOLE_SELECT,
              IDC_COMBO_CONSOLE_PAUSE,IDC_COMBO_CONSOLE_RIGHT,
              IDC_COMBO_CONSOLE_LEFT};
  int id_device[] = {IDC_COMBO_DEVICE_RESET,IDC_COMBO_DEVICE_SELECT,
                     IDC_COMBO_DEVICE_PAUSE,IDC_COMBO_DEVICE_RIGHTD,
                     IDC_COMBO_DEVICE_LEFTD};
  HWND hCombo;

  for ( i = 0; i < 17; ++i ) {
    input_values[i] = 0;
  }

  for ( i = 0; i < 5; ++i ) {
    hCombo = GetDlgItem(hDialog, id[i]);
    index = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    if(index != CB_ERR) {
      value = SendMessage(hCombo, CB_GETITEMDATA, index, 0);
      //if(!input_HasKeyValue(value)) {
        input_keys[i+12] = value;
      //}
    }  

    hCombo = GetDlgItem(hDialog, id_device[i]);
    index = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    if(index != CB_ERR) {
      value = SendMessage(hCombo, CB_GETITEMDATA, index, 0);
      input_devices[i+12] = value;
	}
  }
}

// ----------------------------------------------------------------------------
// InitializeConsoleDialog
// ----------------------------------------------------------------------------
static void input_InitializeConsoleDialog(HWND hDialog) {

  HWND hCombo;
  int i;
  int value,index;
  int id[] = {IDC_COMBO_CONSOLE_RESET,IDC_COMBO_CONSOLE_SELECT,
              IDC_COMBO_CONSOLE_PAUSE,IDC_COMBO_CONSOLE_RIGHT,
              IDC_COMBO_CONSOLE_LEFT};
  int id_device[] = {IDC_COMBO_DEVICE_RESET,IDC_COMBO_DEVICE_SELECT,
                     IDC_COMBO_DEVICE_PAUSE,IDC_COMBO_DEVICE_RIGHTD,
                     IDC_COMBO_DEVICE_LEFTD};

  // Add Keyboard to Device List
  for ( i = 0; i < 5; ++i )
    input_AddDevice(GetDlgItem(hDialog, id_device[i]), "Keyboard", 0);

  // First find joysticks, if any, callback adds to list
  current_device = 0;
  if ( input_dinput != NULL )
	  input_dinput->EnumDevices(DIDEVTYPE_JOYSTICK, InitJoysticksListConsole,
	                            hDialog, DIEDFL_ATTACHEDONLY);

  // Select Keyboard or Joystick Device
  for ( i = 0; i < 5; ++i ) {
  	index = SendMessage(GetDlgItem(hDialog, id_device[i]), CB_SETCURSEL, (LPARAM)input_devices[i+12], 0);
  	if(index != CB_ERR) {
    	value = SendMessage(GetDlgItem(hDialog, id_device[i]), CB_GETITEMDATA, index, 0);
        input_devices[i+12] = value;
	}
	else {
		index = SendMessage(GetDlgItem(hDialog, id_device[i]), CB_GETCOUNT, 0, 0 );
		SendMessage(GetDlgItem(hDialog, id_device[i]), CB_SETCURSEL, index-1, 0 );
    	value = SendMessage(GetDlgItem(hDialog, id_device[i]), CB_GETITEMDATA, index-1, 0);
        input_devices[i+12] = value;
	}
  }
  
  // Initialize parts list based on selected device
  for ( i = 0; i < 5; ++i ) {
  	input_InitializeKeys(hCombo = GetDlgItem(hDialog, id[i]), input_devices[i+12]);
  	input_SelectKey(hCombo, input_keys[i+12]);
  }
}

// ----------------------------------------------------------------------------
// Controller1Procedure
// ----------------------------------------------------------------------------
static BOOL CALLBACK input_Controller1Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

  int value;
  int index, id_index;
  int id_key[] = {3,2,1,0,4,5};
  int id[] = {IDC_COMBO_CONTROLLER_UP,IDC_COMBO_CONTROLLER_DOWN,
              IDC_COMBO_CONTROLLER_LEFT,IDC_COMBO_CONTROLLER_RIGHT,
              IDC_COMBO_CONTROLLER_BUTTON1,IDC_COMBO_CONTROLLER_BUTTON2};

  switch(message) {
    case WM_INITDIALOG:
      input_InitializeControllerDialog(hWnd, 1);
      break;
    case WM_COMMAND:
      if(LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_BUTTON_CONTROLLER_CANCEL) {
		input_ReleaseJoysticks();
		current_device = 0;
        input_dinput->EnumDevices(DIDEVTYPE_JOYSTICK, InitJoysticksCallback,
                                  NULL, DIEDFL_ATTACHEDONLY);
        EndDialog(hWnd, 0);
        return 1;
      }
      else if(LOWORD(wParam) == IDC_BUTTON_CONTROLLER_OK) {
        input_SetControllerValues(hWnd, 1);
		input_ReleaseJoysticks();
		current_device = 0;
        input_dinput->EnumDevices(DIDEVTYPE_JOYSTICK, InitJoysticksCallback,
                                  NULL, DIEDFL_ATTACHEDONLY);
        EndDialog(hWnd, 0);
        return 1;
      }

	  // Check for device change - bberlin
	  else if ( HIWORD(wParam) == CBN_SELCHANGE) {
        index = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
        value = SendMessage((HWND)lParam, CB_GETITEMDATA, index, 0);

		switch ( LOWORD(wParam) ) {
		  case IDC_COMBO_DEVICE_UP: id_index = 0; break;
		  case IDC_COMBO_DEVICE_DOWN: id_index = 1; break;
		  case IDC_COMBO_DEVICE_LEFT: id_index = 2; break;
		  case IDC_COMBO_DEVICE_RIGHT: id_index = 3; break;
		  case IDC_COMBO_DEVICE_BUTTON1: id_index = 4; break;
		  case IDC_COMBO_DEVICE_BUTTON2: id_index = 5; break;
		  default: return 0; break;
		}

		if ( input_devices[id_key[id_index]] != value ) {
		  input_devices[id_key[id_index]] = value;
          index = SendMessage(GetDlgItem(hWnd, id[id_index]), CB_RESETCONTENT, 0, 0);
		  input_InitializeKeys(GetDlgItem(hWnd, id[id_index]), value);
		}
      }

      break;
  }
  return 0;
}

// ----------------------------------------------------------------------------
// Controller2Procedure
// ----------------------------------------------------------------------------
static BOOL CALLBACK input_Controller2Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

  int value;
  int index, id_index;
  int id_key[] = {3,2,1,0,4,5};
  int id[] = {IDC_COMBO_CONTROLLER_UP,IDC_COMBO_CONTROLLER_DOWN,
              IDC_COMBO_CONTROLLER_LEFT,IDC_COMBO_CONTROLLER_RIGHT,
              IDC_COMBO_CONTROLLER_BUTTON1,IDC_COMBO_CONTROLLER_BUTTON2};

  switch(message) {
    case WM_INITDIALOG:
      input_InitializeControllerDialog(hWnd, 2);
      break;
    case WM_COMMAND:
      if(LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_BUTTON_CONTROLLER_CANCEL) {
		input_ReleaseJoysticks();
		current_device = 0;
        input_dinput->EnumDevices(DIDEVTYPE_JOYSTICK, InitJoysticksCallback,
                                  NULL, DIEDFL_ATTACHEDONLY);
        EndDialog(hWnd, 0);
        return 1;
      }              
      else if(LOWORD(wParam) == IDC_BUTTON_CONTROLLER_OK) {
        input_SetControllerValues(hWnd, 2);
		input_ReleaseJoysticks();
		current_device = 0;
        input_dinput->EnumDevices(DIDEVTYPE_JOYSTICK, InitJoysticksCallback,
                                  NULL, DIEDFL_ATTACHEDONLY);
        EndDialog(hWnd, 0);
        return 1;
      }

	  // Check for device change - bberlin
	  else if ( HIWORD(wParam) == CBN_SELCHANGE) {
        index = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
        value = SendMessage((HWND)lParam, CB_GETITEMDATA, index, 0);

		switch ( LOWORD(wParam) ) {
		  case IDC_COMBO_DEVICE_UP: id_index = 0; break;
		  case IDC_COMBO_DEVICE_DOWN: id_index = 1; break;
		  case IDC_COMBO_DEVICE_LEFT: id_index = 2; break;
		  case IDC_COMBO_DEVICE_RIGHT: id_index = 3; break;
		  case IDC_COMBO_DEVICE_BUTTON1: id_index = 4; break;
		  case IDC_COMBO_DEVICE_BUTTON2: id_index = 5; break;
		  default: return 0; break;
		}

		if ( input_devices[id_key[id_index] + (6 * 1)] != value ) {
		  input_devices[id_key[id_index] + (6 * 1)] = value;
          index = SendMessage(GetDlgItem(hWnd, id[id_index]), CB_RESETCONTENT, 0, 0);
		  input_InitializeKeys(GetDlgItem(hWnd, id[id_index]), value);
		}
      }
      break;
  }
  return 0;
}

// ----------------------------------------------------------------------------
// ConsoleProcedure
// ----------------------------------------------------------------------------
static BOOL CALLBACK input_ConsoleProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

  int value;
  int index, id_index;
  int id[] = {IDC_COMBO_CONSOLE_RESET,IDC_COMBO_CONSOLE_SELECT,
              IDC_COMBO_CONSOLE_PAUSE,IDC_COMBO_CONSOLE_RIGHT,
              IDC_COMBO_CONSOLE_LEFT};

  switch(message) {
    case WM_INITDIALOG:
      input_InitializeConsoleDialog(hWnd);      
      break;
    case WM_COMMAND:
      if(LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_BUTTON_CONSOLE_CANCEL) {
		input_ReleaseJoysticks();
		current_device = 0;
        input_dinput->EnumDevices(DIDEVTYPE_JOYSTICK, InitJoysticksCallback,
                                  NULL, DIEDFL_ATTACHEDONLY);
        EndDialog(hWnd, 0);
        return 1;
      }
      else if(LOWORD(wParam) == IDC_BUTTON_CONSOLE_OK) {
        input_SetConsoleValues(hWnd);
		input_ReleaseJoysticks();
		current_device = 0;
        input_dinput->EnumDevices(DIDEVTYPE_JOYSTICK, InitJoysticksCallback,
                                  NULL, DIEDFL_ATTACHEDONLY);
        EndDialog(hWnd, 0);
        return 1;
      }

	  // Check for device change - bberlin
	  else if ( HIWORD(wParam) == CBN_SELCHANGE) {
        index = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
        value = SendMessage((HWND)lParam, CB_GETITEMDATA, index, 0);

		switch ( LOWORD(wParam) ) {
		  case IDC_COMBO_DEVICE_RESET: id_index = 0; break;
		  case IDC_COMBO_DEVICE_SELECT: id_index = 1; break;
		  case IDC_COMBO_DEVICE_PAUSE: id_index = 2; break;
		  case IDC_COMBO_DEVICE_RIGHTD: id_index = 3; break;
		  case IDC_COMBO_DEVICE_LEFTD: id_index = 4; break;
		  default: return 0; break;
		}

		if ( input_devices[id_index+12] != value ) {
		  input_devices[id_index+12] = value;
          index = SendMessage(GetDlgItem(hWnd, id[id_index]), CB_RESETCONTENT, 0, 0);
		  input_InitializeKeys(GetDlgItem(hWnd, id[id_index]), value);
		}
      }

      break;
  }
  return 0;
}

// ----------------------------------------------------------------------------
// InitJoysticksCallback
// ----------------------------------------------------------------------------
BOOL CALLBACK InitJoysticksCallback(const DIDEVICEINSTANCE*     
                                       pdidInstance, VOID* pContext) {
    int i, joy_index;
    HRESULT hr;
	bool needed = false;
	LPDIRECTINPUTDEVICE2 *p_joystick;
	LPDIRECTINPUTDEVICE p_dev;
    DIPROPRANGE diprg;

	joy_index = -1;
	if ( input_joystick[++joy_index] == NULL )
      p_joystick = &input_joystick[joy_index];
	else if ( input_joystick[++joy_index] == NULL )
      p_joystick = &input_joystick[joy_index];
	else
      return DIENUM_STOP;
 
	++current_device;

    // Check if we need this joystick
	for ( i = 0; i < 17; ++i ) {
      if ( input_devices[i] == current_device ) {
        needed = true;
		input_values[i] = joy_index;
      }
    }

	// If not needed, continue to check joysticks
    if ( !needed )
      return DIENUM_CONTINUE;

    // Obtain an interface to the enumerated joystick.
    hr = input_dinput->CreateDevice(pdidInstance->guidInstance,  
                                &p_dev, NULL);
    if(FAILED(hr)) 
        return DIENUM_CONTINUE;

	/* Set joystick data format */
	hr = (p_dev)->SetDataFormat( &c_dfDIJoystick );
	if( FAILED(hr) )
	{
		(p_dev)->Release( );
		p_dev = NULL;
		return DIENUM_CONTINUE;
	}

	/* Set the cooperative level */
	hr = (p_dev)->SetCooperativeLevel( (HWND)pContext, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND );
	if( FAILED(hr) )
	{
		(p_dev)->Release( );
		p_dev = NULL;
		return DIENUM_CONTINUE;
	}

	/* Set X-axis range to (-1000 ... +1000)
	   This lets us test against 0 to see which way the s_anStick is pointed */
	diprg.diph.dwSize       = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwObj        = DIJOFS_X;
	diprg.diph.dwHow        = DIPH_BYOFFSET;
	diprg.lMin              = -1000;
	diprg.lMax              = +1000;

	hr= (p_dev)->SetProperty( DIPROP_RANGE, &diprg.diph );
	if( FAILED(hr) )
	{
		(p_dev)->Release( );
		p_dev = NULL;
		return DIENUM_CONTINUE;
	}

	/* And again for Y-axis range */
	diprg.diph.dwObj = DIJOFS_Y;

	hr= (p_dev)->SetProperty( DIPROP_RANGE, &diprg.diph );
	if( FAILED(hr) )
	{
		(p_dev)->Release( );
		p_dev = NULL;
		return DIENUM_CONTINUE;
	}

	hr = (p_dev)->QueryInterface (IID_IDirectInputDevice2, (LPVOID *)p_joystick );
	if( FAILED(hr) )
	{
		(p_dev)->Release( );
		p_dev = NULL;
		return DIENUM_CONTINUE;
	}

    return DIENUM_CONTINUE;
}

// ----------------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------------
bool input_Initialize(HWND hWnd, HINSTANCE hInstance) {
  if(hWnd == NULL) {
    logger_LogError("Handle to the window is invalid.", INPUT_SOURCE);
  }
  if(hInstance == NULL) {
    logger_LogError("Handle to the application instance is invalid.", INPUT_SOURCE);
    return false;
  }
  
  HRESULT hr = DirectInputCreate(hInstance, DIRECTINPUT_VERSION, &input_dinput, NULL);
  if(FAILED(hr) || input_dinput == NULL) {
    logger_LogError("Failed to initialize direct input.", INPUT_SOURCE);
    logger_LogError(common_Format(hr), INPUT_SOURCE);
    return false;  
  }
  
  hr = input_dinput->CreateDevice(GUID_SysKeyboard, &input_keyboard, NULL);
  if(FAILED(hr) || input_keyboard == NULL) {
    logger_LogError("Failed to create the keyboard device.", INPUT_SOURCE);
    logger_LogError(common_Format(hr), INPUT_SOURCE);    
    return false;
  }
  
  hr = input_keyboard->SetDataFormat(&c_dfDIKeyboard);
  if(FAILED(hr)) {
    logger_LogError("Failed to set the keyboard format.", INPUT_SOURCE);
    logger_LogError(common_Format(hr), INPUT_SOURCE);    
    return false;
  }
  
  hr = input_keyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
  if(FAILED(hr)) {
    logger_LogError("Failed to set the cooperative level.", INPUT_SOURCE);
    logger_LogError(common_Format(hr), INPUT_SOURCE);
    return false;
  }
  
  hr = input_keyboard->Acquire( );
  if(FAILED(hr)) {
    logger_LogError("Failed to aquire the keyboard.", INPUT_SOURCE);
    logger_LogError(common_Format(hr), INPUT_SOURCE);
    return false;
  }

  // Add JoyStick Input - bberlin
  current_device = 0;
  input_dinput->EnumDevices(DIDEVTYPE_JOYSTICK, InitJoysticksCallback,
                            NULL, DIEDFL_ATTACHEDONLY);
 
  return true;
}

// ----------------------------------------------------------------------------
// GetKeyboardState
// ----------------------------------------------------------------------------
bool input_GetKeyboardState(byte* input) {

  int i;
  DIJOYSTATE js;

  if(input_dinput == NULL) {
    logger_LogError("Direct input has not been initialized.", INPUT_SOURCE);
    return false;
  }
  if(input_keyboard == NULL) {
    logger_LogError("Direct input keyboard has not been initialized.", INPUT_SOURCE);
    return false;
  }
  if(input == NULL) {
    logger_LogError("Keyboard input buffer is invalid.", INPUT_SOURCE);
    return false;
  }
  
  byte keyboard[256];
  HRESULT hr = input_keyboard->GetDeviceState(256, keyboard);
  if(FAILED(hr)) {
    logger_LogError("Failed to retrieve the keyboard state.", INPUT_SOURCE);
    logger_LogError(common_Format(hr), INPUT_SOURCE);
    return false;
  }
  
  for(int index = 0; index < 17; index++) {
    if ( !input_values[index] )
      input[index] = (keyboard[input_keys[index]])? 1: 0;
  }
  
  for ( i = 0; i < 2; ++i ) {
    // Get Joystick Input
    if ( input_joystick[i] == NULL )
      return true;
    hr = input_joystick[i]->Poll();
    if(FAILED(hr)) {
      hr = input_joystick[i]->Acquire();
    }
    hr = input_joystick[i]->GetDeviceState(sizeof(DIJOYSTATE), &js);
    if(FAILED(hr)) {
      logger_LogError("Failed to retrieve the joystick state.", INPUT_SOURCE);
      logger_LogError(common_Format(hr), INPUT_SOURCE);
      continue;
    }

	// Check if we need the info
    for(int index = 0; index < 17; index++) {
      if ( input_values[index] == i ) {
        switch ( input_keys[index] ) {
          case JOY_AXIS_UP:
            input[index] = ( js.lY < -500 ) ? 1: 0;
            break;
          case JOY_AXIS_DOWN:
            input[index] = ( js.lY > 500 ) ? 1: 0;
            break;
          case JOY_AXIS_LEFT:
            input[index] = ( js.lX < -500 ) ? 1: 0;
            break;
          case JOY_AXIS_RIGHT:
            input[index] = ( js.lX > 500 ) ? 1: 0;
            break;
          case JOY_PAD_UP:
            input[index] = ( (js.rgdwPOV[0] > 32000 && js.rgdwPOV[0] < 37000) ||
                             (js.rgdwPOV[0] >= 0 && js.rgdwPOV[0] < 4600) ) ? 1: 0;
            break;
          case JOY_PAD_DOWN:
            input[index] = (js.rgdwPOV[0] > 13400 && js.rgdwPOV[0] < 22600) ? 1: 0;
          break;
          case JOY_PAD_LEFT:
            input[index] = (js.rgdwPOV[0] > 22400 && js.rgdwPOV[0] < 31600) ? 1: 0;
          break;
          case JOY_PAD_RIGHT:
            input[index] = (js.rgdwPOV[0] > 4400 && js.rgdwPOV[0] < 13600) ? 1: 0;
          break;
          case JOY_BUTTON_1:
          case JOY_BUTTON_2:
          case JOY_BUTTON_3:
          case JOY_BUTTON_4:
          case JOY_BUTTON_5:
          case JOY_BUTTON_6:
          case JOY_BUTTON_7:
          case JOY_BUTTON_8:
          case JOY_BUTTON_9:
          case JOY_BUTTON_10:
          case JOY_BUTTON_11:
          case JOY_BUTTON_12:
            input[index] = ( js.rgbButtons[input_keys[index]-JOY_BUTTON_1] & 0x80 ) ? 1: 0;
          break;
        }

      } /* end if this part using this joystick */

	} /* end for emulated part */

  } /* end for PC joystick */

  return true;
}

// ----------------------------------------------------------------------------
// Release
// ----------------------------------------------------------------------------
void input_Release( ) {
  if(input_dinput != NULL) {
    input_dinput->Release( );
    input_dinput = NULL;
  }
  if(input_keyboard != NULL) {
    input_keyboard->Release( );
    input_keyboard = NULL;
  }
  input_ReleaseJoysticks();
}

void input_ReleaseJoysticks( ) {

  int i;

  for ( i = 0; i < 2; ++i ) {
    if(input_joystick[i] != NULL) {
      input_joystick[i]->Unacquire( );
      input_joystick[i]->Release( );
      input_joystick[i] = NULL;
    }
  }
}

// ----------------------------------------------------------------------------
// ShowController1Dialog
// ----------------------------------------------------------------------------
void input_ShowController1Dialog(HWND hWnd, HINSTANCE hInstance) {
  DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_CONTROLLER), hWnd, (DLGPROC)input_Controller1Procedure);
}

// ----------------------------------------------------------------------------
// ShowController2Dialog
// ----------------------------------------------------------------------------
void input_ShowController2Dialog(HWND hWnd, HINSTANCE hInstance) {
  DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_CONTROLLER), hWnd, (DLGPROC)input_Controller2Procedure);
}

// ----------------------------------------------------------------------------
// ShowConsoleDialog
// ----------------------------------------------------------------------------
void input_ShowConsoleDialog(HWND hWnd, HINSTANCE hInstance) {
  DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_CONSOLE), hWnd, (DLGPROC)input_ConsoleProcedure);
}

// ----------------------------------------------------------------------------
// Key Titles
// ----------------------------------------------------------------------------
const std::string input_keyTitles[ ] = {
  "ESCAPE",
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "9",
  "0",
  "MINUS",
  "EQUALS",
  "BACK",
  "TAB",
  "Q",
  "W",
  "E",
  "R",
  "T",
  "Y",
  "U",
  "I",
  "O",
  "P",
  "L BRACKET",
  "R BRACKET",
  "RETURN",
  "L CONTROL",
  "A",
  "S",
  "D",
  "F",
  "G",
  "H",
  "J",
  "K",
  "L",
  "SEMICOLON",
  "APOSTROPHE",
  "GRAVE",
  "L SHIFT",
  "BACKSLASH",
  "Z",
  "X",
  "C",
  "V",
  "B",
  "N",
  "M",
  "COMMA",
  "PERIOD",
  "SLASH",
  "R SHIFT",
  "MULTIPLY",
  "L ALT",
  "SPACE",
  "CAPS LOCK",
  "F1",
  "F2",
  "F3",
  "F4",
  "F5",
  "F6",
  "F7",
  "F8",
  "F9",
  "F10",
  "NUMLOCK",
  "SCROLL",
  "NUMPAD 7",
  "NUMPAD 8",
  "NUMPAD 9",
  "SUBTRACT",
  "NUMPAD 4",
  "NUMPAD 5",
  "NUMPAD 6",
  "ADD",
  "NUMPAD 1",
  "NUMPAD 2",
  "NUMPAD 3",
  "NUMPAD 0",
  "DECIMAL",
  "F11",
  "F12",
  "NUMPAD ENTER",
  "R CONTROL",
  "DIVIDE",
  "R ALT",
  "PAUSE",
  "HOME",
  "UP",
  "PGUP",
  "LEFT",
  "RIGHT",
  "END",
  "DOWN",
  "PGDN",
  "INSERT",
  "DELETE"
};

// ----------------------------------------------------------------------------
// Key Values
// ----------------------------------------------------------------------------
const byte input_keyValues[ ] = {
  DIK_ESCAPE,
  DIK_1,
  DIK_2,
  DIK_3,
  DIK_4,
  DIK_5,
  DIK_6,
  DIK_7,
  DIK_8,
  DIK_9,
  DIK_0,
  DIK_MINUS,
  DIK_EQUALS,
  DIK_BACK,
  DIK_TAB,
  DIK_Q,
  DIK_W,
  DIK_E,
  DIK_R,
  DIK_T,
  DIK_Y,
  DIK_U,
  DIK_I,
  DIK_O,
  DIK_P,
  DIK_LBRACKET,
  DIK_RBRACKET,
  DIK_RETURN,
  DIK_LCONTROL,
  DIK_A,
  DIK_S,
  DIK_D,
  DIK_F,
  DIK_G,
  DIK_H,
  DIK_J,
  DIK_K,
  DIK_L,
  DIK_SEMICOLON,
  DIK_APOSTROPHE,
  DIK_GRAVE,
  DIK_LSHIFT,
  DIK_BACKSLASH,
  DIK_Z,
  DIK_X,
  DIK_C,
  DIK_V,
  DIK_B,
  DIK_N,
  DIK_M,
  DIK_COMMA,
  DIK_PERIOD,
  DIK_SLASH,
  DIK_RSHIFT,
  DIK_MULTIPLY,
  DIK_LMENU,
  DIK_SPACE,
  DIK_CAPITAL,
  DIK_F1,
  DIK_F2,
  DIK_F3,
  DIK_F4,
  DIK_F5,
  DIK_F6,
  DIK_F7,
  DIK_F8,
  DIK_F9,
  DIK_F10,
  DIK_NUMLOCK,
  DIK_SCROLL,
  DIK_NUMPAD7,
  DIK_NUMPAD8,
  DIK_NUMPAD9,
  DIK_SUBTRACT,
  DIK_NUMPAD4,
  DIK_NUMPAD5,
  DIK_NUMPAD6,
  DIK_ADD,
  DIK_NUMPAD1,
  DIK_NUMPAD2,
  DIK_NUMPAD3,
  DIK_NUMPAD0,
  DIK_DECIMAL,
  DIK_F11,
  DIK_F12,
  DIK_NUMPADENTER,
  DIK_RCONTROL,
  DIK_DIVIDE,
  DIK_RMENU,
  DIK_PAUSE,
  DIK_HOME,
  DIK_UP,
  DIK_PRIOR,
  DIK_LEFT,
  DIK_RIGHT,
  DIK_END,
  DIK_DOWN,
  DIK_NEXT,
  DIK_INSERT,
  DIK_DELETE
};

// ----------------------------------------------------------------------------
// Joy Titles
// ----------------------------------------------------------------------------
const std::string input_joyTitles[ ] = {
  "Axis Up",
  "Axis Down",
  "Axis Left",
  "Axis Right",
  "Pad Up",
  "Pad Down",
  "Pad Left",
  "Pad Right",
  "Button1",
  "Button2",
  "Button3",
  "Button4",
  "Button5",
  "Button6",
  "Button7",
  "Button8",
  "Button9",
  "Button10",
  "Button11",
  "Button12"
};

// ----------------------------------------------------------------------------
// Joy Values
// ----------------------------------------------------------------------------
const byte input_joyValues[ ] = {
  JOY_AXIS_UP,
  JOY_AXIS_DOWN,
  JOY_AXIS_LEFT,
  JOY_AXIS_RIGHT,
  JOY_PAD_UP,
  JOY_PAD_DOWN,
  JOY_PAD_LEFT,
  JOY_PAD_RIGHT,
  JOY_BUTTON_1,
  JOY_BUTTON_2,
  JOY_BUTTON_3,
  JOY_BUTTON_4,
  JOY_BUTTON_5,
  JOY_BUTTON_6,
  JOY_BUTTON_7,
  JOY_BUTTON_8,
  JOY_BUTTON_9,
  JOY_BUTTON_10,
  JOY_BUTTON_11,
  JOY_BUTTON_12
};
