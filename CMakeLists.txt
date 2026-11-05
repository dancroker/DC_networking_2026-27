cmake_minimum_required(VERSION 3.16)
project(NETWORK_fox_n_hounds)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

#---- Fetch SFML, only works if you have git installed on your machine ----#

include(FetchContent)   # Allows fetching content remotely at configure time
FetchContent_Declare(   # Fetches SFML from github and puts it in the build directory
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0 )   # Makes sure to fetch SFML version 3.1.0)

FetchContent_MakeAvailable(SFML)    # Makes sure SFML and its dependencies are avaiable for the project

#---- File management & .exe creation ----#

# Set variable SOURCE_FILES to hold names & locations of files that need to be compiled into the application
set(SOURCE_FILES src/main.cpp src/Game.cpp src/Game.h "src/Tile.h" "src/Tile.cpp" "src/Piece.h" "src/Piece.cpp" "src/Player.h" "src/Player.cpp" "src/Board.h" "src/Board.cpp")  

# Name the application and tells compiler which source files to use (using SOURCE_FILES variable created above)
add_executable(NETWORK_fox_n_hounds ${SOURCE_FILES} "src/Server.h" "src/Server.cpp" "src/Client.h" "src/Client.cpp")  

target_link_libraries(NETWORK_fox_n_hounds PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Network SFML::Audio) # Links the SFML libraries we need to our application
target_compile_features(NETWORK_fox_n_hounds PRIVATE cxx_std_20)  # Defines which standard of C++ the compiler should use

# Copies anything that's changed in the source data directory over the build's data diroctory, so the executable can access it.
# Happens after code is built, rather than only when this file is run.
add_custom_command(TARGET NETWORK_fox_n_hounds POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_CURRENT_SOURCE_DIR}/Data/" "${CMAKE_CURRENT_BINARY_DIR}/Data/"
)

#Optional command, deletes the data directory in the build folder and copies data from source directory to build.Could make building slower but removes files from the build directory that aren't in the source directory'
#add_custom_command(TARGET NETWORK_fox_n_hounds POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/Data/"
#    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Data/" "${CMAKE_CURRENT_BINARY_DIR}/Data/"
#)