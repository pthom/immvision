#include <pybind11/embed.h>
#include <iostream>
#include <filesystem>

namespace py = pybind11;


std::string THIS_DIR = std::filesystem::path(__FILE__).parent_path().string();


std::string string_replace(const std::string& src, const std::string& target, const std::string& repl)
{
    if (src.empty() || target.empty())
        return src;

    std::string result = src;
    size_t idx = 0;
    while(true)
    {
        idx = result.find( target, idx);
        if (idx == std::string::npos)
            break;
        result.replace(idx, target.length(), repl);
        idx += repl.length();
    }
    return result;
}


void use_venv_python()
{
    std::string venv_dir = THIS_DIR + "/../../venv";
    std::string python_program = venv_dir + "/bin/python";
    std::wstring python_program_wstring(python_program.begin(), python_program.end());
    Py_SetProgramName(python_program_wstring.c_str());
}


int main()
{
    use_venv_python();

    py::scoped_interpreter guard{};

    std::string cmd = R"(
import sys
sys.path.append("THIS_DIR")
import pybind_native_debug
    )";
    cmd = string_replace(cmd, "THIS_DIR", THIS_DIR);
    py::exec(cmd);
}
