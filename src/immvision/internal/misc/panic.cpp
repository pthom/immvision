#include "immvision/internal/misc/panic.h"
#include "immvision/internal/misc/debugbreak.h"
#include "immvision/internal/gl/gl_provider.h"

#include <iostream>

namespace ImmVision
{
    //
    // If you arrive here, it is likely that something very wrong happened
    //

    void Cleanup()
    {
        // Reset the caches
        ImmVision_GlProvider::ResetGlProvider();
    }

    void Panic_UnknownCause()
    {
        std::cerr << "ImmVision: Panic! " << "\n";
        // Put a debugger breakpoint, to help diagnose the issue
#ifndef NDEBUG
        debug_break();
#endif
        // Do some cleanup
        Cleanup();
        std::cerr << "ImmVision: Panic! => Did Cleanup" << "\n";
        // And die...
        std::cerr << "ImmVision: Panic! => will terminate!" << "\n";
        std::terminate();
    }

    void Panic(const std::exception& e)
    {
        std::cerr << "ImmVision: Panic! " << e.what() << "\n";
        // Put a debugger breakpoint, to help diagnose the issue
#ifndef NDEBUG
        debug_break();
#endif
        // Do some cleanup
        Cleanup();
        std::cerr << "ImmVision: Panic! => Did Cleanup" << "\n";
        // And rethrow
        std::cerr << "ImmVision: Panic! => re-throw!" << "\n";
        throw;
    }
} // namespace ImmVision