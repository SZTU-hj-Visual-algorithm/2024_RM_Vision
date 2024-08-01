;; Auto-generated. Do not edit!


(when (boundp 'robot_msgs::Robot_ctrl)
  (if (not (find-package "ROBOT_MSGS"))
    (make-package "ROBOT_MSGS"))
  (shadow 'Robot_ctrl (find-package "ROBOT_MSGS")))
(unless (find-package "ROBOT_MSGS::ROBOT_CTRL")
  (make-package "ROBOT_MSGS::ROBOT_CTRL"))

(in-package "ROS")
;;//! \htmlinclude Robot_ctrl.msg.html


(defclass robot_msgs::Robot_ctrl
  :super ros::object
  :slots (_vx _vy _vw _yaw _pitch _target_lock _fire_command ))

(defmethod robot_msgs::Robot_ctrl
  (:init
   (&key
    ((:vx __vx) 0.0)
    ((:vy __vy) 0.0)
    ((:vw __vw) 0.0)
    ((:yaw __yaw) 0.0)
    ((:pitch __pitch) 0.0)
    ((:target_lock __target_lock) 0)
    ((:fire_command __fire_command) 0)
    )
   (send-super :init)
   (setq _vx (float __vx))
   (setq _vy (float __vy))
   (setq _vw (float __vw))
   (setq _yaw (float __yaw))
   (setq _pitch (float __pitch))
   (setq _target_lock (round __target_lock))
   (setq _fire_command (round __fire_command))
   self)
  (:vx
   (&optional __vx)
   (if __vx (setq _vx __vx)) _vx)
  (:vy
   (&optional __vy)
   (if __vy (setq _vy __vy)) _vy)
  (:vw
   (&optional __vw)
   (if __vw (setq _vw __vw)) _vw)
  (:yaw
   (&optional __yaw)
   (if __yaw (setq _yaw __yaw)) _yaw)
  (:pitch
   (&optional __pitch)
   (if __pitch (setq _pitch __pitch)) _pitch)
  (:target_lock
   (&optional __target_lock)
   (if __target_lock (setq _target_lock __target_lock)) _target_lock)
  (:fire_command
   (&optional __fire_command)
   (if __fire_command (setq _fire_command __fire_command)) _fire_command)
  (:serialization-length
   ()
   (+
    ;; float32 _vx
    4
    ;; float32 _vy
    4
    ;; float32 _vw
    4
    ;; float32 _yaw
    4
    ;; float32 _pitch
    4
    ;; int8 _target_lock
    1
    ;; int8 _fire_command
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _vx
       (sys::poke _vx (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _vy
       (sys::poke _vy (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _vw
       (sys::poke _vw (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _yaw
       (sys::poke _yaw (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _pitch
       (sys::poke _pitch (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; int8 _target_lock
       (write-byte _target_lock s)
     ;; int8 _fire_command
       (write-byte _fire_command s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _vx
     (setq _vx (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _vy
     (setq _vy (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _vw
     (setq _vw (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _yaw
     (setq _yaw (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _pitch
     (setq _pitch (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; int8 _target_lock
     (setq _target_lock (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _target_lock 127) (setq _target_lock (- _target_lock 256)))
   ;; int8 _fire_command
     (setq _fire_command (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _fire_command 127) (setq _fire_command (- _fire_command 256)))
   ;;
   self)
  )

(setf (get robot_msgs::Robot_ctrl :md5sum-) "086d932d89a70cee1876019fbed8c143")
(setf (get robot_msgs::Robot_ctrl :datatype-) "robot_msgs/Robot_ctrl")
(setf (get robot_msgs::Robot_ctrl :definition-)
      "float32 vx
float32 vy
float32 vw
float32 yaw
float32 pitch
int8 target_lock
int8 fire_command
")



(provide :robot_msgs/Robot_ctrl "086d932d89a70cee1876019fbed8c143")


