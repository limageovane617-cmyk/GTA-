
//
// Created by mrxenginner on 11/05/2025.
//

#if defined(ANDROID)

#include "SignalHandler.h"
#include "../logger/log.h"
#include "StackTrace.h"

#include <csignal>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <ucontext.h>

extern int16 g_usLastProcessedModelIndexAutomobile;
extern int g_iLastProcessedModelIndexAutoEnt;

extern int g_iLastProcessedSkinCollision;
extern int g_iLastProcessedEntityCollision;
extern char lastFile[123];
extern int g_iLastRenderedObject;
extern int lastNvEvent;
extern CVector lastPos;

char g_iLastBlock[123];
char streamimgState[255];

namespace CrashHandler
{
    static struct sigaction oldHandlers[4];

    void PrintBuildCrashInfo()
    {
        time_t currentTime = time(nullptr);
        tm* timeInfo = localtime(&currentTime);

        if (timeInfo != nullptr)
        {
            Logger::CrashLog(
                "Crash time: %d/%d/%d %d:%d:%d",
                timeInfo->tm_mday,
                timeInfo->tm_mon + 1,
                timeInfo->tm_year + 1900,
                timeInfo->tm_hour,
                timeInfo->tm_min,
                timeInfo->tm_sec);
        }
        else
        {
            Logger::CrashLog("Crash time unavailable");
        }

#if defined(__arm__) && !defined(__aarch64__)
        const char* abi = "armeabi-v7a";
#elif defined(__aarch64__)
        const char* abi = "arm64-v8a";
#else
        const char* abi = "unknown";
#endif

        Logger::CrashLog(
            "Build times: %s %s. ABI: %s",
            __TIME__,
            __DATE__,
            abi);

        Logger::CrashLog(
            "Last processed auto and entity: %d %d",
            (int)g_usLastProcessedModelIndexAutomobile,
            g_iLastProcessedModelIndexAutoEnt);

        Logger::CrashLog(
            "Last rendered object: %d",
            g_iLastRenderedObject);
    }

    void SignalHandler(int signum, siginfo_t* info, void* contextPtr)
    {
        auto* context = static_cast<ucontext_t*>(contextPtr);

        struct sigaction* oldHandler = nullptr;
        const char* signalName = nullptr;

        switch (signum)
        {
            case SIGSEGV:
                oldHandler = &oldHandlers[0];
                signalName = "SIGSEGV";
                Logger::CrashLog(" ");
                break;

            case SIGABRT:
                oldHandler = &oldHandlers[1];
                signalName = "SIGABRT";
                Logger::CrashLog(" ");
                break;

            case SIGFPE:
                oldHandler = &oldHandlers[2];
                signalName = "SIGFPE";
                break;

            case SIGBUS:
                oldHandler = &oldHandlers[3];
                signalName = "SIGBUS";
                break;

            default:
                Logger::CrashLog("Unhandled signal: %d", signum);
                return;
        }

        PrintBuildCrashInfo();

        if (info != nullptr)
        {
            Logger::CrashLog(
                "%s | Fault address: %p",
                signalName,
                info->si_addr);
        }
        else
        {
            Logger::CrashLog(
                "%s | Fault address unavailable",
                signalName);
        }

        PRINT_CRASH_STATES(context);
        CStackTrace::printBacktrace();

        // Evita chamar um manipulador anterior como se todos
        // os manipuladores registrados usassem sa_sigaction.
        (void)oldHandler;
    }

    void SetupSignalHandlers()
    {
        struct
        {
            int signal;
            int index;
        } signals[] = {
            { SIGSEGV, 0 },
            { SIGABRT, 1 },
            { SIGFPE,  2 },
            { SIGBUS,  3 },
        };

        for (const auto& s : signals)
        {
            struct sigaction act {};
            act.sa_sigaction = SignalHandler;
            sigemptyset(&act.sa_mask);
            act.sa_flags = SA_SIGINFO;

            sigaction(
                s.signal,
                &act,
                &oldHandlers[s.index]);
        }
    }
}

#endif // defined(ANDROID)
