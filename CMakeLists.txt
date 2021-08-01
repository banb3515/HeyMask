cmake_minimum_required(VERSION 3.1)

# ----- Project -----
project(
  HeyMask
  VERSION 1.0
  DESCRIPTION "마스크를 착용하지 않은 사람을 탐지합니다."
  LANGUAGES CXX
)

# CUDA 사용 여부
add_definitions(-D USE_CUDA=1) # 1: 사용, 0: 미사용

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES ./HeyMask/main.cpp ./HeyMask/Detector.cpp ./HeyMask/Sender.cpp ./HeyMask/Sound.cpp ./HeyMask/Logger.cpp)
# ----- Project End -----

# ----- Thread -----
find_package(Threads REQUIRED)
# ----- Thread End -----

# ----- OpenCV -----
set(OpenCV_DIR [OpenCV.cmake PATH])
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
# ----- OpenCV End -----

# ----- MPG123 & AO -----
find_path(MPG123_INCLUDE_DIR mpg123.h PATHS "${MPG123_DIR}" PATH_SUFFIXES include)
find_library(MPG123_LIBRARIES NAMES mpg123 mpg123-0 PATHS "${MPG123_DIR}" PATH_SUFFIXES lib)

find_path(LIBAO_INCLUDE_DIR ao.h /usr/include/ao /usr/local/include/ao)
find_library(LIBAO_LIBRARIES NAMES ao PATH /usr/lib /usr/local/lib)

include_directories(${LIBAO_INCLUDE_DIR} ${MPG123_INCLUDE_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPG123 DEFAULT_MSG MPG123_LIBRARIES MPG123_INCLUDE_DIR LIBAO DEFAULT_MSG LIBAO_LIBRARIES LIBAO_INCLUDE_DIRS)
# ----- MPG123 & AO End -----

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} Threads::Threads ${OpenCV_LIBS} ${LIBAO_LIBRARIES} ${MPG123_LIBRARIES})
