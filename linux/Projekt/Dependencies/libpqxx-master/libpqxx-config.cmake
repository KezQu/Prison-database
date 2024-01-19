include(CMakeFindDependencyMacro)
add_subdirectory (${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/Dependencies/libpq/libpq)

include("${CMAKE_CURRENT_LIST_DIR}/libpqxx-targets.cmake")
