cmake_minimum_required(VERSION 3.0.0)
project(Database)

#版本号
set(VERSION_MAJOR 6)
set(VERSION_MINOR 2)
set(VERSION_PATCH 0)

#编译器参数
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-O0 -g -Wall -Werror -Wno-unknown-pragmas -Wno-parentheses -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -Wall -Wno-unknown-pragmas -DDEBUG")
 
#enable_testing()

aux_source_directory(src SRC)
add_executable(db ${SRC})
target_include_directories(db PUBLIC include)
target_link_libraries(db PUBLIC m)


# #打包
# include(InstallRequiredSystemLibraries) 
# set(CPACK_PACKAGE_DIRECTORY ${PROJECT_SOURCE_DIR}/pack)
# set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/License.txt")
# set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
# set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
# set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
# set(PROJECT_VERSION_FULL ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
# set(CPACK_SOURCE_IGNORE_FILES
#         ${PROJECT_SOURCE_DIR}/build
#         ${PROJECT_SOURCE_DIR}/pack
#         ${PROJECT_SOURCE_DIR}/.git
#         ${PROJECT_SOURCE_DIR}/.gitignore
#         ${PROJECT_SOURCE_DIR}/.vscode)
 
# set(CPACK_SOURCE_GENERATOR "TGZ")
# set(CPACK_SOURCE_PACKAGE_FILE_NAME Database-${PROJECT_VERSION_FULL})

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

# include(CPack)