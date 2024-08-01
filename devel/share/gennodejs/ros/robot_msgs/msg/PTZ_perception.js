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

class PTZ_perception {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.track_id = null;
      this.yaw = null;
      this.pitch = null;
      this.score = null;
      this.target_lock = null;
      this.spin_state = null;
      this.fire_command = null;
      this.fire_mode = null;
      this.x = null;
      this.y = null;
      this.z = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('track_id')) {
        this.track_id = initObj.track_id
      }
      else {
        this.track_id = 0;
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
      if (initObj.hasOwnProperty('score')) {
        this.score = initObj.score
      }
      else {
        this.score = 0;
      }
      if (initObj.hasOwnProperty('target_lock')) {
        this.target_lock = initObj.target_lock
      }
      else {
        this.target_lock = 0;
      }
      if (initObj.hasOwnProperty('spin_state')) {
        this.spin_state = initObj.spin_state
      }
      else {
        this.spin_state = 0;
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
      if (initObj.hasOwnProperty('x')) {
        this.x = initObj.x
      }
      else {
        this.x = 0.0;
      }
      if (initObj.hasOwnProperty('y')) {
        this.y = initObj.y
      }
      else {
        this.y = 0.0;
      }
      if (initObj.hasOwnProperty('z')) {
        this.z = initObj.z
      }
      else {
        this.z = 0.0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type PTZ_perception
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [track_id]
    bufferOffset = _serializer.int8(obj.track_id, buffer, bufferOffset);
    // Serialize message field [yaw]
    bufferOffset = _serializer.float32(obj.yaw, buffer, bufferOffset);
    // Serialize message field [pitch]
    bufferOffset = _serializer.float32(obj.pitch, buffer, bufferOffset);
    // Serialize message field [score]
    bufferOffset = _serializer.int8(obj.score, buffer, bufferOffset);
    // Serialize message field [target_lock]
    bufferOffset = _serializer.int8(obj.target_lock, buffer, bufferOffset);
    // Serialize message field [spin_state]
    bufferOffset = _serializer.int8(obj.spin_state, buffer, bufferOffset);
    // Serialize message field [fire_command]
    bufferOffset = _serializer.int8(obj.fire_command, buffer, bufferOffset);
    // Serialize message field [fire_mode]
    bufferOffset = _serializer.int8(obj.fire_mode, buffer, bufferOffset);
    // Serialize message field [x]
    bufferOffset = _serializer.float32(obj.x, buffer, bufferOffset);
    // Serialize message field [y]
    bufferOffset = _serializer.float32(obj.y, buffer, bufferOffset);
    // Serialize message field [z]
    bufferOffset = _serializer.float32(obj.z, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type PTZ_perception
    let len;
    let data = new PTZ_perception(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [track_id]
    data.track_id = _deserializer.int8(buffer, bufferOffset);
    // Deserialize message field [yaw]
    data.yaw = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [pitch]
    data.pitch = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [score]
    data.score = _deserializer.int8(buffer, bufferOffset);
    // Deserialize message field [target_lock]
    data.target_lock = _deserializer.int8(buffer, bufferOffset);
    // Deserialize message field [spin_state]
    data.spin_state = _deserializer.int8(buffer, bufferOffset);
    // Deserialize message field [fire_command]
    data.fire_command = _deserializer.int8(buffer, bufferOffset);
    // Deserialize message field [fire_mode]
    data.fire_mode = _deserializer.int8(buffer, bufferOffset);
    // Deserialize message field [x]
    data.x = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [y]
    data.y = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [z]
    data.z = _deserializer.float32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    return length + 26;
  }

  static datatype() {
    // Returns string type for a message object
    return 'robot_msgs/PTZ_perception';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'b2635782c4d3d935ef69981c3144d66c';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    Header header
    int8 track_id
    float32 yaw
    float32 pitch
    int8 score
    int8 target_lock
    int8 spin_state
    int8 fire_command
    int8 fire_mode
    float32 x
    float32 y
    float32 z
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
    const resolved = new PTZ_perception(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.track_id !== undefined) {
      resolved.track_id = msg.track_id;
    }
    else {
      resolved.track_id = 0
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

    if (msg.score !== undefined) {
      resolved.score = msg.score;
    }
    else {
      resolved.score = 0
    }

    if (msg.target_lock !== undefined) {
      resolved.target_lock = msg.target_lock;
    }
    else {
      resolved.target_lock = 0
    }

    if (msg.spin_state !== undefined) {
      resolved.spin_state = msg.spin_state;
    }
    else {
      resolved.spin_state = 0
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

    if (msg.x !== undefined) {
      resolved.x = msg.x;
    }
    else {
      resolved.x = 0.0
    }

    if (msg.y !== undefined) {
      resolved.y = msg.y;
    }
    else {
      resolved.y = 0.0
    }

    if (msg.z !== undefined) {
      resolved.z = msg.z;
    }
    else {
      resolved.z = 0.0
    }

    return resolved;
    }
};

module.exports = PTZ_perception;
