#pragma once
#include <iostream>
#include <fstream>
#include <windows.h>
#include <time.h>
#include <mutex>

class Log {
private:
	static std::mutex m;
	static std::mutex& getMutex() { return m; }
	static std::string LogFileName;

public:
	static void InfoLog(std::string msg);
	static void WarningLog(std::string msg);
	static void ErrorLog(std::string msg);
};
