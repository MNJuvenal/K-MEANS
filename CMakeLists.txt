cmake_minimum_required(VERSION 3.15)
project(kmeans_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_TOY "Build toy 2D K-means example" ON)
option(BUILD_IMAGE "Build image compression tool (requires OpenCV)" ON)
option(BUILD_VIEWER "Build side-by-side viewer (requires OpenCV)" ON)

option(BUILD_VIS2D "Build 2D K-means visualizer (requires OpenCV)" ON)
option(BUILD_REFACTORED "Build refactored versions using kmeans_lib.hpp" ON)

if(BUILD_TOY)
  add_executable(kmeans src/kmeans.cpp)
endif()

# Version refactorisée simple
if(BUILD_REFACTORED)
  add_executable(kmeans_simple src/kmeans_simple.cpp)
  target_include_directories(kmeans_simple PRIVATE src)
endif()

if(BUILD_IMAGE OR BUILD_VIEWER)
  find_package(OpenCV 4 QUIET COMPONENTS core imgcodecs imgproc highgui)
  if(NOT OpenCV_FOUND)
    message(WARNING "OpenCV non trouvé. Les cibles image/visualisation seront désactivées.")
    set(BUILD_IMAGE OFF)
    set(BUILD_VIEWER OFF)
  endif()
endif()

if(BUILD_IMAGE)
  add_executable(kmeans_image src/kmeans_image.cpp)
  target_link_libraries(kmeans_image PRIVATE ${OpenCV_LIBS})
  target_include_directories(kmeans_image PRIVATE ${OpenCV_INCLUDE_DIRS})
  
  # Version refactorisée pour images
  if(BUILD_REFACTORED)
    add_executable(kmeans_image_refactored src/kmeans_image_refactored.cpp)
    target_link_libraries(kmeans_image_refactored PRIVATE ${OpenCV_LIBS})
    target_include_directories(kmeans_image_refactored PRIVATE ${OpenCV_INCLUDE_DIRS} src)
  endif()
endif()

if(BUILD_VIEWER)
  add_executable(view_side_by_side src/view_side_by_side.cpp)
  target_link_libraries(view_side_by_side PRIVATE ${OpenCV_LIBS})
  target_include_directories(view_side_by_side PRIVATE ${OpenCV_INCLUDE_DIRS})
endif()


# ---------- 2D K-means Visualizer ----------
if(BUILD_VIS2D)
  add_executable(kmeans_visual_2d src/kmeans_visual_2d.cpp)
  target_link_libraries(kmeans_visual_2d PRIVATE ${OpenCV_LIBS})
  target_include_directories(kmeans_visual_2d PRIVATE ${OpenCV_INCLUDE_DIRS})
endif()

# ---------- Pipeline (compress + visualize) ----------
if(BUILD_IMAGE AND BUILD_VIEWER)
  add_executable(kmeans_pipeline src/kmeans_pipeline.cpp)
  target_link_libraries(kmeans_pipeline PRIVATE ${OpenCV_LIBS})
  target_include_directories(kmeans_pipeline PRIVATE ${OpenCV_INCLUDE_DIRS})
endif()
