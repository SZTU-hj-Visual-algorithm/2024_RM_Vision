
(cl:in-package :asdf)

(defsystem "robot_msgs-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :std_msgs-msg
)
  :components ((:file "_package")
    (:file "PTZ_Yaw" :depends-on ("_package_PTZ_Yaw"))
    (:file "_package_PTZ_Yaw" :depends-on ("_package"))
    (:file "PTZ_perception" :depends-on ("_package_PTZ_perception"))
    (:file "_package_PTZ_perception" :depends-on ("_package"))
    (:file "Track_reset" :depends-on ("_package_Track_reset"))
    (:file "_package_Track_reset" :depends-on ("_package"))
    (:file "Yaw_Decision" :depends-on ("_package_Yaw_Decision"))
    (:file "_package_Yaw_Decision" :depends-on ("_package"))
    (:file "omni_perception" :depends-on ("_package_omni_perception"))
    (:file "_package_omni_perception" :depends-on ("_package"))
    (:file "robot_ctrl" :depends-on ("_package_robot_ctrl"))
    (:file "_package_robot_ctrl" :depends-on ("_package"))
    (:file "usb_camera" :depends-on ("_package_usb_camera"))
    (:file "_package_usb_camera" :depends-on ("_package"))
    (:file "vision" :depends-on ("_package_vision"))
    (:file "_package_vision" :depends-on ("_package"))
  ))