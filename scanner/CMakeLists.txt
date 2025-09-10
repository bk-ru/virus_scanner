cmake_minimum_required(VERSION 3.21)
project(scanner VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск зависимостей
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Создаем библиотеку
add_library(scanner SHARED
    scanner.cpp
    scanner.h
    hashDatabase.cpp
    hashDatabase.h
    logger.cpp
    logger.h
    md5Calc.cpp
    md5Calc.h
    threadPool.cpp
    threadPool.h
    utils.cpp
    utils.h
)

# Windows-specific settings
if(WIN32)
    set_target_properties(scanner PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    target_compile_definitions(scanner PRIVATE _WIN32_WINNT=0x0601)
endif()

# Включение заголовков
target_include_directories(scanner
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Линковка
target_link_libraries(scanner
    PUBLIC
        OpenSSL::Crypto
        Threads::Threads
)

# Экспорт символов
target_compile_definitions(scanner
    PRIVATE 
        SCANNER_DLL_EXPORTS
)

# Тестовое приложение для проверки компиляции
add_executable(test_compilation test_compile.cpp)
target_link_libraries(test_compilation PRIVATE scanner)