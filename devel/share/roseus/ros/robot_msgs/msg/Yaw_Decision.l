;; Auto-generated. Do not edit!


(when (boundp 'robot_msgs::Yaw_Decision)
  (if (not (find-package "ROBOT_MSGS"))
    (make-package "ROBOT_MSGS"))
  (shadow 'Yaw_Decision (find-package "ROBOT_MSGS")))
(unless (find-package "ROBOT_MSGS::YAW_DECISION")
  (make-package "ROBOT_MSGS::YAW_DECISION"))

(in-package "ROS")
;;//! \htmlinclude Yaw_Decision.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass robot_msgs::Yaw_Decision
  :super ros::object
  :slots (_header _yaw _target_lock ))

(defmethod robot_msgs::Yaw_Decision
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:yaw __yaw) 0.0)
    ((:target_lock __target_lock) 0)
    )
   (send-super :init)
   (setq _header __header)
   (setq _yaw (float __yaw))
   (setq _target_lock (round __target_lock))
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
  (:target_lock
   (&optional __target_lock)
   (if __target_lock (setq _target_lock __target_lock)) _target_lock)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; float32 _yaw
    4
    ;; int8 _target_lock
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
     ;; int8 _target_lock
       (write-byte _target_lock s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; float32 _yaw
     (setq _yaw (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; int8 _target_lock
     (setq _target_lock (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _target_lock 127) (setq _target_lock (- _target_lock 256)))
   ;;
   self)
  )

(setf (get robot_msgs::Yaw_Decision :md5sum-) "c65cfdbaa0bdce5faf6cf2f74ce61b80")
(setf (get robot_msgs::Yaw_Decision :datatype-) "robot_msgs/Yaw_Decision")
(setf (get robot_msgs::Yaw_Decision :definition-)
      "Header header
float32 yaw
int8 target_lock
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



(provide :robot_msgs/Yaw_Decision "c65cfdbaa0bdce5faf6cf2f74ce61b80")


