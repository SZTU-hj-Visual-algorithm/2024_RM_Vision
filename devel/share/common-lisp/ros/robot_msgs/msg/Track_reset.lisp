; Auto-generated. Do not edit!


(cl:in-package robot_msgs-msg)


;//! \htmlinclude Track_reset.msg.html

(cl:defclass <Track_reset> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (Track_id
    :reader Track_id
    :initarg :Track_id
    :type cl:fixnum
    :initform 0)
   (Reset
    :reader Reset
    :initarg :Reset
    :type cl:fixnum
    :initform 0))
)

(cl:defclass Track_reset (<Track_reset>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <Track_reset>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'Track_reset)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name robot_msgs-msg:<Track_reset> is deprecated: use robot_msgs-msg:Track_reset instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <Track_reset>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:header-val is deprecated.  Use robot_msgs-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'Track_id-val :lambda-list '(m))
(cl:defmethod Track_id-val ((m <Track_reset>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:Track_id-val is deprecated.  Use robot_msgs-msg:Track_id instead.")
  (Track_id m))

(cl:ensure-generic-function 'Reset-val :lambda-list '(m))
(cl:defmethod Reset-val ((m <Track_reset>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:Reset-val is deprecated.  Use robot_msgs-msg:Reset instead.")
  (Reset m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <Track_reset>) ostream)
  "Serializes a message object of type '<Track_reset>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (cl:let* ((signed (cl:slot-value msg 'Track_id)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 256) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'Reset)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 256) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <Track_reset>) istream)
  "Deserializes a message object of type '<Track_reset>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'Track_id) (cl:if (cl:< unsigned 128) unsigned (cl:- unsigned 256))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'Reset) (cl:if (cl:< unsigned 128) unsigned (cl:- unsigned 256))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<Track_reset>)))
  "Returns string type for a message object of type '<Track_reset>"
  "robot_msgs/Track_reset")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'Track_reset)))
  "Returns string type for a message object of type 'Track_reset"
  "robot_msgs/Track_reset")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<Track_reset>)))
  "Returns md5sum for a message object of type '<Track_reset>"
  "c12ffa81e03dadac12770fb057786612")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'Track_reset)))
  "Returns md5sum for a message object of type 'Track_reset"
  "c12ffa81e03dadac12770fb057786612")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<Track_reset>)))
  "Returns full string definition for message of type '<Track_reset>"
  (cl:format cl:nil "Header header~%int8 Track_id~%int8 Reset~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'Track_reset)))
  "Returns full string definition for message of type 'Track_reset"
  (cl:format cl:nil "Header header~%int8 Track_id~%int8 Reset~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <Track_reset>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     1
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <Track_reset>))
  "Converts a ROS message object to a list"
  (cl:list 'Track_reset
    (cl:cons ':header (header msg))
    (cl:cons ':Track_id (Track_id msg))
    (cl:cons ':Reset (Reset msg))
))
