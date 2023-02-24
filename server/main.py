import cv2
import serial as s
import sys

serial = None

def write(command):
    serial.write(command.encode())
    return

def read():
    while True:
        response = serial.readline()
        if response != b'':
            return response.decode('ascii')

def wait_res(message, expect):
    write(message)
    if read() != expect:
        raise Exception('Connection error.')

def auto_paint(img):
    wait_res('PAINT MODE', 'PAINT MODE RDY')

    for row in range(img.shape[0]):
        print('row: ' + str(row))
        command = ''

        for col in range(img.shape[1]):
            pixel = img[row, col]
            # white
            if pixel > 128:
                command += '0'
            else:
                command += '1'

        command +=  '2'
        write(command)

        # wait for complete task
        read()

    print('Auto paint complete!')
    print('If there are glitches, please fix them in JOG & Fix mode.')

    wait_res('PAINT MODE END', 'PAINT MODE END')

def jog_mode(img):
    return

def update_configure():
    wait_res('CFG MODE', 'CFG MODE RDY')

    while True:
        print('Choose property')
        print('1: Paint mode')
        print('2: Button press duration')
        print('3: Button release duration')
        print('0: Return to menu')

        prop = input('> ')
        if prop == '1':
            print('Choose paint mode')
            print('1: Paint white pixels')
            print('2: White pixels pass through')
            val = input('> ')
            # validate
            if val != '1' and val != '2':
                print('Invalid value')
                continue

            if val == '1':
                write('CFG PAINT 1')
            elif val == '2':
                write('CFG PAINT 2')

            wait_res('SET CFG PAINT ' + val, 'CFG PAINT SAVED')

            print('Paint mode updated.')
            continue

        elif prop == '2':
            print('Input a button press duration [ms]')
            val = input('> ')

            wait_res('SET CFG PRESS DUR ' + val, 'CFG PRESS DUR SAVED')

            print('Button press duration updated.')
            continue
        elif prop == '3':
            print('Input a button release duration [ms]')
            val = input('> ')

            wait_res('SET CFG RELEASE DUR ' + val, 'CFG RELEASE DUR SAVED')

            print('Button release duration updated.')

            continue
        elif prop == '0':
            break
        else:
            print('Invalid value')
            continue

    wait_res('CFG MODE END', 'CFG MODE END')

    return

def main(argv):
    path = 'image.bmp'
    if len(argv) >= 1:
        path = argv[0]

    dev = '/dev/cu.usbserial-AG0JT4BI'
    if len(argv) >= 2:
        dev = argv[1]

    serial = s.Serial(dev, 9600, timeout=3)
    if serial is None:
        raise Exception('Could not open serial port.')

    img_src = cv2.imread('image.bmp')
    img = cv2.cvtColor(img_src, cv2.COLOR_BGR2GRAY)

    # validate
    if img is None:
        raise Exception('Failed to load image file.')

    if img.shape[0] > 120:
        raise Exception('Height is too large.')
    if img.shape[1] > 320:
        raise Exception('Width is too large.')

    # Connection check
    print('Checking connection...')
    wait_res('HELLO', 'HELLO')
    print('Connection success.')

    while True:
        print('Choose mode')
        print('1: Auto paint mode')
        print('2: JOG & Fix mode')
        print('3: Update configure')

        mode = input('> ')
        if mode == '1':
            print('Auto paint mode')
            auto_paint(img)
        elif mode == '2':
            print('JOG & Fix mode')
            jog_mode(img)
        elif mode == '3':
            print('Update configure')
            update_configure()

if __name__ == '__main__':
    main(sys.argv)
