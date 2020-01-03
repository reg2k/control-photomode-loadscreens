#pragma once

#include "httplib/httplib.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace PhotoServer
{
    // Returns the port number that the server is bound to.
    int Start(std::string photoDir);
};