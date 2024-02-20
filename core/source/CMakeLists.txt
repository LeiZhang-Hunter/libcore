# link os source
aux_source_directory(os OS_SOURCE_CODE)

# link event source
aux_source_directory(event OS_EVENT_CODE)

# link component source
aux_source_directory(component OS_COMPONENT_CODE)

aux_source_directory(component/process OS_COMPONENT_PROCESS_CODE)

aux_source_directory(http OS_HTTP_CODE)

aux_source_directory(http/ssl OS_HTTP_SSL_CODE)

aux_source_directory(common OS_COMMON_CODE)

find_package(ZLIB REQUIRED)

