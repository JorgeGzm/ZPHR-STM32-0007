import serial

ser = serial.Serial('/dev/ttyUSB0', 115200)


def is_valid_bytearray(data):
    return all(0 <= byte <= 255 for byte in data)

print("Dados recebidos na porta serial...")
try:
    while True:
        received_data = ser.read(6)

        if (is_valid_bytearray(received_data)):

            print(f"Dado recebido: {received_data}")

            # PAIR BROADCAST
            if received_data == b'a':
                ser.write(b"30")

            # PAIR
            if received_data == b'a':
                ser.write(b"30")

            # RSSI
            if received_data == b'a':
                ser.write(b"30")

            # DECIMAL_PLACE
            if received_data == b'a':
                ser.write(b"30")

            # UNIT
            if received_data == b'a':
                ser.write(b"30")

            # CAPACITY
            if received_data == b'a':
                ser.write(b"30")

            #  LOAD
            if received_data == b'a':
                ser.write(b"30")

except KeyboardInterrupt:
    print("Encerrando o programa.")
finally:
    ser.close()
