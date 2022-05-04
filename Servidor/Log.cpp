#include "Log.h"

std::mutex Log::m {};
std::string Log::LogFileName{ "logFile.txt" };

void Log::InfoLog(std::string msg)
{
    std::unique_lock<std::mutex> lck(getMutex());

    std::ofstream log;

    log.open(LogFileName, std::ios_base::app);

    if (log)
    {
        time_t now;
        time(&now);
        log << ctime(&now) << " [Info]: " << msg << "\n";
        printf("\033[0;37m");
        std::cout << ctime(&now) << " [Info]: " << msg << "\n";
        printf("\033[0m");

        log.close();
    }

}

void Log::WarningLog(std::string msg)
{
    std::unique_lock<std::mutex> lck(getMutex());

    std::ofstream log;

    log.open(LogFileName, std::ios_base::app);

    if (log)
    {
        time_t now;
        time(&now);
        log << ctime(&now) << " [Warning]: " << msg << "\n";
        printf("\033[0;33m");
        std::cout << ctime(&now) << " [Warning]: " << msg << "\n";
        printf("\033[0m");

        log.close();
    }
}

void Log::ErrorLog(std::string msg)
{
    std::unique_lock<std::mutex> lck(getMutex());

    std::ofstream log;

    log.open(LogFileName, std::ios_base::app);

    if (log)
    {
        time_t now;
        time(&now);
        log << ctime(&now) << " [Error]: " << msg << "\n";
        printf("\033[0;31m");
        std::cout << ctime(&now) << " [Error]: " << msg << "\n";
        printf("\033[0m");

        log.close();
    }

}
