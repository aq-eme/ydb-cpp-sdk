add_library(cpp-digest-murmur)

target_link_libraries(cpp-digest-murmur PUBLIC
  yutil
)

target_sources(cpp-digest-murmur PRIVATE
  ${YDB_SDK_SOURCE_DIR}/src/library/digest/murmur/murmur.cpp
)
