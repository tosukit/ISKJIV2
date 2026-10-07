"""ROS 2 -> Arduino GIGA serial bridge for the ISKJI motor controller.

Subscribes:
    drive_cmd       std_msgs/Float32   -100..100 (%)
    excavation_cmd  std_msgs/Float32   -100..100 (%)

Streams "<drive>,<excavation>\\n" to the GIGA at a fixed rate. The stream
doubles as a heartbeat: the GIGA stops the motors if it ever goes quiet.
If a topic goes silent for cmd_timeout_s, that motor is commanded to 0.

Parameters:
    port           "auto" (default) picks /dev/serial/by-id/*Arduino* or *GIGA*,
                   falling back to /dev/ttyACM0. Or give an explicit path.
    baud           115200
    send_rate_hz   50.0   (ints such as 50 are accepted; clamped to 1..500)
    cmd_timeout_s  0.5    (must be > 0)

Run:  python3 serial_bridge.py --ros-args -p port:=/dev/ttyACM0
"""
import glob
import math
import time

import rclpy
import serial
from rcl_interfaces.msg import ParameterDescriptor
from rclpy.node import Node
from std_msgs.msg import Float32

NEVER = -math.inf  


def clamp(x: float) -> float:
    if math.isnan(x):
        return 0.0
    return max(-100.0, min(100.0, x))


def resolve_port(port: str) -> str:
    if port != "auto":
        return port
    matches = sorted(glob.glob("/dev/serial/by-id/*Arduino*") +
                     glob.glob("/dev/serial/by-id/*GIGA*"))
    return matches[0] if matches else "/dev/ttyACM0"


class SerialBridge(Node):
    def __init__(self):
        super().__init__("iskji_bridge")

        
        loose = ParameterDescriptor(dynamic_typing=True)
        self.declare_parameter("port", "auto")
        self.declare_parameter("baud", 115200, loose)
        self.declare_parameter("send_rate_hz", 50.0, loose)
        self.declare_parameter("cmd_timeout_s", 0.5, loose)

        self.port = str(self.get_parameter("port").value)
        self.baud = int(self.get_parameter("baud").value)

        rate = float(self.get_parameter("send_rate_hz").value)
        if not math.isfinite(rate) or rate <= 0.0:
            self.get_logger().warn(f"Invalid send_rate_hz={rate}, using 50.0")
            rate = 50.0
        rate = max(1.0, min(500.0, rate))

        self.cmd_timeout = float(self.get_parameter("cmd_timeout_s").value)
        if not math.isfinite(self.cmd_timeout) or self.cmd_timeout <= 0.0:
            self.get_logger().warn(
                f"Invalid cmd_timeout_s={self.cmd_timeout}, using 0.5")
            self.cmd_timeout = 0.5

        self.drive = 0.0
        self.exc = 0.0
        self.t_drive = NEVER
        self.t_exc = NEVER

        self.ser = None
        self.last_open_try = NEVER

        self.create_subscription(Float32, "drive_cmd", self.on_drive, 10)
        self.create_subscription(Float32, "excavation_cmd", self.on_exc, 10)
        self.create_timer(1.0 / rate, self.tick)

    def on_drive(self, msg: Float32):
        self.drive = clamp(msg.data)
        self.t_drive = time.monotonic()

    def on_exc(self, msg: Float32):
        self.exc = clamp(msg.data)
        self.t_exc = time.monotonic()

    def open_port(self):
        now = time.monotonic()
        if now - self.last_open_try < 1.0:
            return
        self.last_open_try = now
        path = resolve_port(self.port)
        try:
            
            self.ser = serial.Serial(path, self.baud, timeout=0,
                                     write_timeout=0.1)
            self.get_logger().info(f"Opened {path}")
        except (serial.SerialException, OSError) as e:
            self.ser = None
            self.get_logger().warn(f"Cannot open {path}: {e}",
                                   throttle_duration_sec=5.0)

    def tick(self):
        if self.ser is None:
            self.open_port()
            if self.ser is None:
                return

        now = time.monotonic()
        d = self.drive if now - self.t_drive < self.cmd_timeout else 0.0
        e = self.exc if now - self.t_exc < self.cmd_timeout else 0.0

        try:
            self.ser.write(f"{d:.2f},{e:.2f}\n".encode("ascii"))
            self.ser.reset_input_buffer()  # discard GIGA's replies
        except (serial.SerialException, OSError) as ex:
            # includes serial.SerialTimeoutException (a SerialException)
            self.get_logger().error(f"Serial write failed: {ex}")
            try:
                self.ser.close()
            except Exception:
                pass
            self.ser = None

    def destroy_node(self):
        if self.ser is not None:
            try:
                self.ser.write(b"0.00,0.00\n")
                self.ser.close()
            except Exception:
                pass
        super().destroy_node()


def main():
    rclpy.init()
    node = SerialBridge()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == "__main__":
    main()


if __name__ == "__main__":
    main()

