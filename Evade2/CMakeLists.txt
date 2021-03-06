CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(Evade2)

ADD_COMPILE_DEFINITIONS(CE_SDL2_WINDOW_TITLE="Evade 2 :: by Modus Create")

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-implicit-exception-spec-mismatch")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-implicit-exception-spec-mismatch")
include(ProcessorCount)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/creative-engine/cmake.files)

#ADD_COMPILE_DEFINITIONS(__DINGUX__=true)
ADD_COMPILE_DEFINITIONS(__MODUS_TARGET_SDL2_AUDIO__=true)
ADD_COMPILE_DEFINITIONS(__MODUS_TARGET_DESKTOP_CONTROLS__=true)
ADD_COMPILE_DEFINITIONS(__MODUS_TARGET_DESKTOP_DISPLAY__=true)

# Include Creative Engine
# Exports:
# ${CREATIVE_ENGINE_INCLUDE_DIRS} - used in INCLUDE_DIRECTORIES(...)
# ${CREATIVE_ENGINE_SOURCE_FILES} - used in ADD_EXECUTABLE(...)
# ${CREATIVE_ENGINE_LINK_LIBRARIES} - used in TARGET_LINK_LIBRARIES(...)
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/creative-engine/CreativeEngine.cmake)


SET(EVADE2_INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/resources
        ${CMAKE_SOURCE_DIR}/src/common
        ${CMAKE_SOURCE_DIR}/src/SplashState
        ${CMAKE_SOURCE_DIR}/src/AttractState
        ${CMAKE_SOURCE_DIR}/src/MainMenuState
        ${CMAKE_SOURCE_DIR}/src/GameState
        #    ${CMAKE_SOURCE_DIR}/src/img/
        #    ${CMAKE_SOURCE_DIR}/src/GameMenuState
        #    ${CMAKE_SOURCE_DIR}/src/DebugMenuState
        #    ${CMAKE_SOURCE_DIR}/src/GameOverState
        #    ${CMAKE_SOURCE_DIR}/src/VictoryState
        #    ${CMAKE_SOURCE_DIR}/src/MainOptionsState
        #    ${CMAKE_SOURCE_DIR}/src/CreditsState
        #    ${CMAKE_SOURCE_DIR}/src/ResetState
        ${CMAKE_BINARY_DIR}/usr/local/include
        )

INCLUDE_DIRECTORIES(
        ${EVADE2_INCLUDE_DIRS}
        ${CREATIVE_ENGINE_INCLUDE_DIRS}
)

# gather Evade 2 sources
FILE(GLOB_RECURSE EVADE2_SRC RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

ADD_EXECUTABLE(
        ${PROJECT_NAME}
        Resources.bin
        ${CREATIVE_ENGINE_SOURCE_FILES}
        ${EVADE2_SRC}
        src/GameState/GGameState.cpp src/GameState/GGameState.h src/common/GStarfield.cpp src/common/GStarfield.h src/GameState/GPlayerBulletProcess.cpp src/GameState/GPlayerBulletProcess.h src/main.cpp src/GameState/GEnemyProcess.cpp src/GameState/GEnemyProcess.h src/GameState/GEnemyBulletProcess.cpp src/GameState/GEnemyBulletProcess.h src/AttractState/GAttractState.cpp src/AttractState/GAttractState.h src/AttractState/GAttractProcess.cpp src/AttractState/GAttractProcess.h src/MainMenuState/GMainMenuState.cpp src/MainMenuState/GMainMenuState.h src/MainMenuState/GMainMenuProcess.cpp src/MainMenuState/GMainMenuProcess.h src/GameState/GNextWaveProcess.cpp src/GameState/GNextWaveProcess.h src/GameState/GBossProcess.cpp src/GameState/GBossProcess.h src/GameState/GGameOverProcess.cpp src/GameState/GGameOverProcess.h)

ProcessorCount(N)
if (NOT N EQUAL 0)
    set(${PROJECT_NAME}_FLAGS -j${N})
    set(${PROJECT_NAME}_BUILD_FLAGS -j${N})
    set(${PROJECT_NAME}_args ${${PROJECT_NAME}_args} PARALLEL_LEVEL ${N})
endif ()

BUILD_COMMAND(
        $(MAKE) --silent
)

# make Evade2.app
if (APPLE)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
    INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ".")
endif (APPLE)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${CREATIVE_ENGINE_LINK_LIBRARIES})

#INCLUDE( ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cmake )
