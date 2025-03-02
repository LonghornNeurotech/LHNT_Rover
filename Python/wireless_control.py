# this code wirelessly controls the rover by using an integer command as input. 
import socket
import time
# ESP32 IP and port
ESP32_IP = "192.168.4.1"  # Replace with the actual IP address of your ESP32
ESP32_PORT = 1235         # Same port as defined in ESP32 code
# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
def send_command(command):
    try:
        # Send command to ESP32
        sock.sendto(command.encode(), (ESP32_IP, ESP32_PORT))
        print(f"Sent command: {command}")
    except Exception as e:
        print(f"Error sending data: {e}")

# This code is for testing only and not for the actual ML input
# Example loop to send commands periodically
if __name__ == "__main__":
    while True:
        command = input("Enter the integer command: ")
        if command == "exit" :    # type exit to stop running code
            exit()
        send_command(command)  # Example 4-bit command; modify as needed
        time.sleep(1)      # Wait before sending the next command (this can be cleared for faster commands)
        #del input           # clears input to be ready for next input
