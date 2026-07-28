cmake_minimum_required(VERSION 3.16)
project(indi_astroweather VERSION 1.2.0 LANGUAGES CXX)

include(GNUInstallDirs)
find_package(INDI REQUIRED)
find_package(CURL REQUIRED)

add_executable(indi_astroweather astroweather.cpp)
target_compile_features(indi_astroweather PRIVATE cxx_std_17)
target_include_directories(indi_astroweather PRIVATE ${INDI_INCLUDE_DIR})
target_link_libraries(indi_astroweather PRIVATE ${INDI_LIBRARIES} CURL::libcurl)

install(TARGETS indi_astroweather RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES indi_astroweather.xml
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/indi)
