#pragma once

#include <functional>
#include <thread>
#include <vector>

namespace ImmVision
{
    // Simple parallel_for: splits [begin, end) across threads, one chunk per thread.
    // Falls back to sequential on Emscripten or when only 1 thread is available.
    inline void parallel_for(int begin, int end, const std::function<void(int)>& body)
    {
        if (begin >= end)
            return;

#ifdef __EMSCRIPTEN__
        int nThreads = 1;
#else
        int hw = (int)std::thread::hardware_concurrency();
        int nThreads = (hw > 2) ? hw - 1 : 1;  // leave one core spare
#endif

        int count = end - begin;
        if (nThreads <= 1 || count <= 1)
        {
            for (int i = begin; i < end; ++i)
                body(i);
            return;
        }

        if (nThreads > count)
            nThreads = count;

        std::vector<std::thread> threads(nThreads);
        int chunkSize = count / nThreads;
        int remainder = count % nThreads;

        int offset = begin;
        for (int t = 0; t < nThreads; ++t)
        {
            int chunkBegin = offset;
            int chunkEnd = offset + chunkSize + (t < remainder ? 1 : 0);
            offset = chunkEnd;
            threads[t] = std::thread([chunkBegin, chunkEnd, &body]() {
                for (int i = chunkBegin; i < chunkEnd; ++i)
                    body(i);
            });
        }

        for (auto& t : threads)
            t.join();
    }

} // namespace ImmVision
