cmake_minimum_required(VERSION 3.16)
project(indi_astroweather VERSION 1.2.0 LANGUAGES CXX)

include(GNUInstallDirs)
find_package(CURL REQUIRED)

# Debian/Ubuntu libindi-dev releases do not all ship INDIConfig.cmake. Locate
# the stable public header and driver library directly so the same source builds
# on Astroberry-era packages and current INDI installations.
find_path(INDI_INCLUDE_DIR
          NAMES indiapi.h
          PATH_SUFFIXES libindi
          REQUIRED)
find_library(INDI_DRIVER_LIBRARY
             NAMES indidriver
             REQUIRED)

add_executable(indi_astroweather astroweather.cpp)
target_compile_features(indi_astroweather PRIVATE cxx_std_17)
target_include_directories(indi_astroweather PRIVATE ${INDI_INCLUDE_DIR})
target_link_libraries(indi_astroweather PRIVATE
                      ${INDI_DRIVER_LIBRARY}
                      CURL::libcurl)

install(TARGETS indi_astroweather RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES indi_astroweather.xml
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/indi)
