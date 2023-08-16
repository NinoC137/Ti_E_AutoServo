import sensor, lcd,image, time #导入内置的sensor（摄像头）库和lcd（屏幕）库
from machine import UART,Timer
from fpioa_manager import fm
import utime as t                   # 导入定时器模块
import math

# 摄像头初始化
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)#设置分辨率为QVGA，即320x240
sensor.skip_frames(time = 2000)#摄像头刚启动时，图像质量还没稳定，所以跳过一些图像
sensor.set_hmirror(True) #水平翻转,参数是false和true
sensor.set_vflip(True) #垂直翻转
# LCD
lcd.init(freq=15000000,invert=True)#初始化 LCD， 这里传了一个参数叫freq即频率， 是指定驱动 LCD 的时钟频率，这里是15MHz，可以根据硬件性能调整
lcd.rotation(2) #LCD旋转
# 时钟
clock = time.clock()
# 映射串口引脚
fm.register(10, fm.fpioa.UART2_RX, force=True)#IIC_SDA
fm.register(9, fm.fpioa.UART2_TX, force=True)#IIC_SCL
# 初始化串口
uart = UART(UART.UART2, 115200, read_buf_len=4096)
# 颜色阈值
red_color_threshold=(30, 100, 15, 127, -40, 127)
#red_color_threshold=(20, 100, -128, 47, -18, 38)
# 空色块
null_blobs=[]
# 标志变量
black_rect_flag=0 #是否要扫一遍黑框
status=0

# 发送偏差给主控芯片
def SendErr(aim_x,aim_y):
    ## 帧格式封包
    str_aim_x=0
    if aim_x>0:
        str_aim_x='+'+str((int)(aim_x/100%10))+str((int)(aim_x/10%10))+str((int)(aim_x%10))
    else:
         str_aim_x='-'+str((int)(-aim_x/100%10))+str((int)(-aim_x/10%10))+str((int)(-aim_x%10))

    str_aim_y=0
    if aim_y>0:
        str_aim_y='+'+str((int)(aim_y/100%10))+str((int)(aim_y/10%10))+str((int)(aim_y%10))
    else:
         str_aim_y='-'+str((int)(-aim_y/100%10))+str((int)(-aim_y/10%10))+str((int)(-aim_y%10))
    ##发送字符串
    buff='x:'+str_aim_x+','+'y:'+str_aim_y+','
    #print(buff)# 发送信息给主控MCU
    print(buff)
    uart.write(buff)

# 返回最大的的色块区域
def find_max(blobs):
    max_size=0
    for blob_temp in blobs:
        if blob_temp[2]*blob_temp[3] > max_size:
            max_blob=blob_temp
            max_size = blob_temp[2]*blob_temp[3]
    return max_blob

# 获得坐标偏差
def GetBias(corner,blobs):
    global status
    if status==0:
        img.draw_circle(corner[0][0], corner[0][1], 10, color = (0, 0, 255), thickness = 2, fill = False)
        deta_x=corner[0][0]-blobs[5]
        deta_y=corner[0][1]-blobs[6]
    elif status ==1:
        img.draw_circle(corner[1][0], corner[1][1], 6, color = (0, 0, 255), thickness = 2, fill = False)
        deta_x=corner[1][0]-blobs[5]
        deta_y=corner[1][1]-blobs[6]
    elif status ==2:
        img.draw_circle(corner[2][0], corner[2][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
        deta_x=corner[2][0]-blobs[5]
        deta_y=corner[2][1]-blobs[6]
    elif status ==3:
        img.draw_circle(corner[3][0], corner[3][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
        deta_x=corner[3][0]-blobs[5]
        deta_y=corner[3][1]-blobs[6]
    if abs(deta_x)<10 and abs(deta_y)<10: status=(status+1)%4
    return deta_x,deta_y

def BlobsFiliter(blobs):
     for blob_temp in red_blobs:
        break

# --------------------------识别黑框----------------------------- #
while black_rect_flag==0:
    img=sensor.snapshot()
    rects=img.find_rects();
    if rects:
        for r in rects:
            # 判断矩形边长是否符合要求
            if r.w()>50 and r.h()>50:
                 # 在屏幕上框出矩形
                 img.draw_rectangle(r.rect(), color = (255, 0, 0), scale = 4)
                 # 在屏幕上圈出矩形角点
                 corner = r.corners()
                 img.draw_circle(corner[0][0], corner[0][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
                 img.draw_circle(corner[1][0], corner[1][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
                 img.draw_circle(corner[2][0], corner[2][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
                 img.draw_circle(corner[3][0], corner[3][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
                 black_rect_flag=1
    lcd.display(img)
    if black_rect_flag==1:
        t.sleep_ms(1000)#延迟一秒告诉已经识别到黑框
# --------------------------------------------------------------

sensor.set_auto_gain(False)#关闭增益要加在前面才能调低曝光
sensor.skip_frames(time = 20)
sensor.set_auto_exposure(False, 1400)#在这里调节曝光度，调节完可以比较清晰地看清激光点
sensor.set_auto_whitebal(False) # turn this off.
sensor.set_auto_gain(False) # 关闭增益（色块识别时必须要关）
#sensor.set_auto_whitebal(False)#关闭白平衡
#sensor.set_auto_gain(0,800)# 关闭自动增益设定为固定增益

while True:
    # 读取一帧图片
    img=sensor.snapshot()
    #img.draw_line(0,60,160,60,color = (100, 255, 100),thickness = 2)
    #img.draw_line(80,120,80,0,color = (100, 255, 0),thickness = 2)
    # 在屏幕上框出矩形
    #img.draw_rectangle(r.rect(), color = (255, 0, 0), scale = 4)
    # 在屏幕上圈出矩形角点
    #corner = r.corners()
    #img.draw_circle(corner[0][0], corner[0][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
    #img.draw_circle(corner[1][0], corner[1][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
    #img.draw_circle(corner[2][0], corner[2][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
    #img.draw_circle(corner[3][0], corner[3][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
    ## --------------------------跟踪激光----------------------------- #
    #red_blobs = img.find_blobs([red_color_threshold])
    red_blobs=img.find_blobs([red_color_threshold],x_stride = 1, y_stride = 1,pixels_threshold=1, area_threshold=2, merge=True,invert = 0)
    if red_blobs:
        blob_temp=find_max(red_blobs)
        if blob_temp:
            tmp=img.draw_rectangle(blob_temp[0:4], color = (0, 255, 0),thickness = 5, fill = False)
            tmp=img.draw_cross(blob_temp[5], blob_temp[6])
            deta_x,deta_y=GetBias(corner,blob_temp)
            if deta_x and deta_y:
                SendErr(deta_x,deta_y)
    # -------------------------------------------------------------- #
    lcd.display(img)
