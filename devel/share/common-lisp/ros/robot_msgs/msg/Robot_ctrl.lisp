; Auto-generated. Do not edit!


(cl:in-package robot_msgs-msg)


;//! \htmlinclude Robot_ctrl.msg.html

(cl:defclass <Robot_ctrl> (roslisp-msg-protocol:ros-message)
  ((vx
    :reader vx
    :initarg :vx
    :type cl:float
    :initform 0.0)
   (vy
    :reader vy
    :initarg :vy
    :type cl:float
    :initform 0.0)
   (vw
    :reader vw
    :initarg :vw
    :type cl:float
    :initform 0.0)
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
   (target_lock
    :reader target_lock
    :initarg :target_lock
    :type cl:fixnum
    :initform 0)
   (fire_command
    :reader fire_command
    :initarg :fire_command
    :type cl:fixnum
    :initform 0))
)

(cl:defclass Robot_ctrl (<Robot_ctrl>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <Robot_ctrl>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'Robot_ctrl)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name robot_msgs-msg:<Robot_ctrl> is deprecated: use robot_msgs-msg:Robot_ctrl instead.")))

(cl:ensure-generic-function 'vx-val :lambda-list '(m))
(cl:defmethod vx-val ((m <Robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:vx-val is deprecated.  Use robot_msgs-msg:vx instead.")
  (vx m))

(cl:ensure-generic-function 'vy-val :lambda-list '(m))
(cl:defmethod vy-val ((m <Robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:vy-val is deprecated.  Use robot_msgs-msg:vy instead.")
  (vy m))

(cl:ensure-generic-function 'vw-val :lambda-list '(m))
(cl:defmethod vw-val ((m <Robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:vw-val is deprecated.  Use robot_msgs-msg:vw instead.")
  (vw m))

(cl:ensure-generic-function 'yaw-val :lambda-list '(m))
(cl:defmethod yaw-val ((m <Robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:yaw-val is deprecated.  Use robot_msgs-msg:yaw instead.")
  (yaw m))

(cl:ensure-generic-function 'pitch-val :lambda-list '(m))
(cl:defmethod pitch-val ((m <Robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:pitch-val is deprecated.  Use robot_msgs-msg:pitch instead.")
  (pitch m))

(cl:ensure-generic-function 'target_lock-val :lambda-list '(m))
(cl:defmethod target_lock-val ((m <Robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:target_lock-val is deprecated.  Use robot_msgs-msg:target_lock instead.")
  (target_lock m))

(cl:ensure-generic-function 'fire_command-val :lambda-list '(m))
(cl:defmethod fire_command-val ((m <Robot_ctrl>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_msgs-msg:fire_command-val is deprecated.  Use robot_msgs-msg:fire_command instead.")
  (fire_command m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <Robot_ctrl>) ostream)
  "Serializes a message object of type '<Robot_ctrl>"
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'vx))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'vy))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'vw))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
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
  (cl:let* ((signed (cl:slot-value msg 'target_lock)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 256) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'fire_command)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 256) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <Robot_ctrl>) istream)
  "Deserializes a message object of type '<Robot_ctrl>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'vx) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'vy) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'vw) (roslisp-utils:decode-single-float-bits bits)))
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
      (cl:setf (cl:slot-value msg 'target_lock) (cl:if (cl:< unsigned 128) unsigned (cl:- unsigned 256))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'fire_command) (cl:if (cl:< unsigned 128) unsigned (cl:- unsigned 256))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<Robot_ctrl>)))
  "Returns string type for a message object of type '<Robot_ctrl>"
  "robot_msgs/Robot_ctrl")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'Robot_ctrl)))
  "Returns string type for a message object of type 'Robot_ctrl"
  "robot_msgs/Robot_ctrl")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<Robot_ctrl>)))
  "Returns md5sum for a message object of type '<Robot_ctrl>"
  "086d932d89a70cee1876019fbed8c143")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'Robot_ctrl)))
  "Returns md5sum for a message object of type 'Robot_ctrl"
  "086d932d89a70cee1876019fbed8c143")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<Robot_ctrl>)))
  "Returns full string definition for message of type '<Robot_ctrl>"
  (cl:format cl:nil "float32 vx~%float32 vy~%float32 vw~%float32 yaw~%float32 pitch~%int8 target_lock~%int8 fire_command~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'Robot_ctrl)))
  "Returns full string definition for message of type 'Robot_ctrl"
  (cl:format cl:nil "float32 vx~%float32 vy~%float32 vw~%float32 yaw~%float32 pitch~%int8 target_lock~%int8 fire_command~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <Robot_ctrl>))
  (cl:+ 0
     4
     4
     4
     4
     4
     1
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <Robot_ctrl>))
  "Converts a ROS message object to a list"
  (cl:list 'Robot_ctrl
    (cl:cons ':vx (vx msg))
    (cl:cons ':vy (vy msg))
    (cl:cons ':vw (vw msg))
    (cl:cons ':yaw (yaw msg))
    (cl:cons ':pitch (pitch msg))
    (cl:cons ':target_lock (target_lock msg))
    (cl:cons ':fire_command (fire_command msg))
))
