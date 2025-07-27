import csv
import time
import datetime
import socket
import struct


def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # Allow multiple sockets to use the same PORT number
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    current_time = time.time()
    last_time = current_time
    temp_buffer = []
    humid_buffer = []

    # Bind to the multicast group
    multicast_group = '239.255.10.10'
    server_address = ('', 3838)
    sock.bind(server_address)

    # Tell the operating system to add the socket to the multicast group
    # on all interfaces (this will send IGMP join messages)
    mreq = struct.pack("4sl", socket.inet_aton(multicast_group), socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    print(f"Listening for multicast messages on {multicast_group}:3838...")
    print("IGMP join message sent to subscribe to multicast group")

    print("Listening on UDP port 3838...")

    try:
        while True:
            data, addr = sock.recvfrom(1024)
            humidity, temperature = struct.unpack('<ff', data)
            print(f"From: {addr}, Data: {data.hex()}, Humidity: {humidity}, Temperature: {temperature:.2f}")
            current_time = time.time()
            delta = current_time - last_time
            temp_buffer.append(temperature)
            humid_buffer.append(humidity)

            if delta > 60:
                averaged_temp = sum(temp_buffer) / len(temp_buffer)
                averaged_humid = sum(humid_buffer) / len(humid_buffer)
                record_time = datetime.datetime.fromtimestamp(current_time, datetime.timezone.utc)

                print("UPDATE FILE")
                with open("dht_readings.csv", "a+") as f:
                    writer = csv.writer(f)
                    writer.writerow(
                        [record_time.isoformat(), len(temp_buffer), f"{averaged_humid:.2f}", f"{averaged_temp:.2f}"]
                    )

                temp_buffer.clear()
                humid_buffer.clear()
                last_time = current_time

    except KeyboardInterrupt:
        print("\nLeaving multicast group...")
        # Leave the multicast group
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_DROP_MEMBERSHIP, mreq)
        sock.close()


if __name__ == "__main__":
    main()
