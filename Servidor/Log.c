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
        printf("%s [%s]: %s\n", ctime(&now), "Info", msg);

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
        printf("%s [%s]: %s\n", ctime(&now), "Warning", msg);

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
        printf("%s [%s]: %s\n", ctime(&now), "Error", msg);

        fclose(log);
    }

    if (!ReleaseMutex(MutexLog)) {
        //Lidar com erro
    }
    
}
