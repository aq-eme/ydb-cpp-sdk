add_library(cpp-string_utils-scan)

target_link_libraries(cpp-string_utils-scan PUBLIC
  yutil
)

target_sources(cpp-string_utils-scan PRIVATE
  ${YDB_SDK_SOURCE_DIR}/src/library/string_utils/scan/scan.cpp
)


