; Auto-generated. Do not edit!


(cl:in-package robot_msgs-msg)


;//! \htmlinclude robot_ctrl.msg.html

(cl:defclass <robot_ctrl> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (yaw
    :reader yaw
    :initarg :yaw
    :type cl:float
    :initform 0.0)
   (pitch
    :reader pitch
    :initarg :pitch
    :type cl:float
    :initform 0.0)
   (fire_command
    :reader fire_command
    :initarg :fire_command
    :type cl:fixnum
    :initform 0)
   (fire_mode
    :reader fire_mode
    :initarg :fire_mode
    :type cl:fixnum
    :initform 0)
   (is_follow
    :reader is_follow
    :initarg :is_follow
    :type cl:fixnum
    :initform 0))
)

(cl:defclass robot_ctrl (<robot_ctrl>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <robot_ctrl>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'robot_ctrl)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name robot_msgs-msg:<robot_ctrl> is deprecated: use robot_msgs-msg:robot_ctrl instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:header-val is deprecated.  Use robot_msgs-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'yaw-val :lambda-list '(m))
(cl:defmethod yaw-val ((m <robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:yaw-val is deprecated.  Use robot_msgs-msg:yaw instead.")
  (yaw m))

(cl:ensure-generic-function 'pitch-val :lambda-list '(m))
(cl:defmethod pitch-val ((m <robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:pitch-val is deprecated.  Use robot_msgs-msg:pitch instead.")
  (pitch m))

(cl:ensure-generic-function 'fire_command-val :lambda-list '(m))
(cl:defmethod fire_command-val ((m <robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:fire_command-val is deprecated.  Use robot_msgs-msg:fire_command instead.")
  (fire_command m))

(cl:ensure-generic-function 'fire_mode-val :lambda-list '(m))
(cl:defmethod fire_mode-val ((m <robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:fire_mode-val is deprecated.  Use robot_msgs-msg:fire_mode instead.")
  (fire_mode m))

(cl:ensure-generic-function 'is_follow-val :lambda-list '(m))
(cl:defmethod is_follow-val ((m <robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:is_follow-val is deprecated.  Use robot_msgs-msg:is_follow instead.")
  (is_follow m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <robot_ctrl>) ostream)
  "Serializes a message object of type '<robot_ctrl>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'yaw))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'pitch))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let* ((signed (cl:slot-value msg 'fire_command)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 256) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'fire_mode)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 256) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'is_follow)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 256) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <robot_ctrl>) istream)
  "Deserializes a message object of type '<robot_ctrl>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'yaw) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'pitch) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'fire_command) (cl:if (cl:< unsigned 128) unsigned (cl:- unsigned 256))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'fire_mode) (cl:if (cl:< unsigned 128) unsigned (cl:- unsigned 256))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'is_follow) (cl:if (cl:< unsigned 128) unsigned (cl:- unsigned 256))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<robot_ctrl>)))
  "Returns string type for a message object of type '<robot_ctrl>"
  "robot_msgs/robot_ctrl")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'robot_ctrl)))
  "Returns string type for a message object of type 'robot_ctrl"
  "robot_msgs/robot_ctrl")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<robot_ctrl>)))
  "Returns md5sum for a message object of type '<robot_ctrl>"
  "d113cc45537caf169fa198ab9396ee38")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'robot_ctrl)))
  "Returns md5sum for a message object of type 'robot_ctrl"
  "d113cc45537caf169fa198ab9396ee38")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<robot_ctrl>)))
  "Returns full string definition for message of type '<robot_ctrl>"
  (cl:format cl:nil "Header header~%float32 yaw~%float32 pitch~%int8 fire_command~%int8 fire_mode~%int8 is_follow~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'robot_ctrl)))
  "Returns full string definition for message of type 'robot_ctrl"
  (cl:format cl:nil "Header header~%float32 yaw~%float32 pitch~%int8 fire_command~%int8 fire_mode~%int8 is_follow~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <robot_ctrl>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     4
     4
     1
     1
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <robot_ctrl>))
  "Converts a ROS message object to a list"
  (cl:list 'robot_ctrl
    (cl:cons ':header (header msg))
    (cl:cons ':yaw (yaw msg))
    (cl:cons ':pitch (pitch msg))
    (cl:cons ':fire_command (fire_command msg))
    (cl:cons ':fire_mode (fire_mode msg))
    (cl:cons ':is_follow (is_follow msg))
))
