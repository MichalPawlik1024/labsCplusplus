cmake_minimum_required(VERSION 3.24)
project(BattleDuel)

#set(SFML_DIR D:/ProjektyWCplusplus/BattleDuel/SFML-2.5.1/lib/cmake/SFML)
#include_directories(D:/ProjektyWCplusplus/BattleDuel/SFML-2.5.1/include)
#set(SFML_STATIC_LIBRARIES TRUE)
#find_package(SFML COMPONENTS window graphics system audio)
INCLUDE(FetchContent)
SET(BUILD_SHARED_LIBS OFF)
FETCHCONTENT_DECLARE(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.2)
FETCHCONTENT_DECLARE(JSON GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.12.0)
FetchContent_Declare(
        miniaudio
        GIT_REPOSITORY https://github.com/mackron/miniaudio.git
        GIT_TAG master
)

FetchContent_MakeAvailable(miniaudio)
FETCHCONTENT_MAKEAVAILABLE(SFML)
FetchContent_MakeAvailable(JSON)



set(CMAKE_CXX_STANDARD 20)
add_executable(BattleDuel
        main.cpp
        basicClasses.h
        WindowMonitor.h
        GameMechanicWidgets.h
        Game.h
        CrucialData.h
        UIFactory.h
        sound.h
        soundDef.cpp
        easierDesign.h
)
target_link_libraries(BattleDuel  sfml-graphics sfml-audio sfml-window sfml-system nlohmann_json::nlohmann_json
        miniaudio
)
