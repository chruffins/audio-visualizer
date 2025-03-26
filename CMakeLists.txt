cmake_minimum_required(VERSION 3.0)
project(audiovis C)

# set C standard
set(CMAKE_C_STANDARD 11)

# find the files
file(GLOB_RECURSE SOURCES *.c)

add_executable(audiovis main.c menu.c 
graphics/particle.c graphics/vector.c graphics/font.c graphics/camera.c graphics/geometry.c graphics/obj_parser.c graphics/visualization.c
music/song.c music/album.c music/database.c music/audio_parser.c)

target_compile_options(audiovis PRIVATE -O3 -Wall -g -ftree-vectorize)
#target_compile_options(chrus_engine -g)

# set some compiler flags
#target_compile_options(example PUBLIC -std=c++1y -Wall -Wfloat-conversion)
#target_compile_options(${PROJECT_NAME} -O2)

find_library(AL allegro)
find_library(AL_FONT allegro_font)
find_library(AL_AUDIO allegro_audio)
find_library(AL_ACODEC allegro_acodec)
find_library(AL_PRIMITIVES allegro_primitives)
find_library(AL_DIALOG allegro_dialog REQUIRED)
find_library(AL_IMAGE allegro_image)
find_library(AL_COLOR allegro_color)
find_library(MATH m)
find_library(FFTW3 fftw3)
find_library(FFTW3FLOAT fftw3f)

target_link_libraries(audiovis m fftw3 fftw3f allegro allegro_font allegro_ttf allegro_audio 
    allegro_acodec allegro_primitives allegro_color ${AL_DIALOG} allegro_image
    tag_c-2
    sqlite3)