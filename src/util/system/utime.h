#pragma once

#include <ydb-cpp-sdk/util/system/defaults.h>

int TouchFile(const char* filePath);
int SetModTime(const char* filePath, time_t modtime, time_t actime);
