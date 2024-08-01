// Auto-generated. Do not edit!

// (in-package robot_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------

class Robot_ctrl {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.vx = null;
      this.vy = null;
      this.vw = null;
      this.yaw = null;
      this.pitch = null;
      this.target_lock = null;
      this.fire_command = null;
    }
    else {
      if (initObj.hasOwnProperty('vx')) {
        this.vx = initObj.vx
      }
      else {
        this.vx = 0.0;
      }
      if (initObj.hasOwnProperty('vy')) {
        this.vy = initObj.vy
      }
      else {
        this.vy = 0.0;
      }
      if (initObj.hasOwnProperty('vw')) {
        this.vw = initObj.vw
      }
      else {
        this.vw = 0.0;
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
      if (initObj.hasOwnProperty('target_lock')) {
        this.target_lock = initObj.target_lock
      }
      else {
        this.target_lock = 0;
      }
      if (initObj.hasOwnProperty('fire_command')) {
        this.fire_command = initObj.fire_command
      }
      else {
        this.fire_command = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type Robot_ctrl
    // Serialize message field [vx]
    bufferOffset = _serializer.float32(obj.vx, buffer, bufferOffset);
    // Serialize message field [vy]
    bufferOffset = _serializer.float32(obj.vy, buffer, bufferOffset);
    // Serialize message field [vw]
    bufferOffset = _serializer.float32(obj.vw, buffer, bufferOffset);
    // Serialize message field [yaw]
    bufferOffset = _serializer.float32(obj.yaw, buffer, bufferOffset);
    // Serialize message field [pitch]
    bufferOffset = _serializer.float32(obj.pitch, buffer, bufferOffset);
    // Serialize message field [target_lock]
    bufferOffset = _serializer.int8(obj.target_lock, buffer, bufferOffset);
    // Serialize message field [fire_command]
    bufferOffset = _serializer.int8(obj.fire_command, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type Robot_ctrl
    let len;
    let data = new Robot_ctrl(null);
    // Deserialize message field [vx]
    data.vx = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [vy]
    data.vy = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [vw]
    data.vw = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [yaw]
    data.yaw = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [pitch]
    data.pitch = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [target_lock]
    data.target_lock = _deserializer.int8(buffer, bufferOffset);
    // Deserialize message field [fire_command]
    data.fire_command = _deserializer.int8(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 22;
  }

  static datatype() {
    // Returns string type for a message object
    return 'robot_msgs/Robot_ctrl';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '086d932d89a70cee1876019fbed8c143';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    float32 vx
    float32 vy
    float32 vw
    float32 yaw
    float32 pitch
    int8 target_lock
    int8 fire_command
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new Robot_ctrl(null);
    if (msg.vx !== undefined) {
      resolved.vx = msg.vx;
    }
    else {
      resolved.vx = 0.0
    }

    if (msg.vy !== undefined) {
      resolved.vy = msg.vy;
    }
    else {
      resolved.vy = 0.0
    }

    if (msg.vw !== undefined) {
      resolved.vw = msg.vw;
    }
    else {
      resolved.vw = 0.0
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

    if (msg.target_lock !== undefined) {
      resolved.target_lock = msg.target_lock;
    }
    else {
      resolved.target_lock = 0
    }

    if (msg.fire_command !== undefined) {
      resolved.fire_command = msg.fire_command;
    }
    else {
      resolved.fire_command = 0
    }

    return resolved;
    }
};

module.exports = Robot_ctrl;
