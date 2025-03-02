import asyncio
import socket
import time

# ANSI Input color:
input_color = '\033[32m' # GREEN
# ESP32 IP and port
ESP32_IP = "192.168.4.1"  # Replace with the actual IP address of your ESP32
ESP32_PORT = 4210         # Same port as defined in ESP32 code
# Define the host and port to listen on
hostname = socket.gethostname()
local_ip = socket.gethostbyname(hostname)
print('\033[0m' + local_ip + input_color)
THIS_DEVICE_IP = local_ip
THIS_DEVICE_PORT = 4210

running = True
# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((THIS_DEVICE_IP, THIS_DEVICE_PORT))
sock.setblocking(False)  # Non-blocking mode
# sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
def send_command(command):
    try:
        # Send command to ESP32
        sock.sendto(command.encode(), (ESP32_IP, ESP32_PORT))
        print(colorize_text(f"Sent command: {command}"))
    except Exception as e:
        print(colorize_text(f"Error sending data: {e}"))

def colorize_text(message):
    if ' EMERGENCY]: ' in message:
        return '\033[31m' + message + input_color
    elif 'error' in message.lower():
        return '\033[31m' + message + input_color
    elif ' NOTIF]: ' in message:
        return '\033[93m' + message + input_color
    elif ' INFO ]: ' in message or '==== LHNT Rover Info ====' in message:
        return '\033[96m' + message + input_color
    return '\033[0m' + message + input_color

async def udp_listener():
    loop = asyncio.get_running_loop()
    print(colorize_text(f"Listening for UDP packets on port {THIS_DEVICE_PORT}..."))
    while running:
        try:
            data, addr = await loop.run_in_executor(None, sock.recvfrom, 1024)
            print(colorize_text(data.decode()))
        except BlockingIOError:
            pass

async def read_keyboard_input():
    global running
    while running:
        print(colorize_text("Enter a command (or 'q' to quit)\n"), end="", flush=True)
        command = (await asyncio.to_thread(input)).lower()
        if command in ['q', 'quit', 'exit']:    # type exit to stop running code
            print(colorize_text('Quitting...') + '\033[0m')
            running = False
            break
        send_command(command)  # Example 4-bit command; modify as needed

async def main():
    await asyncio.gather(
        udp_listener(),
        read_keyboard_input()
    )
    print('\033[0m') # reset terminal text color

try:
    asyncio.run(main())
except KeyboardInterrupt:
    print('^C Quit\033[0m\nQuitting...\n') # Graceful exit
    exit()

'''
Command Definitions:
s [number] -> sets the speed to number (PWM value)
t [number] -> sets the auto-stop time delay to number (milliseconds)
                (The rover will automatically stop if a movement command is not sent within this number of milliseconds)
d [number] -> sets the debug message level to number
                (0 - OFF except for emergency which is non-suppressible)
                (1 - Notifications only)
                (2 - Notifs and Info messages)
                (3 - Log messages)
                (4 - All Debug Messages)
r [string] -> registers the IP Address associated with this device with the name string (tells the ESP who this is and where to send messages)
                (For this device, the identifier is 'python' -> r python)
                (The identifier for the arduino uno r4 wifi is 'uno r4' -> uno r4)
i -> prints an info report of the ESP's programmable variables
e -> emergency stop that stops the rover and causes all movement commands to no longer be processed until the ESP is reset.

Movement Commands:
0 -> Stop
1 -> Move Forwards
2 -> Move Backwards
3 -> Turn Left
4 -> Turn Right
5 -> Strafe Left
6 -> Strafe Right
Any other number -> Stop
'''