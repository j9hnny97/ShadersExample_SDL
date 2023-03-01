#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <string>
#include <Windows.h>

static std::string makePath(std::string const& filePath)
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string executablePathname = buffer;

    std::size_t const found = executablePathname.find_last_of("/\\");
    executablePathname = executablePathname.substr(0, found);

    executablePathname.append("/../../../../res/");
    executablePathname.append(filePath);

    return executablePathname;
}

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#endif
