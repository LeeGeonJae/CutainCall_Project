#include "pch.h"
#include "YAMLParser.h"

// yaml-cpp
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#ifdef _DEBUG
#pragma comment(lib, "yaml-cppd.lib")
#else
#pragma comment(lib, "yaml-cpp.lib")
#endif

