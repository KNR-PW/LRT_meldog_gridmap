# LRT_meldog_gridmap

ROS 2 package for generating elevation grid maps from RealSense point clouds
using FastDEM.

## Dependencies

```bash
sudo apt update
sudo apt install git build-essential cmake python3-colcon-common-extensions \
  libeigen3-dev libyaml-cpp-dev libspdlog-dev \
  ros-humble-ament-cmake ros-humble-ament-index-cpp \
  ros-humble-rclcpp ros-humble-sensor-msgs ros-humble-grid-map-msgs \
  ros-humble-visualization-msgs ros-humble-tf2 ros-humble-tf2-ros \
  ros-humble-tf2-eigen ros-humble-realsense2-camera
```

## Download

For a new workspace, run the following commands:

```bash
mkdir -p ~/meldog_ws/src
cd ~/meldog_ws

git clone https://github.com/Ikhyeon-Cho/FastDEM.git src/FastDEM
git -C src/FastDEM checkout --detach 5cd3e2552dba62be14cd8fe83d5c81261eafd12e

git clone https://github.com/KNR-PW/LRT_meldog_gridmap.git src/meldog_gridmap
```

## Build

```bash
source /opt/ros/humble/setup.bash
cd ~/meldog_ws

colcon build --packages-select fastdem \
  --cmake-args -DCMAKE_BUILD_TYPE=Release
source install/setup.bash

colcon build --packages-select meldog_gridmap --symlink-install \
  --cmake-args -DCMAKE_BUILD_TYPE=Release
source install/setup.bash
```

## Run

```bash
source /opt/ros/humble/setup.bash
source ~/meldog_ws/install/setup.bash
ros2 launch meldog_gridmap camera_gridmap.launch.py
```

