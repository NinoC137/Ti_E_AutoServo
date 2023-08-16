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
    #img = image.Image(size=(320, 240))  # 图像大小
    img = sensor.snapshot()     # 从摄像头获取图片，将摄像头对准二维码
    res = img.find_qrcodes()    # 从图片中查找所有二维码对象(image.qrcode)列表
    if len(res) > 0:
        #  color：红色 scale：字体大小 x_spacing：x轴间距
        #img.draw_string(2,2, res[0].payload(), color=(255,0,0), scale=2, x_spacing=-5) # 屏幕左上角显示扫码结果 红色 scale 字体大小 x_spacing：x轴间距
        #  color：红色 thickness：边缘厚度 fill 设置为True以填充矩形
        #img.draw_rectangle(res[0].rect(), color=(255,0,0), thickness=3)   # 显示二维码边界矩形
        print(res[0].payload())
    #lcd.display(img)


#'''
#名字：识别二维码信息
#日期：2020.3.19
#作者：咸鱼梦工坊
#说明：识别二维码信息
#思路说明：
#1.导入相关模块
#2.初始化模块
#3.识别二维码

#'''
#import image,sensor,lcd,time

#clock = time.clock()
#lcd.init()
#sensor.reset()
#sensor.set_pixformat(sensor.RGB565)
#sensor.set_framesize(sensor.QVGA)
#sensor.set_vflip(1)   #后置模式
#sensor.skip_frames(30)
#while True:
    #clock.tick()
    #img = sensor.snapshot()
    #res = img.find_qrcodes() #寻找二维码
    #fps =clock.fps()
    #if len(res) > 0:
        #img.draw_string(2,2, res[0].payload(), color=(0,128,0), scale=2)
        #print(res[0].payload())
    #lcd.display(img)


