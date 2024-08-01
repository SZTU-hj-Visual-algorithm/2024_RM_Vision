;; Auto-generated. Do not edit!


(when (boundp 'robot_msgs::PTZ_perception)
  (if (not (find-package "ROBOT_MSGS"))
    (make-package "ROBOT_MSGS"))
  (shadow 'PTZ_perception (find-package "ROBOT_MSGS")))
(unless (find-package "ROBOT_MSGS::PTZ_PERCEPTION")
  (make-package "ROBOT_MSGS::PTZ_PERCEPTION"))

(in-package "ROS")
;;//! \htmlinclude PTZ_perception.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass robot_msgs::PTZ_perception
  :super ros::object
  :slots (_header _track_id _yaw _pitch _score _target_lock _spin_state _fire_command _fire_mode _x _y _z ))

(defmethod robot_msgs::PTZ_perception
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:track_id __track_id) 0)
    ((:yaw __yaw) 0.0)
    ((:pitch __pitch) 0.0)
    ((:score __score) 0)
    ((:target_lock __target_lock) 0)
    ((:spin_state __spin_state) 0)
    ((:fire_command __fire_command) 0)
    ((:fire_mode __fire_mode) 0)
    ((:x __x) 0.0)
    ((:y __y) 0.0)
    ((:z __z) 0.0)
    )
   (send-super :init)
   (setq _header __header)
   (setq _track_id (round __track_id))
   (setq _yaw (float __yaw))
   (setq _pitch (float __pitch))
   (setq _score (round __score))
   (setq _target_lock (round __target_lock))
   (setq _spin_state (round __spin_state))
   (setq _fire_command (round __fire_command))
   (setq _fire_mode (round __fire_mode))
   (setq _x (float __x))
   (setq _y (float __y))
   (setq _z (float __z))
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:track_id
   (&optional __track_id)
   (if __track_id (setq _track_id __track_id)) _track_id)
  (:yaw
   (&optional __yaw)
   (if __yaw (setq _yaw __yaw)) _yaw)
  (:pitch
   (&optional __pitch)
   (if __pitch (setq _pitch __pitch)) _pitch)
  (:score
   (&optional __score)
   (if __score (setq _score __score)) _score)
  (:target_lock
   (&optional __target_lock)
   (if __target_lock (setq _target_lock __target_lock)) _target_lock)
  (:spin_state
   (&optional __spin_state)
   (if __spin_state (setq _spin_state __spin_state)) _spin_state)
  (:fire_command
   (&optional __fire_command)
   (if __fire_command (setq _fire_command __fire_command)) _fire_command)
  (:fire_mode
   (&optional __fire_mode)
   (if __fire_mode (setq _fire_mode __fire_mode)) _fire_mode)
  (:x
   (&optional __x)
   (if __x (setq _x __x)) _x)
  (:y
   (&optional __y)
   (if __y (setq _y __y)) _y)
  (:z
   (&optional __z)
   (if __z (setq _z __z)) _z)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; int8 _track_id
    1
    ;; float32 _yaw
    4
    ;; float32 _pitch
    4
    ;; int8 _score
    1
    ;; int8 _target_lock
    1
    ;; int8 _spin_state
    1
    ;; int8 _fire_command
    1
    ;; int8 _fire_mode
    1
    ;; float32 _x
    4
    ;; float32 _y
    4
    ;; float32 _z
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; int8 _track_id
       (write-byte _track_id s)
     ;; float32 _yaw
       (sys::poke _yaw (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _pitch
       (sys::poke _pitch (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; int8 _score
       (write-byte _score s)
     ;; int8 _target_lock
       (write-byte _target_lock s)
     ;; int8 _spin_state
       (write-byte _spin_state s)
     ;; int8 _fire_command
       (write-byte _fire_command s)
     ;; int8 _fire_mode
       (write-byte _fire_mode s)
     ;; float32 _x
       (sys::poke _x (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _y
       (sys::poke _y (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _z
       (sys::poke _z (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; int8 _track_id
     (setq _track_id (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _track_id 127) (setq _track_id (- _track_id 256)))
   ;; float32 _yaw
     (setq _yaw (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _pitch
     (setq _pitch (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; int8 _score
     (setq _score (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _score 127) (setq _score (- _score 256)))
   ;; int8 _target_lock
     (setq _target_lock (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _target_lock 127) (setq _target_lock (- _target_lock 256)))
   ;; int8 _spin_state
     (setq _spin_state (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _spin_state 127) (setq _spin_state (- _spin_state 256)))
   ;; int8 _fire_command
     (setq _fire_command (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _fire_command 127) (setq _fire_command (- _fire_command 256)))
   ;; int8 _fire_mode
     (setq _fire_mode (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _fire_mode 127) (setq _fire_mode (- _fire_mode 256)))
   ;; float32 _x
     (setq _x (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _y
     (setq _y (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _z
     (setq _z (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get robot_msgs::PTZ_perception :md5sum-) "b2635782c4d3d935ef69981c3144d66c")
(setf (get robot_msgs::PTZ_perception :datatype-) "robot_msgs/PTZ_perception")
(setf (get robot_msgs::PTZ_perception :definition-)
      "Header header
int8 track_id
float32 yaw
float32 pitch
int8 score
int8 target_lock
int8 spin_state
int8 fire_command
int8 fire_mode
float32 x
float32 y
float32 z
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



(provide :robot_msgs/PTZ_perception "b2635782c4d3d935ef69981c3144d66c")


