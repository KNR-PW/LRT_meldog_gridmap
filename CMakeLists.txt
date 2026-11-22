cmake_minimum_required(VERSION 3.8)
project(meldog_gridmap LANGUAGES CXX)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)

find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(grid_map_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)

find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_eigen REQUIRED)

find_package(Eigen3 REQUIRED)
find_package(fastdem REQUIRED)

add_executable(camera_gridmap_node
  src/camera_gridmap_node.cpp
)

target_compile_features(camera_gridmap_node PRIVATE cxx_std_17)

target_include_directories(camera_gridmap_node PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

ament_target_dependencies(camera_gridmap_node
  rclcpp
  sensor_msgs
  grid_map_msgs
  visualization_msgs
  tf2
  tf2_ros
  tf2_eigen
  ament_index_cpp
)

target_link_libraries(camera_gridmap_node
  fastdem::fastdem
  Eigen3::Eigen
)

install(TARGETS camera_gridmap_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  config
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()