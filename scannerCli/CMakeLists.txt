# Исполняемый файл консольного интерфейса
set(CLI_SOURCES
    config.cpp
    config.h
    lineParser.cpp
    lineParser.h
    main.cpp
)

# Создание исполняемого файла
add_executable(scanner_exe ${CLI_SOURCES})

# Переименование выходного файла в scanner
set_target_properties(scanner_exe PROPERTIES OUTPUT_NAME "scanner")

# Связываем с DLL
target_link_libraries(scanner_exe PRIVATE scanner)

# Включаем заголовочные файлы из DLL
target_include_directories(scanner_exe PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../scanner
)