#include "Log.h"

void InfoLog(const char* msg)
{
    DWORD resultMutexCidades = WaitForSingleObject(
        MutexLog,    // handle to mutex
        INFINITE);  // no time-out interval

    FILE* log;

    log = fopen(LogFileName, "a");

    if (log != NULL)
    {
        time_t now;
        time(&now);
        fprintf(log, "%s [%s]: %s\n", ctime(&now), "Info", msg);
        printf("\033[0;37m");
        printf("%s [%s]: %s\n", ctime(&now), "Info", msg);
        printf("\033[0m");

        fclose(log);
    }

    if (!ReleaseMutex(MutexLog)) {
        //Lidar com erro
    }
}

void WarningLog(const char* msg)
{
    DWORD resultMutexCidades = WaitForSingleObject(
        MutexLog,    // handle to mutex
        INFINITE);  // no time-out interval
    FILE* log;

    log = fopen(LogFileName, "a");

    if (log != NULL)
    {
        time_t now;
        time(&now);
        fprintf(log, "%s [%s]: %s\n", ctime(&now), "Warning", msg);
        printf("\033[0;33m");
        printf("%s [%s]: %s\n", ctime(&now), "Warning", msg);
        printf("\033[0m");

        fclose(log);
    }

    if (!ReleaseMutex(MutexLog)) {
        //Lidar com erro
    }
}

void ErrorLog(const char* msg)
{
    DWORD resultMutexCidades = WaitForSingleObject(
        MutexLog,    // handle to mutex
        INFINITE);  // no time-out interval
    FILE* log;

    log = fopen(LogFileName, "a");

    if (log != NULL)
    {
        time_t now;
        time(&now);
        fprintf(log, "%s [%s]: %s\n", ctime(&now), "Error", msg);
        printf("\033[0;31m");
        printf("%s [%s]: %s\n", ctime(&now), "Error", msg);
        printf("\033[0m");

        fclose(log);
    }

    if (!ReleaseMutex(MutexLog)) {
        //Lidar com erro
    }
}
