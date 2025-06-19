cmake_minimum_required(VERSION 3.22)

project(
	TemplatePlugin
	VERSION 1.0.0
	DESCRIPTION ""
	LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
add_compile_definitions(_DISABLE_EXTENDED_ALIGNED_STORAGE)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# #######################################################################################################################
# # Add CMake features
# #######################################################################################################################
include(XSEPlugin)

# #######################################################################################################################
# # Find dependencies
# #######################################################################################################################
find_package(xbyak CONFIG REQUIRED)
find_package(toml11 CONFIG REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_path(CLIB_UTIL_INCLUDE_DIRS "ClibUtil/Utils.hpp")

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${CLIB_UTIL_INCLUDE_DIRS}
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	xbyak::xbyak
	toml11::toml11
	nlohmann_json::nlohmann_json
)


