cmake_minimum_required(VERSION 3.21)

project(BoltBrowser VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Network Widgets WebEngineWidgets)
include(GNUInstallDirs)

qt_add_executable(BoltBrowser
    main.cpp
    main_window.cpp
    main_window.h
    address_resolver.cpp
    address_resolver.h
)

target_link_libraries(BoltBrowser PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::WebEngineWidgets)
target_compile_definitions(BoltBrowser PRIVATE
    BOLT_APP_VERSION=\"${PROJECT_VERSION}\"
)

if(WIN32)
    set_target_properties(BoltBrowser PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
if(APPLE)
    set_target_properties(BoltBrowser PROPERTIES MACOSX_BUNDLE TRUE)
endif()

configure_file(test-page.html test-page.html COPYONLY)

if(APPLE)
    install(TARGETS BoltBrowser BUNDLE DESTINATION .)
    install(FILES test-page.html DESTINATION BoltBrowser.app/Contents/MacOS)
else()
    install(TARGETS BoltBrowser RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES test-page.html DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

option(BOLT_DEPLOY_RUNTIME "Bundle Qt runtime libraries and WebEngine assets" ON)
if(BOLT_DEPLOY_RUNTIME)
    qt_generate_deploy_app_script(TARGET BoltBrowser OUTPUT_SCRIPT deploy_script)
    install(SCRIPT ${deploy_script})
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(AddressResolverTests
        tests/address_resolver_test.cpp
        address_resolver.cpp
        address_resolver.h
    )
    target_link_libraries(AddressResolverTests PRIVATE Qt6::Core Qt6::Network)
    add_test(NAME address-resolver COMMAND AddressResolverTests)
endif()
