# Untitled - By: 30516 - 周四 6月 1 2023

import sensor, lcd,image, time #导入内置的sensor（摄像头）库和lcd（屏幕）库
# 摄像头初始化
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)#设置分辨率为QVGA，即320x240
sensor.skip_frames(time = 2000)#摄像头刚启动时，图像质量还没稳定，所以跳过一些图像
sensor.set_hmirror(true) #水平翻转,参数是false和true
#LCD
lcd.init(freq=15000000,invert=True)#初始化 LCD， 这里传了一个参数叫freq即频率， 是指定驱动 LCD 的时钟频率，这里是15MHz，可以根据硬件性能调整
#时钟
clock = time.clock()

while(True):
    clock.tick()
    img = sensor.snapshot()#从摄像头取一帧图像数据，返回值是一张图像的对象
    lcd.display(img)#显示图像到 LCD
    print(clock.fps())
