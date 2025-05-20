import serial, json, time
import qwiic_scmd


ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)


myMotor = qwiic_scmd.QwiicScmd()
R_MTR, L_MTR = 0, 1
FWD, BWD    = 0, 1

def runExample():
    myMotor.begin()
    myMotor.enable()
    myMotor.set_drive(R_MTR, FWD, 0)
    myMotor.set_drive(L_MTR, BWD, 0)
    print("Ready – streaming JSON → motors")

    while True:
        line = ser.readline().decode('utf-8').strip()
        if not line:
            continue

        try:
            data = json.loads(line)
        except json.JSONDecodeError:
            continue


        if isinstance(data, dict):
            left_val  = float(data.get('left',  0))
            right_val = float(data.get('right', 0))
        else:
            left_val  = right_val = float(data)


        left_speed  = int(max(0.0, min(1.0, left_val))  * 255)
        right_speed = int(max(0.0, min(1.0, right_val)) * 255)

        myMotor.set_drive(R_MTR, FWD, right_speed)
        myMotor.set_drive(L_MTR, BWD,  left_speed)

if __name__ == '__main__':
    try:
        runExample()
    except (KeyboardInterrupt, SystemExit):
        print("Shutting down motors…")
        myMotor.disable()
