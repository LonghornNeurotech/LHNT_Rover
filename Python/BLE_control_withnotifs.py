# this code for sure works to send just integers no UI 
import asyncio
from bleak import BleakClient
# Replace with your ESP32's BLE MAC address or BLE device identifier.
# Example (Linux/macOS): address = "AA:BB:CC:11:22:33"
# On Windows, it might be something like: "00000000-0000-0000-0000-000000000000"
# address = "2c:bc:bb:4c:57:3a" # MAKE SURE TO CHANGE/CHECK THIS IP 
address = 'a0:a3:b3:2b:36:26'
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
ESP_INPUT_CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"
ESP_REPORT_CHARACTERISTIC_UUID = "2d21fb45-ea18-4b00-9fc9-53e55e2363be"
ESP_NOTIFY_CHARACTERISTIC_UUID = "a654858a-5cbc-4bde-9f57-5bb967bd25b0"


async def get_input(client):
    while True:
        print("Enter a command (or 'q' to quit) ", end="", flush=True)
        val = await asyncio.to_thread(input)
        if val.lower() == 'q':
            print("Quitting...")
            await client.stop_notify(ESP_NOTIFY_CHARACTERISTIC_UUID)
            break
        # Write the string (as bytes) to the characteristic
        await client.write_gatt_char(ESP_INPUT_CHARACTERISTIC_UUID, val.encode())
        print(f"Sent '{val}' to the ESP32.")

        if val.lower() == 'r':
            # Read the string (from bytes) from the characteristic
            byte_message = await client.read_gatt_char(ESP_REPORT_CHARACTERISTIC_UUID)
            message = byte_message.decode('utf-8')
            print(f"\033[96m{message}\033[0m")

async def notification_listener(client):

    # Define a callback to handle BLE notifications
    def notification_handler(sender, data):
        # Process the received notification data
        message = data.decode('utf-8')
        print(f"\033[93mESP32 Rover Notification: {message}\033[0m")
    
    # Start listening for notifications
    await client.start_notify(ESP_NOTIFY_CHARACTERISTIC_UUID, notification_handler)

async def main():
    print("Connecting to BLE device...")
    async with BleakClient(address) as client:
        if client.is_connected:
            print("Connected to BLE device!")
            await asyncio.gather(
                get_input(client),
                notification_listener(client)
            )   

# Run the main function
asyncio.run(main())
