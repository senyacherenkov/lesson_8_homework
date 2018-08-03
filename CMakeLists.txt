cmake_minimum_required(VERSION 3.2)

project(allocator)
# VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

add_executable(allocator_cli allocator.cpp user_allocator.h)

#add_library(user_allocator_lib user_allocator.h)
#add_library(user_container_lib user_container.h)

set_target_properties(allocator_cli PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

#target_link_libraries(allocator_cli user_allocator_lib)
#target_link_libraries(allocator_cli user_container_lib)

install(TARGETS allocator_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT senyacherenkov@yandex.ru)

include (CPack)
