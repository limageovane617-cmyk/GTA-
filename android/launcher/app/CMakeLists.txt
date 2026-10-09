# ==================================================================================================
# reVC – Mobile Edition (Android) – Root CMake Configuration
#
# Configuração do CMake para compilar o módulo principal do jogo reVC
# por meio do Android NDK.
# ==================================================================================================

cmake_minimum_required(VERSION 3.22.1)

message("")
message("================================================================================")
message(" reVC - Build Configuration Initialized")
message(" Preparing Android game sources...")
message("================================================================================")
message("")

# --------------------------------------------------------------------------------
# Project Metadata
# --------------------------------------------------------------------------------

set(REVC_PROJECT_NAME "reVC")
set(REVC_VERSION "0.0.1")
set(REVC_DESCRIPTION "reverted ${REVC_PROJECT_NAME}")
set(REVC_LIB_NAME "revc")

project(
    ${REVC_PROJECT_NAME}
    VERSION ${REVC_VERSION}
    DESCRIPTION "${REVC_DESCRIPTION}"
    LANGUAGES C CXX
)

# --------------------------------------------------------------------------------
# Directory Configuration
# --------------------------------------------------------------------------------

# Diretório deste arquivo:
# <repositorio>/android/launcher/app

get_filename_component(
    REVC_ROOT_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}"
    ABSOLUTE
)

# A partir de android/launcher/app, três níveis acima chegam à raiz
# do repositório:
# app -> launcher -> android -> raiz do repositório

get_filename_component(
    REVC_GAME_DIR
    "${REVC_ROOT_DIR}/../../../"
    ABSOLUTE
)

message(" Game source directory: ${REVC_GAME_DIR}")
message(" Checking for the main CMakeLists.txt...")

# --------------------------------------------------------------------------------
# Validate Game Sources
# --------------------------------------------------------------------------------

if(NOT IS_DIRECTORY "${REVC_GAME_DIR}")
    message(FATAL_ERROR
        "Game source directory does not exist: ${REVC_GAME_DIR}"
    )
endif()

if(NOT EXISTS "${REVC_GAME_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "Main CMakeLists.txt was not found at: ${REVC_GAME_DIR}/CMakeLists.txt"
    )
endif()

# --------------------------------------------------------------------------------
# Add Main Game Module
# --------------------------------------------------------------------------------

# Evita que o CMake tente incluir este mesmo arquivo como subdiretório.
if("${REVC_GAME_DIR}" STREQUAL "${REVC_ROOT_DIR}")
    message(FATAL_ERROR
        "Game source directory incorrectly points to the Android app directory."
    )
endif()

message("")
message("================================================================================")
message(" Main game sources found.")
message(" Configuring the reVC game module...")
message("================================================================================")
message("")

add_subdirectory(
    "${REVC_GAME_DIR}"
    "${CMAKE_BINARY_DIR}/game_build"
)

message("")
message(" reVC game module configuration completed.")
message("")
