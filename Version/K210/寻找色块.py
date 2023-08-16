import sensor, lcd,image, time #导入内置的sensor（摄像头）库和lcd（屏幕）库
from machine import UART,Timer
from fpioa_manager import fm

# 摄像头初始化
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)#设置分辨率为QVGA，即320x240
sensor.skip_frames(time = 2000)#摄像头刚启动时，图像质量还没稳定，所以跳过一些图像
sensor.set_hmirror(True) #水平翻转,参数是false和true
sensor.set_vflip(True) #垂直翻转
#LCD
lcd.init(freq=15000000,invert=True)#初始化 LCD， 这里传了一个参数叫freq即频率， 是指定驱动 LCD 的时钟频率，这里是15MHz，可以根据硬件性能调整
lcd.rotation(2) #LCD旋转
#时钟
clock = time.clock()
#映射串口引脚
fm.register(10, fm.fpioa.UART2_RX, force=True)#IIC_SDA
fm.register(9, fm.fpioa.UART2_TX, force=True)#IIC_SCL
#初始化串口
uart = UART(UART.UART2, 115200, read_buf_len=4096)
# 判断列表为空
null_list=[]
# 背景洞的颜色阈值，通过阈值编辑器确定
hole_color_threshold=(30,60,3,127,-3,127)
# 板球系统中球的颜色阈值
#ball_color_threshold

# 返回最大的的色块区域
def find_max(blobs):
    max_size=0
    for blob_temp in blobs:
        if blob_temp[2]*blob_temp[3] > max_size:
            max_blob=blob_temp
            max_size = blob_temp[2]*blob_temp[3]
    return max_blob


green_threshold   = (29, 84, -60, -7, 27, 60)
while True:
    img=sensor.snapshot()
    blobs = img.find_blobs([green_threshold])
    if blobs:
        b=find_max(blobs)
        #for b in blobs:
        if b[4]>5 and b[2]*b[3]>0: ##检测到的色块像素点也就是面积要大防止误判
            tmp=img.draw_rectangle(b[0:4])
            tmp=img.draw_cross(b[5], b[6])
            c=img.get_pixel(b[5], b[6])
            ## 得到坐标
            aim_x=b[5]-160
            aim_y=-(b[6]-120)
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

    lcd.display(img)
