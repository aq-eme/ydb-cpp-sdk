add_library(client-ydb_types-exceptions)

target_link_libraries(client-ydb_types-exceptions PUBLIC
  yutil
)

target_sources(client-ydb_types-exceptions PRIVATE
  exceptions.cpp
)


