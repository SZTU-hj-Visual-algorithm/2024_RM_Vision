# Install script for directory: /home/steins_xin/RM_Detection_Sentinel_ROS/src/robot_msgs

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/steins_xin/RM_Detection_Sentinel_ROS/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/robot_msgs/msg" TYPE FILE FILES
    "/home/steins_xin/RM_Detection_Sentinel_ROS/src/robot_msgs/msg/vision.msg"
    "/home/steins_xin/RM_Detection_Sentinel_ROS/src/robot_msgs/msg/robot_ctrl.msg"
    "/home/steins_xin/RM_Detection_Sentinel_ROS/src/robot_msgs/msg/PTZ_Yaw.msg"
    "/home/steins_xin/RM_Detection_Sentinel_ROS/src/robot_msgs/msg/usb_camera.msg"
    "/home/steins_xin/RM_Detection_Sentinel_ROS/src/robot_msgs/msg/PTZ_perception.msg"
    "/home/steins_xin/RM_Detection_Sentinel_ROS/src/robot_msgs/msg/Track_reset.msg"
    "/home/steins_xin/RM_Detection_Sentinel_ROS/src/robot_msgs/msg/Yaw_Decision.msg"
    "/home/steins_xin/RM_Detection_Sentinel_ROS/src/robot_msgs/msg/omni_perception.msg"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/robot_msgs/cmake" TYPE FILE FILES "/home/steins_xin/RM_Detection_Sentinel_ROS/build/robot_msgs/catkin_generated/installspace/robot_msgs-msg-paths.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/steins_xin/RM_Detection_Sentinel_ROS/devel/include/robot_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/roseus/ros" TYPE DIRECTORY FILES "/home/steins_xin/RM_Detection_Sentinel_ROS/devel/share/roseus/ros/robot_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/common-lisp/ros" TYPE DIRECTORY FILES "/home/steins_xin/RM_Detection_Sentinel_ROS/devel/share/common-lisp/ros/robot_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gennodejs/ros" TYPE DIRECTORY FILES "/home/steins_xin/RM_Detection_Sentinel_ROS/devel/share/gennodejs/ros/robot_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND "/usr/bin/python3" -m compileall "/home/steins_xin/RM_Detection_Sentinel_ROS/devel/lib/python3/dist-packages/robot_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages" TYPE DIRECTORY FILES "/home/steins_xin/RM_Detection_Sentinel_ROS/devel/lib/python3/dist-packages/robot_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/steins_xin/RM_Detection_Sentinel_ROS/build/robot_msgs/catkin_generated/installspace/robot_msgs.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/robot_msgs/cmake" TYPE FILE FILES "/home/steins_xin/RM_Detection_Sentinel_ROS/build/robot_msgs/catkin_generated/installspace/robot_msgs-msg-extras.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/robot_msgs/cmake" TYPE FILE FILES
    "/home/steins_xin/RM_Detection_Sentinel_ROS/build/robot_msgs/catkin_generated/installspace/robot_msgsConfig.cmake"
    "/home/steins_xin/RM_Detection_Sentinel_ROS/build/robot_msgs/catkin_generated/installspace/robot_msgsConfig-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/robot_msgs" TYPE FILE FILES "/home/steins_xin/RM_Detection_Sentinel_ROS/src/robot_msgs/package.xml")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/steins_xin/RM_Detection_Sentinel_ROS/build/robot_msgs/catkin_generated/installspace/robot_msgs.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/robot_msgs/cmake" TYPE FILE FILES "/home/steins_xin/RM_Detection_Sentinel_ROS/build/robot_msgs/catkin_generated/installspace/robot_msgs-msg-extras.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/robot_msgs/cmake" TYPE FILE FILES
    "/home/steins_xin/RM_Detection_Sentinel_ROS/build/robot_msgs/catkin_generated/installspace/robot_msgsConfig.cmake"
    "/home/steins_xin/RM_Detection_Sentinel_ROS/build/robot_msgs/catkin_generated/installspace/robot_msgsConfig-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/robot_msgs" TYPE FILE FILES "/home/steins_xin/RM_Detection_Sentinel_ROS/src/robot_msgs/package.xml")
endif()

