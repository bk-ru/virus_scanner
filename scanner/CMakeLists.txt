# DLL библиотека с основной логикой сканирования
set(SCANNER_SOURCES
    hashDatabase.cpp
    hashDatabase.h
    logger.cpp
    logger.h
    md5Calc.cpp
    md5Calc.h
    scanner.cpp
    scanner.h
    scannerApi.h
    threadPool.cpp
    threadPool.h
    utils.cpp
    utils.h
)

find_package(OpenSSL REQUIRED)

# Создание динамической библиотеки
add_library(scanner SHARED ${SCANNER_SOURCES})

# Определяем макрос для экспорта
target_compile_definitions(scanner PRIVATE SCANNER_DLL_EXPORTS)

# Подключаем OpenSSL
target_link_libraries(scanner PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Подключаем системные библиотеки
if(WIN32)
    target_link_libraries(scanner PRIVATE ws2_32 wsock32)
endif()

# Настройки для UNIX систем
if(UNIX)
    target_link_libraries(scanner PRIVATE pthread)
endif()

# Подключаем заголовочные файлы
target_include_directories(scanner PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)