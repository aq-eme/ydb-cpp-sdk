add_subdirectory(credentials)
add_subdirectory(exceptions)
add_subdirectory(fatal_error_handlers)
add_subdirectory(operation)
add_subdirectory(status)

add_library(cpp-client-ydb_types)

target_link_libraries(cpp-client-ydb_types PUBLIC
  yutil
  protobuf::libprotobuf
  library-grpc-client
  yql-public-issue
  tools-enum_parser-enum_serialization_runtime
)

generate_enum_serilization(cpp-client-ydb_types
  ${YDB_SDK_SOURCE_DIR}/include/ydb-cpp-sdk/client/types/s3_settings.h
  INCLUDE_HEADERS
  include/ydb-cpp-sdk/client/types/s3_settings.h
)

generate_enum_serilization(cpp-client-ydb_types
  ${YDB_SDK_SOURCE_DIR}/include/ydb-cpp-sdk/client/types/status_codes.h
  INCLUDE_HEADERS
  include/ydb-cpp-sdk/client/types/status_codes.h
)

add_library(YDB-CPP-SDK::Types ALIAS cpp-client-ydb_types)
