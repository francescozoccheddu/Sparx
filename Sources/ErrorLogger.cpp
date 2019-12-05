#include "ErrorLogger.hpp"

#include <sstream>
#include <iostream>
#include <io.h>
#include <fcntl.h>


std::string Sparx::Error::GetErrorMessage (DWORD _error)
{
	LPSTR msgPtr;
	const DWORD msgLen { FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, _error, 0, reinterpret_cast<LPSTR>(&msgPtr), 0, nullptr) };
	std::string msg { msgPtr, msgPtr + msgLen };
	LocalFree (msgPtr);
	return msg;
}

void Sparx::Error::PreQuit ()
{
#if _DEBUG
	if (GetConsoleWindow ())
	{
		std::cout << "Application has been terminated" << std::endl;
		Sleep (INFINITE);
	}
#endif
}

void Sparx::Error::Log (const std::string& _message)
{
	if (IsDebuggerPresent ())
	{
		OutputDebugStringA (_message.c_str ());
		OutputDebugStringA ("\n");
	}
	else
	{
		if (!GetConsoleWindow ())
		{
			AllocConsole ();
			FILE* out;
			freopen_s (&out, "CONOUT$", "w", stdout);
			std::cout << SOLUTIONNAME << " - Log console" << std::endl << std::endl;
		}
		std::cout << _message << std::endl;
	}
}

void Sparx::Error::Post (const std::string& _message)
{
#ifdef _DEBUG
	Log (_message);
	if (IsDebuggerPresent ())
	{
		DebugBreak ();
	}
	else
#endif
	{
		if (MessageBoxA (nullptr, _message.c_str (), SOLUTIONNAME "Error happened", MB_OKCANCEL | MB_ICONWARNING) != IDOK)
		{
#ifdef _DEBUG
			PreQuit ();
#endif
			ExitProcess (0);
		}
	}
}

void Sparx::Error::Post (const std::string& _message, DWORD _error)
{
	std::stringstream ss;
	ss << _message;
	ss << "\n";
	ss << GetErrorMessage (_error);
	Post (ss.str ().c_str ());
}

void Sparx::Error::PostLast (const std::string& _message)
{
	Post (_message, GetLastError ());
}