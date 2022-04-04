#pragma once
#include <string>
#include <memory>

namespace ImmVision
{
    class SingleInstanceAppPImpl;


    // SingleInstanceApp: utility class in order to make an application "single instance",
    // i.e any attempt to open it again will fail, but can let the already opened one to
    // brings its window to the top.
    class SingleInstanceApp
    {
    public:
        SingleInstanceApp(const std::string& lockName);
        ~SingleInstanceApp();

        bool RunSingleInstance();
        bool WasPinged();
    private:
        std::unique_ptr<SingleInstanceAppPImpl> mPImpl;
    };
}