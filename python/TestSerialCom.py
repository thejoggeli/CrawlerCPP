import serial
import struct
import time
from crccheck.crc import Crc8Maxim
import numpy as np
import ctypes

PACKET_TYPE_JOINT_ANGLE = 0
PACKET_TYPE_LEG_ANGLES = 1

# Open a serial port on the Jetson Xavier NX
serialMetro = serial.Serial(
    port='/dev/ttyTHS0',
    baudrate=115200*8,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    timeout=5
)


def send_packet(packet_type, packet_data):


    # add packet length to start of packet
    packet_size = len(packet_data)+3
    packet_data = bytearray([packet_size, packet_type]) + bytearray(packet_data)

    # add CRC to end of packet
    crc = Crc8Maxim.calc(packet_data)
    packet_data = bytearray(packet_data) + bytearray([crc])

    # write packet
    # print("send_packet: ", end="")
    # print("[", end="")
    # for i in range(packet_size):
    #     if(i > 0):
    #         print(", ", end="")
    #     print(packet_data[i], end="")
    # print("]")
    serialMetro.write(packet_data)

# angle must be in the range [-pi, +pi]
def angle_to_uint16(angle):

    angle_normalized = (angle+np.pi)/(np.pi*2.0)
    angle_16bit = int(angle_normalized*65535.0)
    # print(np.rad2deg(angle), angle_normalized, angle_16bit)
    return ctypes.c_uint16(angle_16bit)

# angle must be in the range [-pi, +pi]
def send_joint_angle(joint_id, angle):

    angle_int16 = angle_to_uint16(angle)
    packet_data = bytes([joint_id]) + bytes(angle_int16)
    send_packet(PACKET_TYPE_JOINT_ANGLE, packet_data)

# angle must be in the range [-pi, +pi]
def send_leg_angles(leg_id, angles):

    angles_int16 = [angle_to_uint16(angle) for angle in angles]
    packet_data = bytes([leg_id])
    for angle_int16 in angles_int16:
        packet_data += bytes(angle_int16)
    send_packet(PACKET_TYPE_LEG_ANGLES, packet_data)

if __name__ == "__main__":

    print(f"SerialMetro.is_open: {serialMetro.is_open}")

    # send a packet to the Metro M4 Express
    packet_data = bytearray([0x01, 0x02, 0x03, 0x04])
    send_packet(packet_type=255, packet_data=packet_data)

    # send the angle of a joint 
    send_joint_angle(joint_id=0, angle=np.deg2rad(-180.0))
    send_joint_angle(joint_id=1, angle=np.deg2rad(-1.0))
    send_joint_angle(joint_id=2, angle=np.deg2rad(0.0))
    send_joint_angle(joint_id=3, angle=np.deg2rad(1.0))
    send_joint_angle(joint_id=4, angle=np.deg2rad(+180.0))

    # send the angles of a leg
    angles = np.array([45.0, 0.0, -180.0, +180.0], dtype=np.float64)

    t_start = time.time()
    while(time.time()-t_start < 10.0):
        send_leg_angles(leg_id=0, angles=np.deg2rad(angles))
        time.sleep(0.001)

    serialMetro.close()