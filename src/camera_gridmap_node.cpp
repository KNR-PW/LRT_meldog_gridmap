#include <cmath>
#include <functional>

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <nanopcl/bridge/ros2.hpp>
#include <fastdem/bridge/ros2.hpp>
#include <tf2_eigen/tf2_eigen.hpp>

#include "meldog_gridmap/camera_gridmap_node.hpp"


namespace camera_gridmap {

  CameraGridMapNode::CameraGridMapNode() : Node("gridmap_publisher")
  {
      // fastdem config path
      this->declare_parameter("fastdem_config_path",
          ament_index_cpp::get_package_share_directory("meldog_gridmap") + "/config/fastdem.yaml");

      // realsense camera topic name
      this->declare_parameter("realsense_topic", "/camera/camera/depth/color/points");

      // fastdem map resolution[m]
      this->declare_parameter("map_resolution", 0.04f);
      
      // camera link 
      _baseFrame = this->declare_parameter<std::string>("base_frame", "camera_link");
      
      // pub topic 
      const auto gridmap_topic = this->declare_parameter<std::string>("gridmap_topic", "gridmap");

      try {
        _config = fastdem::loadConfig(this->get_parameter("fastdem_config_path").as_string());
      } catch (const std::exception& e) {
        RCLCPP_FATAL(this->get_logger(), "Failed to load FastDEM config: %s", e.what());
        throw;
      }

      const float resolution = this->get_parameter("map_resolution").get_value<float>();
      const float mapSize = std::ceil(2.0f * _config.point_filter.range_max / resolution) * resolution;

      _map.setGeometry(mapSize, mapSize, resolution);
      _map.setFrameId(_baseFrame);

      _mapper = std::make_unique<fastdem::FastDEM>(_map, _config);

      _tfBuffer = std::make_shared<tf2_ros::Buffer>(this->get_clock());
      _tfListener = std::make_shared<tf2_ros::TransformListener>(*_tfBuffer);

      _gridMapPub = this->create_publisher<grid_map_msgs::msg::GridMap>(gridmap_topic, 1);
      
      auto cloudQos = rclcpp::SensorDataQoS();
      cloudQos.keep_last(1);

      _cloudSub = this->create_subscription<sensor_msgs::msg::PointCloud2>(
        this->get_parameter("realsense_topic").as_string(), cloudQos,
        [this](sensor_msgs::msg::PointCloud2::SharedPtr msg) { cloudCallback(msg); });
  }

  void CameraGridMapNode::cloudCallback(sensor_msgs::msg::PointCloud2::SharedPtr msg)
  {
    if (!_T_base_sensor.has_value()) {
      try {
        _T_base_sensor = tf2::transformToEigen(_tfBuffer->lookupTransform(_baseFrame, msg->header.frame_id, tf2::TimePointZero));
      } catch (const tf2::TransformException& e) {
        RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 2000, "TF lookup failed: %s", e.what());
        return;
      }
    }

    if (!_mapper->integrate(nanopcl::from(*msg), _T_base_sensor.value(), Eigen::Isometry3d::Identity())) {
      RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 2000, "Couldn't convert PointCloud2 to GridMap");
      return;
    }

    publishGridMap();
  }

  void CameraGridMapNode::publishGridMap()
  {
    if (_gridMapPub->get_subscription_count() == 0) {
      RCLCPP_DEBUG(this->get_logger(), "GridMap Subscription cout == 0");
      return;
    }

    if (_map.isEmpty()) {
      RCLCPP_DEBUG(this->get_logger(), "GridMap is empty");
      return;
    }

    _gridMapPub->publish(fastdem::ros2::toGridMap(_map));
  }
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<camera_gridmap::CameraGridMapNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}