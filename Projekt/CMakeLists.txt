# CMakeList.txt : CMake project for Projekt, include source and define
# project specific logic here.
#

# Setting include and source directories' variables

set(IMGUI_DIR Dependencies/imgui)
set(OWN_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/include)

# Own include and source files 

file (GLOB SRC source/*.cpp)
file (GLOB IMGUI_SRC Dependencies/imgui/*.cpp)

add_executable (Projekt "Projekt.cpp" ${SRC} ${IMGUI_SRC})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property (TARGET Projekt PROPERTY CXX_STANDARD 20)
endif()

set (GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE) 
set (GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set (GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Linking libraries

find_package(OpenGL REQUIRED)

target_link_libraries (${PROJECT_NAME}
#PUBLIC ${PSQL_DIR}/lib/libpgcommon.a
#PUBLIC ${PSQL_DIR}/lib/libpgport.a
#PUBLIC ${OPENSSL_DIR}/lib/libssl.a
PUBLIC pqxx
PUBLIC pq
PUBLIC glfw
PUBLIC OpenGL::GL)

#Including headers directories
target_include_directories (${PROJECT_NAME} PUBLIC ${IMGUI_DIR} ${OWN_INCLUDE_DIR})