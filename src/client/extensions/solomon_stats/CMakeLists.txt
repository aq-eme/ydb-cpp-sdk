add_library(client-extensions-solomon_stats)

target_link_libraries(client-extensions-solomon_stats PUBLIC
  yutil
  monlib-encode-json
  cpp-monlib-metrics
  cpp-monlib-service
  monlib-service-pages
  cpp-client-ydb_extension
)

target_sources(client-extensions-solomon_stats PRIVATE
  pull_client.cpp
  pull_connector.cpp
)

add_library(YDB-CPP-SDK::SolomonStats ALIAS client-extensions-solomon_stats)
