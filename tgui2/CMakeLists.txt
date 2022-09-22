cmake_minimum_required(VERSION 2.6)
project(tgui2 CXX)

OPTION(DEBUG "Build debug version" off)
OPTION(WANT_WIDGETS "Want some default widgets" on)

# Set USER_INCLUDE_PATH as may be necessary
# e.g., -DUSER_INCLUDE_PATH=/home/trent/local/include

set(USER_INCLUDE_PATH "" CACHE FILEPATH "Any extra include directories you want")
set(USER_LIBRARY_PATH "" CACHE FILEPATH "Any extra library directories you want")

include_directories(${USER_INCLUDE_PATH})
link_directories(${USER_LIBRARY_PATH})

set(SOURCES tgui2.cpp)

if(WANT_WIDGETS)
	list(APPEND SOURCES tgui2_widgets.cpp)
endif()

if(DEBUG)
	add_library(tgui2-debug SHARED ${SOURCES})
	add_library(tgui2-debug-static STATIC ${SOURCES})
endif()

add_library(tgui2 SHARED ${SOURCES})
add_library(tgui2-static STATIC ${SOURCES})

if(WIN32)
	if(DEBUG)
		set_target_properties(tgui2-debug PROPERTIES COMPILE_FLAGS "/DTGUI2_BUILD /DALLEGRO_NO_MAGIC_MAIN /MDd")
		target_link_libraries(tgui2-debug allegro_monolith)
		set_target_properties(tgui2-debug-static PROPERTIES COMPILE_FLAGS "/DTGUI2_BUILD /DTGUI2_STATIC /MTd /Zl")
	endif()
	set_target_properties(tgui2 PROPERTIES COMPILE_FLAGS "/DTGUI2_BUILD /DALLEGRO_NO_MAGIC_MAIN /MD")
	target_link_libraries(tgui2 allegro_monolith)
	set_target_properties(tgui2-static PROPERTIES COMPILE_FLAGS "/DTGUI2_BUILD /DTGUI2_STATIC /MT /Zl")
else()
	if(DEBUG)
		set_target_properties(tgui2-debug PROPERTIES COMPILE_FLAGS "-O3")
		set_target_properties(tgui2-debug-static PROPERTIES COMPILE_FLAGS "-O3")
	endif()
	set_target_properties(tgui2 PROPERTIES COMPILE_FLAGS "-O3")
	set_target_properties(tgui2-static PROPERTIES COMPILE_FLAGS "-O3")
endif()
