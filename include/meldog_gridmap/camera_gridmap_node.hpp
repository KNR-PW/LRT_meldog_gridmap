#pragma once 

#include <memory>
#include <optional>
#include <Eigen/Geometry>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <grid_map_msgs/msg/grid_map.hpp>

#include <tf2_ros/transform_listener.hpp>
#include <tf2_ros/buffer.hpp>

#include <fastdem/fastdem.hpp>

namespace camera_gridmap {

    class CameraGridMapNode : public rclcpp::Node 
    {

    public:

        CameraGridMapNode();
    
    private:
        
        void cloudCallback(sensor_msgs::msg::PointCloud2::SharedPtr msg);
        void publishGridMap();

    private:

        rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr _cloudSub;
        rclcpp::Publisher<grid_map_msgs::msg::GridMap>::SharedPtr _gridMapPub;

        fastdem::Config _config;
        fastdem::ElevationMap _map;
        std::unique_ptr<fastdem::FastDEM> _mapper; 

        std::shared_ptr<tf2_ros::Buffer> _tfBuffer;
        std::shared_ptr<tf2_ros::TransformListener> _tfListener;

        std::string _baseFrame = "camera_link";
        std::optional<Eigen::Isometry3d> _T_base_sensor;
    };
}