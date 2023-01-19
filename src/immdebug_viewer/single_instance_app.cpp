#include "immdebug_viewer/single_instance_app.h"

#include <string>
#include <thread>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <future>



namespace ImmVision
{


    class SingleInstanceAppPImpl
    {
    public:
        SingleInstanceAppPImpl(const std::string& lockName) : mLockName(lockName){}
        ~SingleInstanceAppPImpl() { mExit = true; }

        bool RunSingleInstance()
        {
            if (HasOtherInstance())
                return false;
            mFutureResult = std::async(std::launch::async, [this](){ PingLoop(); });
            return true;
        }

        bool WasPinged()
        {
            if (mPingReceived)
            {
                mPingReceived = false;
                return true;
            }
            return false;
        }

    private:
        bool HasOtherInstance()
        {
            using namespace std::literals;

            if (HasPingFile())
            {
                std::cout << "Ooops : stale ping file!\n";
                RemovePingFile();
                std::this_thread::sleep_for(100ms);
            }

            CreatePingFile();
            std::this_thread::sleep_for(120ms);
            if ( ! HasPingFile())
            {
                std::cout << "Other instance already launched!\n";
                return true;
            }
            else
            {
                // Master process not answering
                std::cout << "First instance!\n";
                RemovePingFile();
                return false;
            }
        }

        void AnswerPings()
        {
            if (std::filesystem::is_regular_file(PingFilename()))
            {
                std::cout << "Answering ping!\n";
                mPingReceived = true;
                std::filesystem::remove(PingFilename());
            }
        }

        void PingLoop()
        {
            using namespace std::literals;
            while(!mExit)
            {
                AnswerPings();
                std::this_thread::sleep_for(60ms);
            }
        }

        bool HasPingFile() { return std::filesystem::is_regular_file(PingFilename()); }
        void CreatePingFile() {  std::ofstream os(PingFilename()); os << "Lock"; }
        void RemovePingFile() { std::filesystem::remove(PingFilename()); }
        std::string PingFilename()
        {
            return std::filesystem::temp_directory_path().string() + "/" + mLockName + ".ping";
        }

        std::string mLockName;
        std::atomic<bool> mExit = false;
        std::atomic<bool> mPingReceived = false;
        std::future<void> mFutureResult;
    };

    SingleInstanceApp::SingleInstanceApp(const std::string& lockName)
        : mPImpl(std::make_unique<SingleInstanceAppPImpl>(lockName)){}
    SingleInstanceApp::~SingleInstanceApp() = default;
    bool SingleInstanceApp::RunSingleInstance() {return mPImpl->RunSingleInstance(); }
    bool SingleInstanceApp::WasPinged() { return mPImpl->WasPinged(); }

} // namespace ImmVision
