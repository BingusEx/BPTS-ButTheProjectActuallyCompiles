cmake_minimum_required(VERSION 3.22)

project(
	BetterThirdPersonSelection
	VERSION 0.8.7
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
include(FetchContent)


FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

# #######################################################################################################################
# # Find dependencies
# #######################################################################################################################
find_package(xbyak CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_path(CLIB_UTIL_INCLUDE_DIRS "ClibUtil/Utils.hpp")

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${CLIB_UTIL_INCLUDE_DIRS}
	${jsoncpp_includes}
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	xbyak::xbyak
	tomlplusplus::tomlplusplus
	magic_enum::magic_enum
)


