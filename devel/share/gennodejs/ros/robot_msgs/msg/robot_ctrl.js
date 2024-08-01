// Auto-generated. Do not edit!

// (in-package robot_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class robot_ctrl {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.yaw = null;
      this.pitch = null;
      this.fire_command = null;
      this.fire_mode = null;
      this.is_follow = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('yaw')) {
        this.yaw = initObj.yaw
      }
      else {
        this.yaw = 0.0;
      }
      if (initObj.hasOwnProperty('pitch')) {
        this.pitch = initObj.pitch
      }
      else {
        this.pitch = 0.0;
      }
      if (initObj.hasOwnProperty('fire_command')) {
        this.fire_command = initObj.fire_command
      }
      else {
        this.fire_command = 0;
      }
      if (initObj.hasOwnProperty('fire_mode')) {
        this.fire_mode = initObj.fire_mode
      }
      else {
        this.fire_mode = 0;
      }
      if (initObj.hasOwnProperty('is_follow')) {
        this.is_follow = initObj.is_follow
      }
      else {
        this.is_follow = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type robot_ctrl
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [yaw]
    bufferOffset = _serializer.float32(obj.yaw, buffer, bufferOffset);
    // Serialize message field [pitch]
    bufferOffset = _serializer.float32(obj.pitch, buffer, bufferOffset);
    // Serialize message field [fire_command]
    bufferOffset = _serializer.int8(obj.fire_command, buffer, bufferOffset);
    // Serialize message field [fire_mode]
    bufferOffset = _serializer.int8(obj.fire_mode, buffer, bufferOffset);
    // Serialize message field [is_follow]
    bufferOffset = _serializer.int8(obj.is_follow, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type robot_ctrl
    let len;
    let data = new robot_ctrl(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [yaw]
    data.yaw = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [pitch]
    data.pitch = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [fire_command]
    data.fire_command = _deserializer.int8(buffer, bufferOffset);
    // Deserialize message field [fire_mode]
    data.fire_mode = _deserializer.int8(buffer, bufferOffset);
    // Deserialize message field [is_follow]
    data.is_follow = _deserializer.int8(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    return length + 11;
  }

  static datatype() {
    // Returns string type for a message object
    return 'robot_msgs/robot_ctrl';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'd113cc45537caf169fa198ab9396ee38';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    Header header
    float32 yaw
    float32 pitch
    int8 fire_command
    int8 fire_mode
    int8 is_follow
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
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new robot_ctrl(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.yaw !== undefined) {
      resolved.yaw = msg.yaw;
    }
    else {
      resolved.yaw = 0.0
    }

    if (msg.pitch !== undefined) {
      resolved.pitch = msg.pitch;
    }
    else {
      resolved.pitch = 0.0
    }

    if (msg.fire_command !== undefined) {
      resolved.fire_command = msg.fire_command;
    }
    else {
      resolved.fire_command = 0
    }

    if (msg.fire_mode !== undefined) {
      resolved.fire_mode = msg.fire_mode;
    }
    else {
      resolved.fire_mode = 0
    }

    if (msg.is_follow !== undefined) {
      resolved.is_follow = msg.is_follow;
    }
    else {
      resolved.is_follow = 0
    }

    return resolved;
    }
};

module.exports = robot_ctrl;
