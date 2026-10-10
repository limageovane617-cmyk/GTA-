
//
// Created by mrxenginner on 13/07/2025.
//

#ifndef REVC_STACKTRACE_H
#define REVC_STACKTRACE_H

#if defined(ANDROID)

#include <dlfcn.h>
#include <execinfo.h>
#include <unwind.h>
#include <stdint.h>
#include <ucontext.h>

extern uintptr_t g_libREVC;

// ARM de 32 bits
#if defined(__arm__) && !defined(__aarch64__)

#define PRINT_CRASH_STATES(context) do { \
    Logger::CrashLog("register states:"); \
    Logger::CrashLog( \
        "r0: 0x%08lx, r1: 0x%08lx, r2: 0x%08lx, r3: 0x%08lx", \
        (unsigned long)(context)->uc_mcontext.arm_r0, \
        (unsigned long)(context)->uc_mcontext.arm_r1, \
        (unsigned long)(context)->uc_mcontext.arm_r2, \
        (unsigned long)(context)->uc_mcontext.arm_r3); \
    Logger::CrashLog( \
        "r4: 0x%08lx, r5: 0x%08lx, r6: 0x%08lx, r7: 0x%08lx", \
        (unsigned long)(context)->uc_mcontext.arm_r4, \
        (unsigned long)(context)->uc_mcontext.arm_r5, \
        (unsigned long)(context)->uc_mcontext.arm_r6, \
        (unsigned long)(context)->uc_mcontext.arm_r7); \
    Logger::CrashLog( \
        "r8: 0x%08lx, r9: 0x%08lx, sl: 0x%08lx, fp: 0x%08lx", \
        (unsigned long)(context)->uc_mcontext.arm_r8, \
        (unsigned long)(context)->uc_mcontext.arm_r9, \
        (unsigned long)(context)->uc_mcontext.arm_r10, \
        (unsigned long)(context)->uc_mcontext.arm_fp); \
    Logger::CrashLog( \
        "ip: 0x%08lx, sp: 0x%08lx, lr: 0x%08lx, pc: 0x%08lx", \
        (unsigned long)(context)->uc_mcontext.arm_ip, \
        (unsigned long)(context)->uc_mcontext.arm_sp, \
        (unsigned long)(context)->uc_mcontext.arm_lr, \
        (unsigned long)(context)->uc_mcontext.arm_pc); \
    Logger::CrashLog("1: libreVC.so + 0x%08lx", \
        (unsigned long)((context)->uc_mcontext.arm_pc - g_libREVC)); \
    Logger::CrashLog("2: libreVC.so + 0x%08lx", \
        (unsigned long)((context)->uc_mcontext.arm_lr - g_libREVC)); \
} while (0)

// ARM de 64 bits
#elif defined(__aarch64__)

#define PRINT_CRASH_STATES(context) do { \
    Logger::CrashLog("register states:"); \
    Logger::CrashLog("1: libreVC.so + 0x%llx", \
        (unsigned long long)((context)->uc_mcontext.pc - g_libREVC)); \
    Logger::CrashLog("2: libreVC.so + 0x%llx", \
        (unsigned long long)((context)->uc_mcontext.regs[30] - g_libREVC)); \
} while (0)

// Arquitetura não reconhecida
#else

#define PRINT_CRASH_STATES(context) do { \
    (void)(context); \
    Logger::CrashLog( \
        "Crash register details unavailable for this architecture."); \
} while (0)

#endif

class CStackTrace
{
public:
    static void printBacktrace()
    {
        Logger::CrashLog("------------ START BACKTRACE ------------");
        Logger::CrashLog(" ");
        PrintStackTrace();
    }

private:
    static _Unwind_Reason_Code TraceFunction(
        _Unwind_Context* context,
        void* arg)
    {
        (void)arg;

        uintptr_t pc = (uintptr_t)_Unwind_GetIP(context);
        Dl_info info = {};

        if (dladdr(reinterpret_cast<void*>(pc), &info)
            && info.dli_sname != nullptr)
        {
            Logger::CrashLog(
                "[adr: %p reVC: %p] %s",
                reinterpret_cast<void*>(pc),
                reinterpret_cast<void*>(pc - g_libREVC),
                info.dli_sname);
        }
        else
        {
            Logger::CrashLog(
                "[adr: %p reVC: %p] name not found",
                reinterpret_cast<void*>(pc),
                reinterpret_cast<void*>(pc - g_libREVC));
        }

        return _URC_NO_REASON;
    }

    static void PrintStackTrace()
    {
        _Unwind_Backtrace(TraceFunction, nullptr);
    }
};

#endif // defined(ANDROID)

#endif // REVC_STACKTRACE_H
