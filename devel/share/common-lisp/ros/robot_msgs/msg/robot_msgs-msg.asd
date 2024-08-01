
(cl:in-package :asdf)

(defsystem "robot_msgs-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :std_msgs-msg
)
  :components ((:file "_package")
    (:file "Robot_ctrl" :depends-on ("_package_Robot_ctrl"))
    (:file "_package_Robot_ctrl" :depends-on ("_package"))
    (:file "Vision" :depends-on ("_package_Vision"))
    (:file "_package_Vision" :depends-on ("_package"))
  ))