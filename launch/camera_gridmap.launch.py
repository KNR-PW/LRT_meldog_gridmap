from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    realsense = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            PathJoinSubstitution([
                FindPackageShare("realsense2_camera"), "launch", "rs_launch.py"
            ])
        ),
        launch_arguments={
            "camera_namespace": "camera",
            "camera_name": "camera",
            "serial_no": LaunchConfiguration("serial_no"),
            "enable_depth": "true",
            "enable_color": "false",
            "depth_module.depth_profile": "424x240x15",
            "pointcloud.enable": "true",
            "pointcloud.stream_filter": "0",
            "publish_tf": "true",
        }.items(),
    )

    gridmap = Node(
        package="meldog_gridmap",
        executable=LaunchConfiguration("gridmap_executable"),
        name="gridmap_publisher",
        output="screen",
        parameters=[{"realsense_topic": "/camera/camera/depth/color/points"}],
        remappings=[("/gridmap", LaunchConfiguration("gridmap_topic"))],
    )

    return LaunchDescription([
        DeclareLaunchArgument(
            "gridmap_executable",
            default_value="camera_gridmap_node",
            description="Installed executable from the meldog_gridmap package.",
        ),
        DeclareLaunchArgument(
            "gridmap_topic",
            default_value="/gridmap",
            description="Topic for the published GridMap messages.",
        ),
        DeclareLaunchArgument(
            "serial_no",
            default_value="''",
            description="RealSense serial with a leading underscore. Empty selects a camera.",
        ),
        realsense,
        gridmap,
    ])
