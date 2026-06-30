cmake_minimum_required(VERSION 3.19)
project(BallGame LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(BallGame
    WIN32 MACOSX_BUNDLE
    main.cpp
    ballgrid.cpp
    ballgrid.h
    ballgrid.ui
    ball.h ball.cpp
    weapon.h weapon.cpp
    geometry.h geometry.cpp
    upgrade.h upgrade.cpp
    power.h power.cpp
    holypower.h holypower.cpp
    debugwidget.h debugwidget.cpp
    waterpower.h waterpower.cpp
    powerfactory.h powerfactory.cpp
    gamelogic.h gamelogic.cpp
    gamecontext.h
    thiefpower.h thiefpower.cpp
    earthpower.h earthpower.cpp
    powercolors.h
    gameconfig.h
    gameconfig.cpp
)

target_link_libraries(BallGame
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS BallGame
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET BallGame
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
