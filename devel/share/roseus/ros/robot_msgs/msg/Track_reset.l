;; Auto-generated. Do not edit!


(when (boundp 'robot_msgs::Track_reset)
  (if (not (find-package "ROBOT_MSGS"))
    (make-package "ROBOT_MSGS"))
  (shadow 'Track_reset (find-package "ROBOT_MSGS")))
(unless (find-package "ROBOT_MSGS::TRACK_RESET")
  (make-package "ROBOT_MSGS::TRACK_RESET"))

(in-package "ROS")
;;//! \htmlinclude Track_reset.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass robot_msgs::Track_reset
  :super ros::object
  :slots (_header _Track_id _Reset ))

(defmethod robot_msgs::Track_reset
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:Track_id __Track_id) 0)
    ((:Reset __Reset) 0)
    )
   (send-super :init)
   (setq _header __header)
   (setq _Track_id (round __Track_id))
   (setq _Reset (round __Reset))
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:Track_id
   (&optional __Track_id)
   (if __Track_id (setq _Track_id __Track_id)) _Track_id)
  (:Reset
   (&optional __Reset)
   (if __Reset (setq _Reset __Reset)) _Reset)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; int8 _Track_id
    1
    ;; int8 _Reset
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; int8 _Track_id
       (write-byte _Track_id s)
     ;; int8 _Reset
       (write-byte _Reset s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; int8 _Track_id
     (setq _Track_id (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _Track_id 127) (setq _Track_id (- _Track_id 256)))
   ;; int8 _Reset
     (setq _Reset (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _Reset 127) (setq _Reset (- _Reset 256)))
   ;;
   self)
  )

(setf (get robot_msgs::Track_reset :md5sum-) "c12ffa81e03dadac12770fb057786612")
(setf (get robot_msgs::Track_reset :datatype-) "robot_msgs/Track_reset")
(setf (get robot_msgs::Track_reset :definition-)
      "Header header
int8 Track_id
int8 Reset
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



(provide :robot_msgs/Track_reset "c12ffa81e03dadac12770fb057786612")


