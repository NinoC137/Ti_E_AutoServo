# 条形码识别，显示边界和扫码结果
import sensor
import image
import lcd
import time

clock = time.clock()
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_vflip(1)
sensor.run(1)
sensor.skip_frames(30)
sensor.set_hmirror(1)
while True:
    clock.tick()
    img = sensor.snapshot()     # 从摄像头获取图片，将摄像头对准二维码
    code = img.find_barcodes([0,0,320,240])    # 从图片中查找条形码
    for i in code:
        #  color：红色 scale：字体大小 x_spacing：x轴间距
        img.draw_string(2,2, i.payload(), color=(255,0,0), scale=2, x_spacing=-5) # 屏幕左上角显示扫码结果 红色 scale 字体大小 x_spacing：x轴间距
        #  color：红色 thickness：边缘厚度 fill 设置为True以填充矩形
        img.draw_rectangle(i.rect(), color=(255,0,0), thickness=3)   # 显示条形码边界矩形
        print(i.payload())
    lcd.display(img)
