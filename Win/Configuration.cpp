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
// Configuration.cpp
// ----------------------------------------------------------------------------
#include "Configuration.h"

bool configuration_enabled = true;

static const std::string CONFIGURATION_SECTION_CONSOLE = "Console";
static const std::string CONFIGURATION_SECTION_RECENT = "Recent";
static const std::string CONFIGURATION_SECTION_DISPLAY = "Display";
static const std::string CONFIGURATION_SECTION_SOUND = "Sound";
static const std::string CONFIGURATION_SECTION_EMULATION = "Emulation";
static const std::string CONFIGURATION_SECTION_INPUT = "Input";
static std::string configuration_filename;

// ----------------------------------------------------------------------------
// HasKey
// ----------------------------------------------------------------------------
static bool configuration_HasKey(std::string section, std::string name) {
  char value[128];
  GetPrivateProfileString(section.c_str( ), name.c_str( ), "__TEST__", value, 128, configuration_filename.c_str( ));
  std::string test = value;
  if(test != "__TEST__") {
    return true;
  }
  return false;
}

// ----------------------------------------------------------------------------
// WritePrivatePath
// ----------------------------------------------------------------------------
static void configuration_WritePrivatePath(std::string section, std::string name, std::string path) {
  WritePrivateProfileString(section.c_str( ), name.c_str( ), path.c_str( ), configuration_filename.c_str( ));
}

// ----------------------------------------------------------------------------
// WritePrivateBool
// ----------------------------------------------------------------------------
static void configuration_WritePrivateBool(std::string section, std::string name, bool value) {
  WritePrivateProfileString(section.c_str( ), name.c_str( ), common_Format(value).c_str( ), configuration_filename.c_str( ));
}

// ----------------------------------------------------------------------------
// WritePrivateUint
// ----------------------------------------------------------------------------
static void configuration_WritePrivateUint(std::string section, std::string name, uint value) {
  WritePrivateProfileString(section.c_str( ), name.c_str( ), common_Format(value).c_str( ), configuration_filename.c_str( ));
}

// ----------------------------------------------------------------------------
// ReadPrivatePath
// ----------------------------------------------------------------------------
static std::string configuration_ReadPrivatePath(std::string section, std::string name, std::string defaultValue) {
  char path[_MAX_PATH];
  GetPrivateProfileString(section.c_str( ), name.c_str( ), defaultValue.c_str( ), path, _MAX_PATH, configuration_filename.c_str( ));
  return path;
}

// ----------------------------------------------------------------------------
// ReadPrivateBool
// ----------------------------------------------------------------------------
static bool configuration_ReadPrivateBool(std::string section, std::string name, std::string defaultValue) {
  char value[6];
  GetPrivateProfileString(section.c_str( ), name.c_str( ), defaultValue.c_str( ), value, 6, configuration_filename.c_str( ));
  return common_ParseBool(value);
}

// ----------------------------------------------------------------------------
// ReadPrivateUint
// ----------------------------------------------------------------------------
static uint configuration_ReadPrivateUint(std::string section, std::string name, uint value) {
  return GetPrivateProfileInt(section.c_str( ), name.c_str( ), value, configuration_filename.c_str( ));
}

// ----------------------------------------------------------------------------
// Load
// ----------------------------------------------------------------------------
void configuration_Load(std::string filename) {
  configuration_filename = filename; 
  for(uint index = 0; index < 10; index++) {
    console_recent[index] = configuration_ReadPrivatePath(CONFIGURATION_SECTION_RECENT, "Recent" + common_Format(index), "");
  }
  
  RECT windowRect = {0};
  windowRect.left = configuration_ReadPrivateUint(CONFIGURATION_SECTION_CONSOLE, "Window.Left", 0);
  windowRect.top = configuration_ReadPrivateUint(CONFIGURATION_SECTION_CONSOLE, "Window.Top", 0);
  windowRect.right = configuration_ReadPrivateUint(CONFIGURATION_SECTION_CONSOLE, "Window.Right", 0);
  windowRect.bottom = configuration_ReadPrivateUint(CONFIGURATION_SECTION_CONSOLE, "Window.Bottom", 0);
  console_SetWindowRect(windowRect);

  display_stretched = configuration_ReadPrivateBool(CONFIGURATION_SECTION_DISPLAY, "Stretched", "false");
  display_mode.width = configuration_ReadPrivateUint(CONFIGURATION_SECTION_DISPLAY, "Mode.Width", 640);
  display_mode.height = configuration_ReadPrivateUint(CONFIGURATION_SECTION_DISPLAY, "Mode.Height", 480);
  display_mode.bpp = configuration_ReadPrivateUint(CONFIGURATION_SECTION_DISPLAY, "Mode.BPP", 8);
  
  if(configuration_HasKey(CONFIGURATION_SECTION_DISPLAY, "Palette.Default") && configuration_HasKey(CONFIGURATION_SECTION_DISPLAY, "Palette.Filename")) {
    palette_default = configuration_ReadPrivateBool(CONFIGURATION_SECTION_DISPLAY, "Palette.Default", "true");
    if(!palette_default) {
      palette_Load(configuration_ReadPrivatePath(CONFIGURATION_SECTION_DISPLAY, "Palette.Filename", ""));
    }
  }

  console_SetZoom(configuration_ReadPrivateUint(CONFIGURATION_SECTION_DISPLAY, "Zoom", 1));
  console_SetFullscreen(configuration_ReadPrivateBool(CONFIGURATION_SECTION_DISPLAY, "Fullscreen", "false"));
  
  sound_SetMuted(configuration_ReadPrivateBool(CONFIGURATION_SECTION_SOUND, "Mute", "false"));
  sound_latency = configuration_ReadPrivateUint(CONFIGURATION_SECTION_SOUND, "Latency", 1);
  sound_SetSampleRate(configuration_ReadPrivateUint(CONFIGURATION_SECTION_SOUND, "Sample.Rate", 44100));

  region_type = configuration_ReadPrivateUint(CONFIGURATION_SECTION_EMULATION, "Region", 2);
  console_frameSkip = configuration_ReadPrivateUint(CONFIGURATION_SECTION_EMULATION, "Frame.Skip", 0);
  
  if(configuration_HasKey(CONFIGURATION_SECTION_EMULATION, "Bios.Enabled") && configuration_HasKey(CONFIGURATION_SECTION_EMULATION, "Bios.Filename")) {
    bios_enabled = configuration_ReadPrivateBool(CONFIGURATION_SECTION_EMULATION, "Bios.Enabled", "false");
    if(bios_enabled) {
      bios_Load(configuration_ReadPrivatePath(CONFIGURATION_SECTION_EMULATION, "Bios.Filename", ""));
    }
  }
  
  if(configuration_HasKey(CONFIGURATION_SECTION_EMULATION, "Database.Enabled") && configuration_HasKey(CONFIGURATION_SECTION_EMULATION, "Database.Filename")) {
    database_enabled = configuration_ReadPrivateBool(CONFIGURATION_SECTION_EMULATION, "Database.Enabled", "true");
    if(database_enabled) {
      database_filename = configuration_ReadPrivatePath(CONFIGURATION_SECTION_EMULATION, "Database.Filename", "");
    }
  }
  
  for(index = 0; index < 17; index++) {
    input_keys[index] = configuration_ReadPrivateUint(CONFIGURATION_SECTION_INPUT, "Key" + common_Format(index), input_keys[index]);
  }

  console_savePath = configuration_ReadPrivatePath(CONFIGURATION_SECTION_CONSOLE, "Save.Path", "");
}

// ----------------------------------------------------------------------------
// Save
// ----------------------------------------------------------------------------
void configuration_Save(std::string filename) {
  configuration_filename = filename; 
  for(uint index = 0; index < 10; index++) {
    configuration_WritePrivatePath(CONFIGURATION_SECTION_RECENT, "Recent" + common_Format(index), console_recent[index]);
  }

  configuration_WritePrivateBool(CONFIGURATION_SECTION_DISPLAY, "Fullscreen", display_IsFullscreen( ));
  configuration_WritePrivateUint(CONFIGURATION_SECTION_DISPLAY, "Mode.Height", display_mode.height);
  configuration_WritePrivateUint(CONFIGURATION_SECTION_DISPLAY, "Mode.Width", display_mode.width);
  configuration_WritePrivateUint(CONFIGURATION_SECTION_DISPLAY, "Mode.BPP", display_mode.bpp);
  configuration_WritePrivateBool(CONFIGURATION_SECTION_DISPLAY, "Palette.Default", palette_default);
  configuration_WritePrivatePath(CONFIGURATION_SECTION_DISPLAY, "Palette.Filename", palette_filename);
  configuration_WritePrivateBool(CONFIGURATION_SECTION_DISPLAY, "Stretched", display_stretched);
  configuration_WritePrivateUint(CONFIGURATION_SECTION_DISPLAY, "Zoom", display_zoom);
  
  configuration_WritePrivateBool(CONFIGURATION_SECTION_SOUND, "Mute", sound_IsMuted( ));
  configuration_WritePrivateUint(CONFIGURATION_SECTION_SOUND, "Sample.Rate", sound_GetSampleRate( ));
  configuration_WritePrivateUint(CONFIGURATION_SECTION_SOUND, "Latency", sound_latency);

  configuration_WritePrivateUint(CONFIGURATION_SECTION_EMULATION, "Region", region_type);
  configuration_WritePrivateUint(CONFIGURATION_SECTION_EMULATION, "Frame.Skip", console_frameSkip);
  configuration_WritePrivatePath(CONFIGURATION_SECTION_EMULATION, "Bios.Filename", bios_filename);
  configuration_WritePrivateBool(CONFIGURATION_SECTION_EMULATION, "Bios.Enabled", bios_enabled);
  configuration_WritePrivatePath(CONFIGURATION_SECTION_EMULATION, "Database.Filename", database_filename);
  configuration_WritePrivateBool(CONFIGURATION_SECTION_EMULATION, "Database.Enabled", database_enabled);

  for(index = 0; index < 17; index++) {
    configuration_WritePrivateUint(CONFIGURATION_SECTION_INPUT, "Key" + common_Format(index), input_keys[index]);
  }
  configuration_WritePrivatePath(CONFIGURATION_SECTION_CONSOLE, "Save.Path", console_savePath);

  RECT windowRect = console_GetWindowRect( );
  configuration_WritePrivateUint(CONFIGURATION_SECTION_CONSOLE, "Window.Left", windowRect.left);
  configuration_WritePrivateUint(CONFIGURATION_SECTION_CONSOLE, "Window.Top", windowRect.top);
  configuration_WritePrivateUint(CONFIGURATION_SECTION_CONSOLE, "Window.Right", windowRect.right);
  configuration_WritePrivateUint(CONFIGURATION_SECTION_CONSOLE, "Window.Bottom", windowRect.bottom);
}
