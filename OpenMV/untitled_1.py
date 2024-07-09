import sensor,image
import time
import math
import utime
from pyb import UART
from pyb import Servo
from pyb import Pin, Timer
import os, tf, uos, gc


uart = UART(3,9600)

threshold_index = 1  # 0 for red, 1 for green, 2 for blue


thresholds = [
    (50, 2, -13, 5, -8, 19),  #灰色盲道
    (41, 76, -19, 37, 80, 4),   #黄色盲道
    (5, 26, 52, 16, 56, -11),   #红灯
    (9, 67, -13, -50, 2, 38),
    (28, 85, -14, 7, 3, 60), #黄色盲道
    (69, 87, -22, 36, -20, 1),#坡道
    (71, 86, -100, 114, -101, 87),
    (55, 0, -30, 0, 33, -18),#斑马线
    (17, 65, -47, -16, 1, 37),
]


sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
sensor.set_windowing((240, 240))
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking
clock = time.clock()

s1 = Servo(1) #底座 +是左转  -是右转
s2 = Servo(2) #头 -是抬头 +是低头

s1.angle(0)
time.sleep_ms(500)
s2.angle(45)
time.sleep_ms(500)

# 等待一段时间确保摄像头已经准备好
while not sensor.snapshot():  # 检查图像是否已准备好
    pass

white = (255, 255, 255)
sign = 12#停止判断
flag = 0
Obstacles = 0#障碍物
complete1 = 0
complete2 = 0
complete3 = 0
complete4 = 0
complete5 = 0
complete6 = 0
complete7 = 0
up,down,left,right = 0,0,0,0
turn = 0
receive = 0
K = 5000
blind,red_light,stair,ramp,zebra_crossing=0,0,0,0,0
a = 0
out1,out2=0,0
step = 0
symbol = 0
succeed = 0
c = 0
d = 0
PD,HUISMD,HSMD,LT = 0,0,0,0
number = 0
#神经网络训练标签
net = None
labels = None
min_confidence = 0.5

try:
    # load the model, alloc the model file on the heap if we have at least 64K free after loading
    net = tf.load("trained.tflite", load_to_fb=uos.stat('trained.tflite')[6] > (gc.mem_free() - (64*1024)))
except Exception as e:
    raise Exception('Failed to load "trained.tflite", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')

try:
    labels = [line.rstrip('\n') for line in open("labels.txt")]
except Exception as e:
    raise Exception('Failed to load "labels.txt", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')

colors = [ # Add more colors if you are detecting more than 7 types of classes at once.
    (255,   0,   0),
    (  0, 255,   0),
    (255, 255,   0),
    (  0,   0, 255),
    (255,   0, 255),
    (  0, 255, 255),
    (255, 255, 255),
]

def tran():
    global PD,HUISMD,HSMD,LT
    for i, detection_list in enumerate(net.detect(img, thresholds=[(math.ceil(min_confidence * 255), 255)])):
        if (i == 0): continue # background class
        if (len(detection_list) == 0): continue # no detections for this class?

        print("********** %s **********" % labels[i])
        if labels[i] == 'HSMD':
            HSMD += 1
        elif labels[i] == 'HUISMD':
            HUISMD += 2
        elif labels[i] == 'LT':
            LT += 3
        elif labels[i] == 'PD':
            PD += 4

        for d in detection_list:
            [x, y, w, h] = d.rect()
            center_x = math.floor(x + (w / 2))
            center_y = math.floor(y + (h / 2))
            print('x %d\ty %d' % (center_x, center_y))
            img.draw_rectangle((center_x, center_y, 12,12), color=colors[i], thickness=2)


def panduan():
    global blind,stair,ramp,PD,HUISMD,HSMD,LT,c
    c = max(PD,HUISMD,HSMD,LT)
    if c == HSMD:
        blind = 100
    elif c == HUISMD:
        blind = 100
    elif c == LT:
        stair = 100
    elif c == PD:
        ramp = 100
    else:
        return 0


def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob[2]*blob[3] > max_size:
            max_blob=blob
            max_size = blob[2]*blob[3]

    return max_blob

def search(flag):
    global complete1,complete2,complete3,complete4,complete5,complete6,complete7
    if flag == 1:#左转


        if s1.angle() >=78:

            complete1=1
            return
        else:
             s1.angle(s1.angle()+2)
    if flag == 2:#右转


        if s1.angle() <=-78:

            complete2=1
            return
        else:
            s1.angle(s1.angle()-2)
    if flag == 3:#从右边回正


        if s1.angle() >= 0:

            complete3=1
            return
        else:
            s1.angle(s1.angle()+2)
    if flag == 4:#从左边回正


        if s1.angle() <= 0:

            complete4=1
            return
        else:
            s1.angle(s1.angle()-2)
    if flag == 5:#低头


        if s2.angle() >= 45:

            complete5=1
            return
        else:
            s2.angle(s2.angle()+2)
    if flag == 6:#抬头回正


        if s2.angle() <= 25:

            complete6=1
            return
        else:
            s2.angle(s2.angle()-2)


    if flag == 7:#判断坡道


        if s2.angle() <= 35:

            complete7=1
            return
        else:
            s2.angle(s2.angle()+2)

def seek():
    global blind,red_light,stair,ramp,zebra_crossing,complete1,complete2,complete3,complete4,complete5,complete6,flag,symbol,succeed,step,sign,c
#    gray_img = img.to_grayscale()
    if flag == 0:
        if complete6 == 0:
            search(6)
        elif complete6 == 1:
            blob_red = img.find_blobs([thresholds[2]])#红灯
            if blob_red:
                red_light=100
                print("ok")
                decision(8)
            else:
                initialization()
                print("no")
                flag = 1
    if flag == 1:
        if complete1 == 0:
            search(1)
        elif complete1 == 1:

            tran()
            step += 1
            if step >= 30:
                panduan()
                if c!=0:

                    decision(48)
                else:
                    initialization()

                    flag = 2

    if flag == 2:
        if complete5 == 0:
            search(5)
        elif complete5 == 1:


            tran()
            step+=1
            if step >= 30:
                panduan()
                if c!=0:
                    decision(42)
                else:
                    flag = 3
                    initialization()

    if flag == 3:
        if complete2 == 0:
            search(2)
        elif complete2 == 1 and complete6 == 0:
            search(6)
        elif complete6 == 1:

            tran()
            step+=1
            if step >= 30:
                panduan()
                if c!=0:

                    decision(68)
                else:
                    flag = 4
                    initialization()



    if flag == 4:
        if complete5 == 0:
            search(5)
        elif complete5 == 1:


            tran()
            step += 1
            if step >= 30:
                panduan()
                if c!=0:

                    decision(62)
                else:
                    flag = 5
                    initialization()


    if flag == 5:
        if complete3 == 0:
            search(3)
        elif complete3 == 1:


            tran()
            step+=1
            if step >= 30:
                panduan()
                if c!=0:

                    decision(2)
                else:
                    sign = 1
                    print("错误，请重新开机")
                    initialization()
    if flag == 6:
        print("flag数值溢出")
    print("flag="+str(flag))
    print("step="+str(step))
    print("c="+str(c))

def decision(condition):
    global blind,red_light,stair,ramp,zebra_crossing,sign,flag


    if condition == 42:
        if blind == 100:
            print("检测到盲道")
            sign = 2
        else:
            flag += 1
            initialization()
            print("低头识别到盲道之外的物体")
    elif condition == 62:
        if blind == 100:
            print("检测到盲道")
            sign = 6
        else:
            flag += 1
            initialization()
            print("低头识别到盲道之外的物体")
    elif condition == 48:
        if ramp == 100:
            print("检测到坡道")
            sign = 3
        elif stair == 100:
            print("检测到楼梯")
            flag = 2
            seek()
        else:
            flag += 1
            initialization()
    elif condition == 68:
        if ramp == 100:
            print("检测到坡道")
            sign = 7
        elif stair == 100:
            print("检测到楼梯")
            flag = 4
            seek()
        else:
            flag += 1
            initialization()
    elif condition == 2:
        if blind == 100:
            print("检测到盲道")
            sign = 1
        else:
            flag += 1
            initialization()
            print("低头识别到盲道之外的物体")
    elif condition == 8:
        if red_light == 100:
            print("检测到红灯")
            sign = 10
        else:
            flag += 1
            initialization()

def initialization():
    global complete1,complete2,complete3,complete4,complete5,complete6,turn,blind,red_light,stair,ramp,zebra_crossing,succeed,step,PD,HUISMD,HSMD,LT,d
    complete1,complete2,complete3,complete4,complete5,complete6=0,0,0,0,0,0
    turn = 0
    blind,red_light,stair,ramp,zebra_crossing=0,0,0,0,0
    succeed,step=0,0
    PD,HUISMD,HSMD,LT=0,0,0,0
    d = 0

def zhixian(area):
    global sign
    if area < 1000 :

        sign = 0
        number = 0
        uart.write("S")
        print("s")

def xunji(blobs):

    max_blob = find_max(blobs)

    # 计算色块中心点位置
    center_x = max_blob.cx()
    center_y = max_blob.cy()

    # 计算中心线与摄像头中心点的距离
    distance_to_center = center_x - img.width() // 2

#    max_blob_x = max_blob.x()+max_blob.w()//4
#    max_blob_w = max_blob.w()//2
    img.draw_rectangle(max_blob.x(),max_blob.y(),max_blob.w(),max_blob.h())
    img.draw_cross(max_blob.cx(), max_blob.cy())
    img.draw_line(max_blob.minor_axis_line(), color=(0, 0, 255))
    print(max_blob.w() * max_blob.h())
    # Note - the blob rotation is unique to 0-180 only.
    img.draw_keypoints(
        [(max_blob.cx(), max_blob.cy(), int(math.degrees(max_blob.rotation())))], size=20
    )
    degress = int(math.degrees(max_blob.rotation()))
    area = max_blob.w() * max_blob.h()
    # 将degress限制在0到255的范围内
    degress = max(0, min(degress, 255))
        # 转换为16进制字符串
    if 20<degress<60:

        hex_degrees = hex(20)[2:]  # 去掉前缀0x
        if len(hex_degrees) < 2:
            hex_degrees = '0' + hex_degrees  # 补零
    elif 120<degress<160:
        hex_degrees = hex(160)[2:]  # 去掉前缀0x
        if len(hex_degrees) < 2:
            hex_degrees = '0' + hex_degrees  # 补零
        # 发送16进制数
    else:
        hex_degrees = hex(degress)[2:]  # 去掉前缀0x
        if len(hex_degrees) < 2:
            hex_degrees = '0' + hex_degrees  # 补零
    uart.write(bytes.fromhex(hex_degrees))
    print(int(math.degrees(max_blob.rotation())))
    print(area)
    zhixian(area)

def send_degress(blobs):
    max_blob = find_max(blobs)

    # 计算色块中心点位置
    center_x = max_blob.cx()
    center_y = max_blob.cy()

    # 计算中心线与摄像头中心点的距离
    distance_to_center = center_x - img.width() // 2

    #    max_blob_x = max_blob.x()+max_blob.w()//4
    #    max_blob_w = max_blob.w()//2
    img.draw_rectangle(max_blob.x(),max_blob.y(),max_blob.w(),max_blob.h())
    img.draw_cross(max_blob.cx(), max_blob.cy())
    img.draw_line(max_blob.minor_axis_line(), color=(0, 0, 255))
    print(max_blob.w() * max_blob.h())
    # Note - the blob rotation is unique to 0-180 only.
    img.draw_keypoints(
        [(max_blob.cx(), max_blob.cy(), int(math.degrees(max_blob.rotation())))], size=20
    )
    degress = int(math.degrees(max_blob.rotation()))
    area = max_blob.w() * max_blob.h()
    # 将degress限制在0到255的范围内
    degress = max(0, min(degress, 255))
        # 转换为16进制字符串
    if 20<degress<60:

        hex_degrees = hex(20)[2:]  # 去掉前缀0x
        if len(hex_degrees) < 2:
            hex_degrees = '0' + hex_degrees  # 补零
    elif 120<degress<160:
        hex_degrees = hex(160)[2:]  # 去掉前缀0x
        if len(hex_degrees) < 2:
            hex_degrees = '0' + hex_degrees  # 补零
        # 发送16进制数
    else:
        hex_degrees = hex(degress)[2:]  # 去掉前缀0x
        if len(hex_degrees) < 2:
            hex_degrees = '0' + hex_degrees  # 补零
    uart.write(bytes.fromhex(hex_degrees))
    print(int(math.degrees(max_blob.rotation())))
    print(area)



def delay_time(a):
    start_time = utime.ticks_ms() # 获取当前时间

    # 在此处添加需要延时的代码
    utime.sleep(a) # 添加一秒延时

    end_time = utime.ticks_ms() # 获取当前时间
    elapsed_time = utime.ticks_diff(end_time, start_time) # 计算延时时间

    print("Elapsed time: {} ms".format(elapsed_time))

def adjust(blobs):
    global K
    max_blob = find_max(blobs)
    #        if max_blob.h() > max_blob.w():
            # Draw a rect around the blob.
    center_x = max_blob.cx()
    center_y = max_blob.cy()

                    # 计算中心线与摄像头中心点的距离
    distance_to_center = center_x - img.width() // 2
    rotation_angle = distance_to_center*0.1
    #                print(rotation_angle)

    img.draw_rectangle(max_blob.x(),max_blob.y(),max_blob.w(),max_blob.h())
    img.draw_cross(max_blob.cx(), max_blob.cy())
    img.draw_keypoints(
        [(max_blob.cx(), max_blob.cy(), int(math.degrees(max_blob.rotation())))], size=20
    )
    #                degress = int(math.degrees(max_blob.rotation()))
    if rotation_angle>=0:
        degress = 180-distance_to_center
    elif rotation_angle<0:
        degress = -distance_to_center
    #                print(degress)
    print(rotation_angle)
    degress = max(0, min(degress, 255))
        # 转换为16进制字符串
    if 30<degress<80:

        hex_degrees = hex(30)[2:]  # 去掉前缀0x
        if len(hex_degrees) < 2:
            hex_degrees = '0' + hex_degrees  # 补零
    elif 100<degress<150:
        hex_degrees = hex(150)[2:]  # 去掉前缀0x
        if len(hex_degrees) < 2:
            hex_degrees = '0' + hex_degrees  # 补零
        # 发送16进制数
    else:
        hex_degrees = hex(degress)[2:]  # 去掉前缀0x
        if len(hex_degrees) < 2:
            hex_degrees = '0' + hex_degrees  # 补零
    uart.write(bytes.fromhex(hex_degrees))
    print(int(math.degrees(max_blob.rotation())))
    Lm = (max_blob.h() + max_blob.w()) / 2  # 计算blob的平均尺寸
    length = K/Lm
    angle_str = str(int(rotation_angle))
    img.draw_string(max_blob.cx(), max_blob.cy()-max_blob.h(), angle_str, color=white)
    #                print(length)
    if length < 40:
        uart.write("S")
    #                    sign = 8
        complete6 = 0
        uart.write("S")

while(True):
    clock.tick()
    img = sensor.snapshot()
    if sign == 0:
        seek()
        print("搜索中")

    if sign == 1:
        print("寻迹盲道")
        if complete5 == 0:
            search(5)
        elif complete5 == 1:
            if d == 0:
                tran()
                step += 1
                if step >= 30:
                    c = max(PD,HUISMD,HSMD,LT)
                    if c == HSMD:
                        d = 1
                    elif c == HUISMD:
                        d = 2
            elif d == 1:
                blobs = img.find_blobs([thresholds[1]])

                if blobs:
                    xunji(blobs)
            elif d == 2:
                blobs_hui = img.find_blobs([thresholds[0]])
                if blobs_hui:
                    xunji(blobs_hui)



    if sign == 2:
        print("左转中")
        if complete4 == 0:
            search(4)
        elif complete4 == 1:
            if turn < 1:
                uart.write("Z")
                turn+=1
            else:
                if uart.any():
                    # 读取一个字节的数据
                    data = uart.read(1)
                    if data:
                        # 转换为字符
                        received_char = chr(data[0])
                        if received_char == 't':
                            print("Received 't'")
                            delay_time(1)
                            uart.write("v")
                            sign = 1

    if sign == 3:
        print("左转上坡")
        if complete4 == 0:
            search(4)
        elif complete4 == 1:
            if turn < 1:
                uart.write("Z")
                turn+=1
            else:
                if uart.any():
                    # 读取一个字节的数据
                    data = uart.read(1)
                    if data:
                        # 转换为字符
                        received_char = chr(data[0])
                        if received_char == 't':#*************************************
                            print("Received 't'")
                            uart.write("p")
                            sign = 4

    if sign == 4:
        print("上坡中")


        if complete5 == 0:
            search(5)
        elif complete5 == 1:
            blobs = img.find_blobs([thresholds[1]])
            if blobs:
                send_degress(blobs)
                if uart.any():
                    # 读取一个字节的数据
                    data = uart.read(1)
                    if data:
                        # 转换为字符
                        received_char = chr(data[0])#*********************************
                        if received_char == 'x':
                            print("Received 'x'")
                            delay_time(2)
                            uart.write("b")
                            flag = 0
                            initialization()
                            sign = 1
    if sign == 5:
        print("寻迹红灯5")
        if complete4 == 0:
            search(4)
        elif complete4 == 1:
            blobs = img.find_blobs([thresholds[2]])
            if blobs:
                adjust(blobs)

    if sign == 6:
        print("右转中")
        if complete3 == 0:
            search(3)
        elif complete3 == 1:
            if turn < 1:
                uart.write("Y")
                turn+=1
            else:
                if uart.any():
                    # 读取一个字节的数据
                    data = uart.read(1)
                    if data:
                        # 转换为字符
                        received_char = chr(data[0])
                        if received_char == 't':
                            print("Received 't'")
                            delay_time(2)
                            uart.write("m")
                            sign = 1

    if sign == 7:
        print("右转上坡")
        if complete3 == 0:
            search(3)
        elif complete3 == 1:
            if turn < 1:
                uart.write("Y")
                turn+=1
            else:
                if uart.any():
                    # 读取一个字节的数据
                    data = uart.read(1)
                    if data:
                        # 转换为字符
                        received_char = chr(data[0])
                        if received_char == 't':#*************************************
                            print("Received 't'")
                            delay_time(2)
                            uart.write("p")
                            sign = 4

    if sign == 8:
        print("寻迹红灯8")
        if complete3 == 0:
            search(3)
        elif complete3 == 1:
            blobs = img.find_blobs([thresholds[2]])
            if blobs:
                adjust(blobs)

    if sign == 9:
        print("寻迹红灯9")
        if complete6 == 0:
            search(6)
        elif complete6 == 1:
            blobs = img.find_blobs([thresholds[2]])
            if blobs:
                adjust(blobs)

    if sign == 10:
        print("前方红灯，停止")
        if receive == 0:

            uart.write("h")
            blobs_h1 = img.find_blobs([thresholds[2]])
            if blobs_h1:
                uart.write("S")
                print("S")
            else:
                receive=1
        elif receive == 1:
            blobs_lv = img.find_blobs([thresholds[8]])#绿灯**************************
            if blobs_lv:
                adjust(blobs_lv)
            else:
                uart.write("S")

    if sign == 11:
        print("直走")

        sign = 1


    if sign == 12:
        if number == 0:
            uart.write("v")
            number = 1
        else:
            blobs = img.find_blobs([thresholds[1]])
#            blobs_hui = img.find_blobs([thresholds[0]])
            if blobs:
                xunji(blobs)



    print("sign="+str(sign))
