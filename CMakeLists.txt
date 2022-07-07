cmake_minimum_required(VERSION 3.6)
project(searchBase)
set(CMAKE_CXX_STANDARD 14)
IF(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RELEASE")
    message(AUTHOR_WARNING "RV: Creating the executable in release mode.")
ENDIF()
# SET(CMAKE_BUILD_TYPE Debug) # Set Debug build
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# ####### Below: Including libtorch
# list(APPEND CMAKE_PREFIX_PATH "/usr/local/libtorch")

# set(CUDNN_LIBRARY_PATH "/usr/local/cuda-11.6/lib64")
# set(CUDNN_INCLUDE_PATH "/usr/include")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
# find_package(Torch REQUIRED)
# set(TORCH_INCLUDE_DIRS
#     ${TORCH_INSTALL_PREFIX}/include
#     ${TORCH_INSTALL_PREFIX}/include/torch/csrc/api/include)

##### Including all header and cpp files 
include_directories("inc")  # Add header files in inc folder
# https://stackoverflow.com/questions/1027247/is-it-better-to-specify-source-files-with-glob-or-each-file-individually-in-cmak
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")  # Add all .cpp files in src

add_library(myLib ${SOURCES}) # Adds these files and everything into a "library" called my lib


find_package(Boost REQUIRED COMPONENTS program_options system filesystem) # Find Boost
include_directories( ${Boost_INCLUDE_DIRS}) 
# ${TORCH_INCLUDE_DIRS}) # Include those directories


######################### Below: Create executables for each target #######################


### Create an executable target from test
add_executable(tests runner/tests.cpp)
target_link_libraries(tests ${Boost_LIBRARIES} myLib )
# ${TORCH_LIBRARIES})

### Create an executable target from torch_practice
# add_executable(torch_practice runner/torch_practice.cpp)
# target_link_libraries(torch_practice ${Boost_LIBRARIES} myLib ${TORCH_LIBRARIES})

### Create an executable target from batch_h_runner
add_executable(batch_h_runner runner/batch_h_runner.cpp)
target_link_libraries(batch_h_runner ${Boost_LIBRARIES} myLib)# ${TORCH_LIBRARIES})

### Create an executable target from practice
# add_executable(practice runner/practice.cpp)
# target_link_libraries(practice ${Boost_LIBRARIES} myLib ${TORCH_LIBRARIES})