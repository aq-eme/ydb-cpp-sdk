add_library(cpp-streams-zstd)
target_link_libraries(cpp-streams-zstd PUBLIC
  yutil
  ZSTD::ZSTD
)
target_sources(cpp-streams-zstd PRIVATE
  ${YDB_SDK_SOURCE_DIR}/src/library/streams/zstd/zstd.cpp
)
