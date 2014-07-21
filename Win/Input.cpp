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
#include "Input.h"
#define INPUT_SOURCE "Input.cpp"
#define INPUT_KEY_SIZE 100

byte input_keys[17] = {DIK_RIGHT, DIK_LEFT, DIK_DOWN, DIK_UP, DIK_X, DIK_Z, DIK_L, DIK_J, DIK_K, DIK_I, DIK_M, DIK_N, DIK_F2, DIK_F3, DIK_F4, DIK_F5, DIK_F6};

static LPDIRECTINPUT input_dinput = NULL;
static LPDIRECTINPUTDEVICE input_keyboard = NULL;
extern const std::string input_keyTitles[ ];
extern const byte input_keyValues[ ];

// ----------------------------------------------------------------------------
// InitializeKeys
// ----------------------------------------------------------------------------
static void input_InitializeKeys(HWND hCombo) {
  for(int index = 0; index < INPUT_KEY_SIZE; index++) {
    SendMessage(hCombo, CB_INSERTSTRING, index, (LPARAM)input_keyTitles[index].c_str( ));
    SendMessage(hCombo, CB_SETITEMDATA, index, input_keyValues[index]);
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
// InitializeControllerDialog
// ----------------------------------------------------------------------------
static input_InitializeControllerDialog(HWND hDialog, byte controller) {
  std::string title = "Controller " + common_Format(controller);
  SetWindowText(hDialog, title.c_str( ));
  
  controller--;
  HWND hComboUp = GetDlgItem(hDialog, IDC_COMBO_CONTROLLER_UP);
  input_InitializeKeys(hComboUp);
  input_SelectKey(hComboUp, input_keys[3 + (6 * controller)]);

  HWND hComboDown = GetDlgItem(hDialog, IDC_COMBO_CONTROLLER_DOWN);
  input_InitializeKeys(hComboDown);
  input_SelectKey(hComboDown, input_keys[2 + (6 * controller)]);

  HWND hComboLeft = GetDlgItem(hDialog, IDC_COMBO_CONTROLLER_LEFT);
  input_InitializeKeys(hComboLeft);
  input_SelectKey(hComboLeft, input_keys[1 + (6 * controller)]);

  HWND hComboRight = GetDlgItem(hDialog, IDC_COMBO_CONTROLLER_RIGHT);
  input_InitializeKeys(hComboRight);
  input_SelectKey(hComboRight, input_keys[0 + (6 * controller)]);

  HWND hComboButton1 = GetDlgItem(hDialog, IDC_COMBO_CONTROLLER_BUTTON1);
  input_InitializeKeys(hComboButton1);
  input_SelectKey(hComboButton1, input_keys[4 + (6 * controller)]);

  HWND hComboButton2 = GetDlgItem(hDialog, IDC_COMBO_CONTROLLER_BUTTON2);
  input_InitializeKeys(hComboButton2);
  input_SelectKey(hComboButton2, input_keys[5 + (6 * controller)]);
}

// ----------------------------------------------------------------------------
// SetControllerValues
// ----------------------------------------------------------------------------
static void input_SetControllerValues(HWND hDialog, byte controller) {
  controller--;
  
  HWND hComboUp = GetDlgItem(hDialog, IDC_COMBO_CONTROLLER_UP);
  int index = SendMessage(hComboUp, CB_GETCURSEL, 0, 0);
  if(index != CB_ERR) {
    int value = SendMessage(hComboUp, CB_GETITEMDATA, index, 0);
    if(!input_HasKeyValue(value)) {
      input_keys[3 + (6 * controller)] = value;
    }
  }
 
  HWND hComboDown = GetDlgItem(hDialog, IDC_COMBO_CONTROLLER_DOWN);
  index = SendMessage(hComboDown, CB_GETCURSEL, 0, 0);
  if(index != CB_ERR) {
    int value = SendMessage(hComboDown, CB_GETITEMDATA, index, 0); 
    if(!input_HasKeyValue(value)) {
      input_keys[2 + (6 * controller)] = value;
    }
  }  
 
  HWND hComboLeft = GetDlgItem(hDialog, IDC_COMBO_CONTROLLER_LEFT);
  index = SendMessage(hComboLeft, CB_GETCURSEL, 0, 0);
  if(index != CB_ERR) {
    int value = SendMessage(hComboLeft, CB_GETITEMDATA, index, 0);
    if(!input_HasKeyValue(value)) {
      input_keys[1 + (6 * controller)] = value;
    }
  } 

  HWND hComboRight = GetDlgItem(hDialog, IDC_COMBO_CONTROLLER_RIGHT);
  index = SendMessage(hComboRight, CB_GETCURSEL, 0, 0);
  if(index != CB_ERR) {
    int value = SendMessage(hComboRight, CB_GETITEMDATA, index, 0);
    if(!input_HasKeyValue(value)) {
      input_keys[0 + (6 * controller)] = value;
    }
  }

  HWND hComboButton1 = GetDlgItem(hDialog, IDC_COMBO_CONTROLLER_BUTTON1);
  index = SendMessage(hComboButton1, CB_GETCURSEL, 0, 0);
  if(index != CB_ERR) {
    int value = SendMessage(hComboButton1, CB_GETITEMDATA, index, 0);
    if(!input_HasKeyValue(value)) {
      input_keys[4 + (6 * controller)] = value;
    }
  }

  HWND hComboButton2 = GetDlgItem(hDialog, IDC_COMBO_CONTROLLER_BUTTON2);
  index = SendMessage(hComboButton2, CB_GETCURSEL, 0, 0);
  if(index != CB_ERR) {
    int value = SendMessage(hComboButton2, CB_GETITEMDATA, index, 0);
    if(!input_HasKeyValue(value)) {
      input_keys[5 + (6 * controller)] = value;
    }
  }
}

// ----------------------------------------------------------------------------
// SetConsoleValues
// ----------------------------------------------------------------------------
static void input_SetConsoleValues(HWND hDialog) {
  HWND hComboReset = GetDlgItem(hDialog, IDC_COMBO_CONSOLE_RESET);
  int index = SendMessage(hComboReset, CB_GETCURSEL, 0, 0);
  if(index != CB_ERR) {
    int value = SendMessage(hComboReset, CB_GETITEMDATA, index, 0);
    if(!input_HasKeyValue(value)) {
      input_keys[12] = value;
    }
  }  
  
  HWND hComboSelect = GetDlgItem(hDialog, IDC_COMBO_CONSOLE_SELECT);
  index = SendMessage(hComboSelect, CB_GETCURSEL, 0, 0);
  if(index != CB_ERR) {
    int value = SendMessage(hComboSelect, CB_GETITEMDATA, index, 0);
    if(!input_HasKeyValue(value)) {
      input_keys[13] = value;
    }
  }

  HWND hComboPause = GetDlgItem(hDialog, IDC_COMBO_CONSOLE_PAUSE);
  index = SendMessage(hComboPause, CB_GETCURSEL, 0, 0);
  if(index != CB_ERR) {
    int value = SendMessage(hComboPause, CB_GETITEMDATA, index, 0);
    if(!input_HasKeyValue(value)) {
      input_keys[14] = value;
    }
  }

  HWND hComboRight = GetDlgItem(hDialog, IDC_COMBO_CONSOLE_RIGHT);
  index = SendMessage(hComboRight, CB_GETCURSEL, 0, 0);
  if(index != CB_ERR) {
    int value = SendMessage(hComboRight, CB_GETITEMDATA, index, 0);
    if(!input_HasKeyValue(value)) {
      input_keys[15] = value;
    }
  }

  HWND hComboLeft = GetDlgItem(hDialog, IDC_COMBO_CONSOLE_LEFT);
  index = SendMessage(hComboLeft, CB_GETCURSEL, 0, 0);
  if(index != CB_ERR) {
    int value = SendMessage(hComboLeft, CB_GETITEMDATA, index, 0);
    if(!input_HasKeyValue(value)) {
      input_keys[16] = value;
    }
  }
}

// ----------------------------------------------------------------------------
// InitializeConsoleDialog
// ----------------------------------------------------------------------------
static void input_InitializeConsoleDialog(HWND hDialog) {
  HWND hComboReset = GetDlgItem(hDialog, IDC_COMBO_CONSOLE_RESET);
  input_InitializeKeys(hComboReset);
  input_SelectKey(hComboReset, input_keys[12]);

  HWND hComboSelect = GetDlgItem(hDialog, IDC_COMBO_CONSOLE_SELECT);
  input_InitializeKeys(hComboSelect);
  input_SelectKey(hComboSelect, input_keys[13]);
  
  HWND hComboPause = GetDlgItem(hDialog, IDC_COMBO_CONSOLE_PAUSE);
  input_InitializeKeys(hComboPause);
  input_SelectKey(hComboPause, input_keys[14]);

  HWND hComboRight = GetDlgItem(hDialog, IDC_COMBO_CONSOLE_RIGHT);
  input_InitializeKeys(hComboRight);
  input_SelectKey(hComboRight, input_keys[15]);

  HWND hComboLeft = GetDlgItem(hDialog, IDC_COMBO_CONSOLE_LEFT);
  input_InitializeKeys(hComboLeft);
  input_SelectKey(hComboLeft, input_keys[16]);
}

// ----------------------------------------------------------------------------
// Controller1Procedure
// ----------------------------------------------------------------------------
static BOOL CALLBACK input_Controller1Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch(message) {
    case WM_INITDIALOG:
      input_InitializeControllerDialog(hWnd, 1);
      break;
    case WM_COMMAND:
      if(LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_BUTTON_CONTROLLER_CANCEL) {
        EndDialog(hWnd, 0);
        return 1;
      }
      else if(LOWORD(wParam) == IDC_BUTTON_CONTROLLER_OK) {
        input_SetControllerValues(hWnd, 1);
        EndDialog(hWnd, 0);
        return 1;
      }
      break;
  }
  return 0;
}

// ----------------------------------------------------------------------------
// Controller2Procedure
// ----------------------------------------------------------------------------
static BOOL CALLBACK input_Controller2Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch(message) {
    case WM_INITDIALOG:
      input_InitializeControllerDialog(hWnd, 2);
      break;
    case WM_COMMAND:
      if(LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_BUTTON_CONTROLLER_CANCEL) {
        EndDialog(hWnd, 0);
        return 1;
      }              
      else if(LOWORD(wParam) == IDC_BUTTON_CONTROLLER_OK) {
        input_SetControllerValues(hWnd, 2);
        EndDialog(hWnd, 0);
        return 1;
      }
      break;
  }
  return 0;
}

// ----------------------------------------------------------------------------
// ConsoleProcedure
// ----------------------------------------------------------------------------
static BOOL CALLBACK input_ConsoleProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch(message) {
    case WM_INITDIALOG:
      input_InitializeConsoleDialog(hWnd);      
      break;
    case WM_COMMAND:
      if(LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_BUTTON_CONSOLE_CANCEL) {
        EndDialog(hWnd, 0);
        return 1;
      }
      else if(LOWORD(wParam) == IDC_BUTTON_CONSOLE_OK) {
        input_SetConsoleValues(hWnd);
        EndDialog(hWnd, 0);
        return 1;
      }
      break;
  }
  return 0;
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

  return true;
}

// ----------------------------------------------------------------------------
// GetKeyboardState
// ----------------------------------------------------------------------------
bool input_GetKeyboardState(byte* input) {
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
  
  for(int index = 0; index < 19; index++) {
    input[index] = (keyboard[input_keys[index]])? 1: 0;
  }
  
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

