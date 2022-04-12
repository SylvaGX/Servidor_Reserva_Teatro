#pragma once
#include <stdio.h>
#include <windows.h>
#include <time.h>

static HANDLE MutexLog;

static char LogFileName[] = "logFile.txt";

void InfoLog(const char* msg);
void WarningLog(const char* msg);
void ErrorLog(const char* msg);
