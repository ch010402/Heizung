//ch010402 29.12.2020
//Logger class / singleton 
//future tasks 
// [ ] set file yes/no
// [ ] set cosle output yes/no aka silent
// [ ] optimize performance (allocations)

#include <iostream>
#include <string>
#include <string_view>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "Log.h"

uint32_t s_AllocCount = 0;

void* operator new(size_t size)
{
	s_AllocCount++;
	return malloc(size);
}

Log Log::s_Instance;

Log::Level m_LogLevel = Log::Level::LevelInfo;

void Log::I_Setup(const char* argv0, Log::Level level)
{
	m_StartFileName = argv0;
	m_LogLevel = level;
	Initialize();
}

void Log::I_Error(const char* message)
{
	if (m_LogLevel >= Log::Level::LevelError)
	{
		Timestamp();
		std::cout << Log::m_Timestamp << " [ERROR]: " << message << std::endl;
		WriteFile(message, "[ERROR]:");
	}
};

void Log::I_Warn(const char* message)
{
	if (m_LogLevel >= Log::Level::LevelWarning)
	{
		Timestamp();
		std::cout << Log::m_Timestamp << " [WARNING]: " << message << std::endl;
		WriteFile(message, "[WARNING]:");
	}
};

void Log::I_Info(const char* message)
{
	if (m_LogLevel >= Log::Level::LevelInfo)
	{
		Timestamp();
		std::cout << Log::m_Timestamp << " [INFO]: " << message << std::endl;
		WriteFile(message, "[INFO]:");
	}
}

void Log::CreateFileName()
{
	std::string FullFileName = m_StartFileName;
	size_t FileNamePos = FullFileName.find_last_of("\\");
	if (FileNamePos == std::string::npos)
	{
		std::cout << "in linux ";
		FileNamePos = FullFileName.find_last_of("/");
		if (FileNamePos == std::string::npos)
		{
			std::cout << "filename not found, use noname instead" << std::endl;
			m_FileName = "noname";
		}
	}
	size_t DotPos = FullFileName.find_first_of(".");

	std::string FileName = FullFileName.substr(FileNamePos + 1,DotPos - FileNamePos -1 );

	m_FileName = FileName + "_" + GetDate() + ".log";

	CheckFileName(m_FileName);
}

void Log::CheckFileName(std::string& FileName)
{
	int i = 1;
	std::ifstream file;
	bool check = true;
	while (check)
	{
		file.open(FileName, std::ios::binary | std::ios::ate);
		if (file.is_open()) // check if file exists when yes check size 
		{
			if (file.tellg() > 2*1048576) //2MB
			{
				std::string index = std::to_string(i);
				int minus = FileName.length() - FileName.find_last_of("-");
				if (i > 1) FileName = FileName.erase(FileName.size() - minus);
				else FileName = FileName.erase(FileName.size() - 4);
				FileName = FileName + "-" + index + ".log";
				i++;
			}
			else check = false;
		}
		else
		{
			check = false;
		}

		file.close();
		if (m_FileName != FileName) m_FileName = FileName;
	}

}

std::string Log::GetDate()
{
	auto now = std::chrono::system_clock::now();
	std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);

	std::tm buffer;
	#ifdef defined __linux__
	localtime_r(&in_time_t, &buffer);
	#elif defined _WIN32 || defined _WIN64
	localtime_s(&buffer, &in_time_t);
	#endif

	std::stringstream ss;
	ss << std::put_time(&buffer, "%Y-%m-%d");
	return ss.str();
}

void Log::Timestamp()
{
	auto now = std::chrono::system_clock::now();
	std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);

	std::tm buffer;
	#ifdef defined __linux__
	localtime_r(&in_time_t, &buffer);
	#elif defined _WIN32 || defined _WIN64
	localtime_s(&buffer, &in_time_t);
	#endif

	std::stringstream ss;
	ss << std::put_time(&buffer, "%Y-%m-%d %X");
	Log::m_Timestamp =  ss.str();
}

void Log::WriteFile(const char* message , const char* level)
{
	if (!m_inintialized) Initialize();

	CheckFileName(m_FileName);

	std::fstream LogFile;
	LogFile.open(m_FileName, std::ios::app);
	LogFile << Log::m_Timestamp << " " << level << " " << message << "\n";

	if (LogFile.tellp() > 1000000) m_inintialized = false;
	LogFile.close();
	
	//std::cout << "made " << s_AllocCount << " allocations" << std::endl;

}

void Log::Initialize()
{
	Log::CreateFileName();
		
	m_inintialized = true;
}

void Log::I_PrintInformation()
{
	std::cout << "**** Information ****" << std::endl;
	
	std::cout << "we are running on -> ";
	#ifdef TARGET_OS_MAC
			std::cout << "osx";
	#elif defined __linux__
			std::cout << "linux";
	#elif defined _WIN32 || defined _WIN64
			std::cout << "windows";
	#else
	#error "unknown platform"
	#endif
	std::cout << std::endl;

	std::cout << "program started from file -> "<< m_StartFileName << std::endl;
	std::cout << "logelevel set to -> ";
	switch (static_cast<std::underlying_type<Level>::type>(m_LogLevel))
	{
	case 0: std::cout << "Error" << std::endl; break;
	case 1: std::cout << "Warning" << std::endl; break;
	case 2: std::cout << "Info" << std::endl; break;
	}
	std::cout << "create logfile name -> " << m_FileName << std::endl;
	Timestamp();
	std::cout << "current timestamp -> " << m_Timestamp << std::endl;

	std::cout << m_FileName << std::endl;
	std::cout << "********************" << std::endl;
}
