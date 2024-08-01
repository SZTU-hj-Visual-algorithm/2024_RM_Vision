; Auto-generated. Do not edit!


(cl:in-package robot_msgs-msg)


;//! \htmlinclude Yaw_Decision.msg.html

(cl:defclass <Yaw_Decision> (roslisp-msg-protocol:ros-message)
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
   (target_lock
    :reader target_lock
    :initarg :target_lock
    :type cl:fixnum
    :initform 0))
)

(cl:defclass Yaw_Decision (<Yaw_Decision>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <Yaw_Decision>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'Yaw_Decision)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name robot_msgs-msg:<Yaw_Decision> is deprecated: use robot_msgs-msg:Yaw_Decision instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <Yaw_Decision>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:header-val is deprecated.  Use robot_msgs-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'yaw-val :lambda-list '(m))
(cl:defmethod yaw-val ((m <Yaw_Decision>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:yaw-val is deprecated.  Use robot_msgs-msg:yaw instead.")
  (yaw m))

(cl:ensure-generic-function 'target_lock-val :lambda-list '(m))
(cl:defmethod target_lock-val ((m <Yaw_Decision>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:target_lock-val is deprecated.  Use robot_msgs-msg:target_lock instead.")
  (target_lock m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <Yaw_Decision>) ostream)
  "Serializes a message object of type '<Yaw_Decision>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'yaw))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let* ((signed (cl:slot-value msg 'target_lock)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 256) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <Yaw_Decision>) istream)
  "Deserializes a message object of type '<Yaw_Decision>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'yaw) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'target_lock) (cl:if (cl:< unsigned 128) unsigned (cl:- unsigned 256))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<Yaw_Decision>)))
  "Returns string type for a message object of type '<Yaw_Decision>"
  "robot_msgs/Yaw_Decision")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'Yaw_Decision)))
  "Returns string type for a message object of type 'Yaw_Decision"
  "robot_msgs/Yaw_Decision")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<Yaw_Decision>)))
  "Returns md5sum for a message object of type '<Yaw_Decision>"
  "c65cfdbaa0bdce5faf6cf2f74ce61b80")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'Yaw_Decision)))
  "Returns md5sum for a message object of type 'Yaw_Decision"
  "c65cfdbaa0bdce5faf6cf2f74ce61b80")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<Yaw_Decision>)))
  "Returns full string definition for message of type '<Yaw_Decision>"
  (cl:format cl:nil "Header header~%float32 yaw~%int8 target_lock~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'Yaw_Decision)))
  "Returns full string definition for message of type 'Yaw_Decision"
  (cl:format cl:nil "Header header~%float32 yaw~%int8 target_lock~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <Yaw_Decision>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     4
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <Yaw_Decision>))
  "Converts a ROS message object to a list"
  (cl:list 'Yaw_Decision
    (cl:cons ':header (header msg))
    (cl:cons ':yaw (yaw msg))
    (cl:cons ':target_lock (target_lock msg))
))
