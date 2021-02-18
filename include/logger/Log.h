//ch010402 29.12.2020
//header file to logger class / singleton 

#pragma once


class Log
{
public:
	Log(const Log&) = delete;

	static Log& Get()
	{
		return s_Instance;
	}

	enum class Level
	{
		LevelError = 0, LevelWarning, LevelInfo, Debug
	};

private:
	Log() {}
	static Log s_Instance;
	Level m_LogLevel = Level::LevelInfo;
	const char* m_StartFileName;
	bool m_inintialized = false;
	std::string m_FileName;
	std::string m_Timestamp = "empty";

public:
	static void Setup(const char* argv0, Log::Level level) { return Get().I_Setup(argv0, level); };
	static void Error(const char* message) { return Get().I_Error(message); };
	static void Error(std::string message) { return Get().I_Error(message.c_str()); };
	static void Warning(const char* message) { return Get().I_Warn(message); };
	static void Warning(std::string message) { return Get().I_Warn(message.c_str()); };
	static void Info(const char* message) { return Get().I_Info(message); };
	static void Info(std::string message) { return Get().I_Info(message.c_str()); };
	static void Debug(const char* message) { return Get().I_Debug(message); };
	static void Debug(std::string message) { return Get().I_Debug(message.c_str()); }
	static void PrintInformation() { return Get().I_PrintInformation(); };

private:
	void I_Error(const char* message);
	void I_Warn(const char* message);
	void I_Info(const char* message);
	void I_Debug(const char* message);
	void I_Setup(const char* argv0, Log::Level level);
	void I_PrintInformation();
	void Initialize();
	void CreateFileName();
	void CheckFileName(std::string& FileName);
	std::string GetDate();
	void Timestamp();
	void WriteFile(const char* message,const char* level);
};