set(CLI_SOURCES
    config.cpp
    config.h
    lineParser.cpp
    lineParser.h
    main.cpp
)

add_executable(scanner_exe ${CLI_SOURCES})
set_target_properties(scanner_exe PROPERTIES OUTPUT_NAME "scanner")
target_link_libraries(scanner_exe PRIVATE scanner)
target_include_directories(scanner_exe PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../scanner
)