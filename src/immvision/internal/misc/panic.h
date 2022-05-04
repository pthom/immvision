#pragma once
#include <exception>

namespace ImmVision
{
    void Panic(const std::exception& e);
    void Panic_UnknownCause();
} // namespace ImmVision