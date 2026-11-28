from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    pkg_share = FindPackageShare("meldog_gridmap")

    camera_gridmap = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            PathJoinSubstitution([pkg_share, "launch", "camera_gridmap.launch.py"])
        ),
    )

    rviz = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="screen",
        arguments=["-d", LaunchConfiguration("rviz_config")],
    )

    return LaunchDescription([
        DeclareLaunchArgument(
            "rviz_config",
            default_value=PathJoinSubstitution([pkg_share, "config", "rviz_config.rviz"]),
            description="Path to the RViz2 config file.",
        ),
        camera_gridmap,
        rviz,
    ])
