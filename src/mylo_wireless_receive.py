import network, espnow, ujson, sys

# bring up Wi-Fi in station mode
sta = network.WLAN(network.WLAN.IF_STA)
sta.active(True)
sta.disconnect()

# init ESP-NOW
e = espnow.ESPNow()
e.active(True)

# (optional) whitelist the sender so packets get through
sender = b'\x3c\x8a\x1f\xa4\x5d\x0c'
e.add_peer(sender)

print("ESP-NOW → serial bridge ready")

while True:
    host, msg = e.recv()   # blocking
    if not msg:
        continue
    line = msg.decode('utf-8')
    sys.stdout.write(line + "\n")
