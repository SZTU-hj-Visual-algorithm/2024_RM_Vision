;; Auto-generated. Do not edit!


(when (boundp 'robot_msgs::robot_ctrl)
  (if (not (find-package "ROBOT_MSGS"))
    (make-package "ROBOT_MSGS"))
  (shadow 'robot_ctrl (find-package "ROBOT_MSGS")))
(unless (find-package "ROBOT_MSGS::ROBOT_CTRL")
  (make-package "ROBOT_MSGS::ROBOT_CTRL"))

(in-package "ROS")
;;//! \htmlinclude robot_ctrl.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass robot_msgs::robot_ctrl
  :super ros::object
  :slots (_header _yaw _pitch _fire_command _fire_mode _is_follow ))

(defmethod robot_msgs::robot_ctrl
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:yaw __yaw) 0.0)
    ((:pitch __pitch) 0.0)
    ((:fire_command __fire_command) 0)
    ((:fire_mode __fire_mode) 0)
    ((:is_follow __is_follow) 0)
    )
   (send-super :init)
   (setq _header __header)
   (setq _yaw (float __yaw))
   (setq _pitch (float __pitch))
   (setq _fire_command (round __fire_command))
   (setq _fire_mode (round __fire_mode))
   (setq _is_follow (round __is_follow))
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:yaw
   (&optional __yaw)
   (if __yaw (setq _yaw __yaw)) _yaw)
  (:pitch
   (&optional __pitch)
   (if __pitch (setq _pitch __pitch)) _pitch)
  (:fire_command
   (&optional __fire_command)
   (if __fire_command (setq _fire_command __fire_command)) _fire_command)
  (:fire_mode
   (&optional __fire_mode)
   (if __fire_mode (setq _fire_mode __fire_mode)) _fire_mode)
  (:is_follow
   (&optional __is_follow)
   (if __is_follow (setq _is_follow __is_follow)) _is_follow)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; float32 _yaw
    4
    ;; float32 _pitch
    4
    ;; int8 _fire_command
    1
    ;; int8 _fire_mode
    1
    ;; int8 _is_follow
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; float32 _yaw
       (sys::poke _yaw (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _pitch
       (sys::poke _pitch (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; int8 _fire_command
       (write-byte _fire_command s)
     ;; int8 _fire_mode
       (write-byte _fire_mode s)
     ;; int8 _is_follow
       (write-byte _is_follow s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; float32 _yaw
     (setq _yaw (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _pitch
     (setq _pitch (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; int8 _fire_command
     (setq _fire_command (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _fire_command 127) (setq _fire_command (- _fire_command 256)))
   ;; int8 _fire_mode
     (setq _fire_mode (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _fire_mode 127) (setq _fire_mode (- _fire_mode 256)))
   ;; int8 _is_follow
     (setq _is_follow (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _is_follow 127) (setq _is_follow (- _is_follow 256)))
   ;;
   self)
  )

(setf (get robot_msgs::robot_ctrl :md5sum-) "d113cc45537caf169fa198ab9396ee38")
(setf (get robot_msgs::robot_ctrl :datatype-) "robot_msgs/robot_ctrl")
(setf (get robot_msgs::robot_ctrl :definition-)
      "Header header
float32 yaw
float32 pitch
int8 fire_command
int8 fire_mode
int8 is_follow
================================================================================
MSG: std_msgs/Header
# Standard metadata for higher-level stamped data types.
# This is generally used to communicate timestamped data 
# in a particular coordinate frame.
# 
# sequence ID: consecutively increasing ID 
uint32 seq
#Two-integer timestamp that is expressed as:
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
# time-handling sugar is provided by the client library
time stamp
#Frame this data is associated with
string frame_id

")



(provide :robot_msgs/robot_ctrl "d113cc45537caf169fa198ab9396ee38")


