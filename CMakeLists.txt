cmake_minimum_required(VERSION 2.8)

project(lv2lint.lv2)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR})

set(CMAKE_C_FLAGS "-std=gnu11 -Wextra -Wno-unused-parameter -ffast-math -fvisibility=hidden ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS "-Wshadow -Wimplicit-function-declaration -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes ${CMAKE_C_FLAGS}")
add_definitions("-D_GNU_SOURCE=1") # asprintf

file(STRINGS "VERSION" LV2LINT_VERSION)
string(REPLACE "." ";" VERSION_LIST ${LV2LINT_VERSION})
list(GET VERSION_LIST 0 LV2LINT_MAJOR_VERSION)
list(GET VERSION_LIST 1 LV2LINT_MINOR_VERSION)
list(GET VERSION_LIST 2 LV2LINT_MICRO_VERSION)
add_definitions("-DLV2LINT_VERSION=\"${LV2LINT_VERSION}\"")

find_package(PkgConfig) # ${PKG_CONFIG_FOUND}

pkg_search_module(LV2 REQUIRED lv2>=1.10)
include_directories(${LV2_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LV2_LDFLAGS})

pkg_search_module(LILV REQUIRED lilv-0>=0.20.0)
include_directories(${LILV_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LILV_LDFLAGS})
if((${LILV_VERSION} VERSION_EQUAL "0.22.0") OR (${LILV_VERSION} VERSION_GREATER "0.22.0"))
	add_definitions("-DLILV_0_22")
endif()

add_executable(lv2lint
	lv2lint.c)
set_target_properties(lv2lint PROPERTIES INTERPROCEDURAL_OPTIMIZATION true) # -flto
target_link_libraries(lv2lint ${LIBS})
install(TARGETS lv2lint DESTINATION bin)

include(CTest)

if(${BUILD_TESTING})
	if(NOT WIN32)
		add_test(NAME LV2-Lint COMMAND lv2lint "http://open-music-kontrollers.ch/lv2/moony#a1xa1")
	else()
		add_test(NAME LV2-Lint COMMAND ${WINE} lv2lint "http://open-music-kontrollers.ch/lv2/moony#a1xa1")
	endif()
endif()
