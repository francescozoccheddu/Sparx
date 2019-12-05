#pragma once

#include <string>
#include <Windows.h>

namespace Sparx::Error
{

	std::string GetErrorMessage (DWORD error);

	void PreQuit ();

	void Log (const std::string& message);

	void Post (const std::string& message);

	void Post (const std::string& message, DWORD error);

	void PostLast (const std::string& message);

}