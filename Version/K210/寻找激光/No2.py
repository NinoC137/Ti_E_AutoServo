# Untitled - By: 30516 - 周四 8�? 3 2023

import sensor, lcd,image, time #导入内置的sensor（摄像头）库和lcd（屏幕）�?
from machine import UART,Timer
from fpioa_manager import fm
import utime as t                   # 导入定时器模�?
import math
import struct

# 摄像头初始化
sensor.reset(dual_buff=True)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)#设置分辨率为QVGA，即320x240
sensor.skip_frames(time = 2000)#摄像头刚启动时，图像质量还没稳定，所以跳过一些图�?
sensor.set_hmirror(True) #水平翻转,参数是false和true
sensor.set_vflip(True) #垂直翻转
# LCD
lcd.init(freq=15000000,invert=True)#初始�? LCD�? 这里传了一个参数叫freq即频率， 是指定驱�? LCD 的时钟频率，这里�?15MHz，可以根据硬件性能调整
lcd.rotation(2) #LCD旋转
# 映射串口引脚
fm.register(10, fm.fpioa.UART2_RX, force=True)#IIC_SDA
fm.register(9, fm.fpioa.UART2_TX, force=True)#IIC_SCL
# 初始化串�?
uart = UART(UART.UART2, 115200, read_buf_len=4096)
# 时钟
clock = time.clock()

target_point=[164,122] #目标�?
send_data_flag=0

def on_timer(timer):
    global send_data_flag
    if send_data_flag==0:
        send_data_flag=1

# 定时器初始化
tim = Timer(Timer.TIMER0, Timer.CHANNEL0, mode=Timer.MODE_PERIODIC, period=10, callback=on_timer, arg=on_timer)

# 返回最大的的色块区�?
def find_max(blobs):
    max_size=0
    for blob_temp in blobs:
        if blob_temp[2]*blob_temp[3] > max_size:
            max_blob=blob_temp
            max_size = blob_temp[2]*blob_temp[3]
    return max_blob

# 获得坐标偏差
def GetBias(blobs):
    deta_x=target_point[0]-blobs[5]
    deta_y=target_point[1]-blobs[6]
    return deta_x,deta_y

# 发送偏差给主控芯片
def SendErr(aim_x,aim_y):
   # 帧格式封�?
   temp_x=aim_x*1000
   str_aim_x=0
   if aim_x>0:
       str_aim_x='+'+str((int)(temp_x/100000%10))+str((int)(temp_x/10000%10))+str((int)(temp_x/1000%10))+str((int)(temp_x/100%10))+str((int)(temp_x/10%10))+str((int)(temp_x%10))
   else:
        str_aim_x='-'+str((int)(-temp_x/100000%10))+str((int)(-temp_x/10000%10))+str((int)(-temp_x/1000%10))+str((int)(-temp_x/100%10))+str((int)(-temp_x/10%10))+str((int)(-temp_x%10))

   temp_y=aim_y*1000
   str_aim_y=0
   if aim_y>0:
       str_aim_y='+'+str((int)(temp_y/100000%10))+str((int)(temp_y/10000%10))+str((int)(temp_y/1000%10))+str((int)(temp_y/100%10))+str((int)(temp_y/10%10))+str((int)(temp_y%10))
   else:
        str_aim_y='-'+str((int)(-temp_y/100000%10))+str((int)(-temp_y/10000%10))+str((int)(-temp_y/1000%10))+str((int)(-temp_y/100%10))+str((int)(-temp_y/10%10))+str((int)(-temp_y%10))
   #发送字符串
   buff='x:'+str_aim_x+','+'y:'+str_aim_y+','
   print(buff)

   uart.write(buff)

# ------------------------摄像头位置标�?

#while 1:
    #img = sensor.snapshot()

    #img.draw_line(160, 240, 160, 0,color = (0, 255, 0),thickness = 5)#
    #img.draw_line(0, 120, 320, 120,color = (0, 255, 0),thickness = 5)#
    #lcd.display(img)

# ------------------------准备激光识�?---------------------------- #
sensor.set_auto_exposure(1)                                 # 设置自动曝光
#sensor.set_auto_exposure(0, exposure=120000)               # 设置手动曝光 曝光时间 120000 us

sensor.set_auto_gain(0, gain_db = 2)                       # 设置画面增益 17 dB 影响实时画面亮度
sensor.set_auto_whitebal(0, rgb_gain_db = (0,0,0))          # 设置RGB增益 0 0 0 dB 影响画面色彩呈现效果 �? K210 上无法调节增�? 初步判定是感光元�? ov2640 无法支持
#-----------------------------------------------------------------#


# 颜色阈值y
red_color_threshold=(41, 100, -45, 127, -40, 127)
#red_color_threshold=(30, 100, 15, 127, -40, 127)

while(True):
    clock.tick()

    deta_x=0
    deta_y=0
    # t_1 = time.ticks_ms()
    img = sensor.snapshot()

    img.set_pixel(target_point[0],target_point[1],(0,0,0))
    img.set_pixel(target_point[0]+1,target_point[1]+1,(0,0,0))
    img.set_pixel(target_point[0]-1,target_point[1]-1,(0,0,0))
    img.set_pixel(target_point[0]-1,target_point[1]+1,(0,0,0))
    img.set_pixel(target_point[0]+1,target_point[1]-1,(0,0,0))

    red_blobs=img.find_blobs([red_color_threshold],x_stride = 1, y_stride = 1,pixels_threshold=1, area_threshold=1, merge=True,invert = 0)
    # t_1 = time.ticks_ms() - t_1
    # print(t_1)
    if red_blobs:
        blob_temp=find_max(red_blobs)
        if blob_temp:
            #tmp=img.draw_rectangle(blob_temp[0:4], color = (0, 255, 0),thickness = 5, fill = False)
            tmp=img.draw_cross(blob_temp[5], blob_temp[6])
            deta_x,deta_y=GetBias(blob_temp)
    lcd.display(img)
    if send_data_flag==1:
        SendErr(deta_x,deta_y)
        send_data_flag=0
