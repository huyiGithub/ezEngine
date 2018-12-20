#include <PCH.h>

#include <Core/Input/InputManager.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Foundation/Configuration/CVar.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/Time/Clock.h>
#include <GameEngine/Configuration/InputConfig.h>
#include <GameEngine/Console/Console.h>
#include <GameEngine/GameApplication/GameApplication.h>
#include <System/Window/Window.h>

extern ezCVarBool CVarShowFPS;

namespace
{
  const char* g_szInputSet = "GameApp";
  const char* g_szCloseAppAction = "CloseApp";
  const char* g_szShowConsole = "ShowConsole";
  const char* g_szShowFpsAction = "ShowFps";
  const char* g_szReloadResourcesAction = "ReloadResources";
  const char* g_szCaptureProfilingAction = "CaptureProfiling";
  const char* g_szTakeScreenshot = "TakeScreenshot";
} // namespace

void ezGameApplication::DoConfigureInput(bool bReinitialize)
{
  ezInputActionConfig config;

  config.m_sInputSlotTrigger[0] = ezInputSlot_KeyEscape;
  ezInputManager::SetInputActionConfig(g_szInputSet, g_szCloseAppAction, config, true);

  // the tilde has problematic behavior on keyboards where it is a hat (^)
  config.m_sInputSlotTrigger[0] = ezInputSlot_KeyF1;
  ezInputManager::SetInputActionConfig("Console", g_szShowConsole, config, true);

  // in the editor we cannot use F5, because that is already 'run application'
  // so we use F4 there, and it should be consistent here
  config.m_sInputSlotTrigger[0] = ezInputSlot_KeyF4;
  ezInputManager::SetInputActionConfig(g_szInputSet, g_szReloadResourcesAction, config, true);

  config.m_sInputSlotTrigger[0] = ezInputSlot_KeyF5;
  ezInputManager::SetInputActionConfig(g_szInputSet, g_szShowFpsAction, config, true);

  config.m_sInputSlotTrigger[0] = ezInputSlot_KeyF8;
  ezInputManager::SetInputActionConfig(g_szInputSet, g_szCaptureProfilingAction, config, true);

  config.m_sInputSlotTrigger[0] = ezInputSlot_KeyF12;
  ezInputManager::SetInputActionConfig(g_szInputSet, g_szTakeScreenshot, config, true);

  {
    ezFileReader file;
    if (file.Open(":project/InputConfig.ddl").Succeeded())
    {
      ezHybridArray<ezGameAppInputConfig, 32> InputActions;

      ezGameAppInputConfig::ReadFromDDL(file, InputActions);
      ezGameAppInputConfig::ApplyAll(InputActions);
    }
  }
}


void ezGameApplication::ProcessApplicationInput()
{
  // the show console command must be in the "Console" input set, because we are using that for exclusive input when the console is open
  if (ezInputManager::GetInputActionState("Console", g_szShowConsole) == ezKeyState::Pressed)
  {
    m_bShowConsole = !m_bShowConsole;

    if (m_bShowConsole)
      ezInputManager::SetExclusiveInputSet("Console");
    else
      ezInputManager::SetExclusiveInputSet("");
  }

  if (ezInputManager::GetInputActionState(g_szInputSet, g_szShowFpsAction) == ezKeyState::Pressed)
  {
    CVarShowFPS = !CVarShowFPS;
  }

  if (ezInputManager::GetInputActionState(g_szInputSet, g_szReloadResourcesAction) == ezKeyState::Pressed)
  {
    ezResourceManager::ReloadAllResources(false);
  }

  if (ezInputManager::GetInputActionState(g_szInputSet, g_szTakeScreenshot) == ezKeyState::Pressed)
  {
    TakeScreenshot();
  }

  if (ezInputManager::GetInputActionState(g_szInputSet, g_szCaptureProfilingAction) == ezKeyState::Pressed)
  {
    TakeProfilingCapture();
  }

  if (m_bShowConsole && m_pConsole)
    return;

  if (ezInputManager::GetInputActionState(g_szInputSet, g_szCloseAppAction) == ezKeyState::Pressed)
  {
    RequestQuit();
  }
}

void ezGameApplication::UpdateInput()
{
  ezInputManager::Update(ezClock::GetGlobalClock()->GetTimeDiff());

  ProcessApplicationInput();

  if (m_pConsole)
  {
    m_pConsole->DoDefaultInputHandling(m_bShowConsole);

    if (m_bShowConsole)
      return;
  }

  if (m_pGameState)
  {
    m_pGameState->ProcessInput();
  }
}



EZ_STATICLINK_FILE(GameEngine, GameEngine_GameApplication_Implementation_Input);
