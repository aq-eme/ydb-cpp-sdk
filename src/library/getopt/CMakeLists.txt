add_subdirectory(small)

add_library(library-cpp-getopt INTERFACE)
target_link_libraries(library-cpp-getopt INTERFACE
  yutil
  cpp-getopt-small
  library-cpp-svnversion
  library-cpp-build_info
)

add_global_library_for(library-cpp-getopt.global library-cpp-getopt)
target_link_libraries(library-cpp-getopt.global PUBLIC
  yutil
  cpp-getopt-small
  library-cpp-svnversion
  library-cpp-build_info
)
target_sources(library-cpp-getopt.global PRIVATE
  ${YDB_SDK_SOURCE_DIR}/src/library/getopt/print.cpp
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(library-cpp-getopt INTERFACE
    
  )
  target_link_libraries(library-cpp-getopt.global PUBLIC
    
  )
endif()
