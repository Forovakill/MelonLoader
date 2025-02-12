#include "Assertion.h"
#include "../Core.h"
#include "Debug.h"
#include "Logging/Logger.h"
#include "Console.h"
#include "../Managers/Game.h"
#include <string>
#include <iostream>

bool Assertion::ShouldContinue = true;

void Assertion::ThrowInternalFailure(const char* msg)
{
	if (ShouldContinue)
	{
		ShouldContinue = false;
		std::string timestamp = Logger::GetTimestamp();
		Logger::LogFile << "[" << timestamp << "] [INTERNAL FAILURE] " << msg << std::endl;
		bool should_print_debug_info = (!Logger::LogFile.coss.is_open() || Debug::Enabled);
		if (should_print_debug_info)
		{
			std::cout
				<< Console::ColorToAnsi(Console::Color::Red)
				<< "["
				<< timestamp
				<< "] [INTERNAL FAILURE] "
				<< msg
				<< std::endl
				<< Console::ColorToAnsi(Console::Color::Gray, false);
			MessageBoxA(NULL, msg, "MelonLoader - INTERNAL FAILURE", MB_OK | MB_ICONERROR);
		}
		else
		{
			Console::Close();
			MessageBoxA(NULL, ("Please upload the log file \""
				+ std::string(Game::BasePath)
				+ "\\MelonLoader\\" 
				+ Logger::LatestLogFileName 
				+ Logger::FileExtension 
				+ "\" when requesting support.").c_str(), "MelonLoader - INTERNAL FAILURE!", MB_OK | MB_ICONERROR);
		}
		Core::KillCurrentProcess();
	}
}

FARPROC Assertion::GetExport(HMODULE mod, const char* export_name, bool internalfailure)
{
	if (!ShouldContinue)
		return NULL;
	Debug::Msg(export_name);
	FARPROC returnval = GetProcAddress(mod, export_name);
	if (internalfailure && (returnval == NULL))
		ThrowInternalFailure((std::string("Failed to GetExport ( ") + export_name + " )").c_str());
	return returnval;
}