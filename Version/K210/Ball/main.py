import sensor, lcd,image, time #导入内置的sensor（摄像头）库和lcd（屏幕）库

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

while(True):
    clock.tick()
    img = sensor.snapshot()#从摄像头取一帧图像数据，返回值是一张图像的对象
    ###################寻找小球###################
    ## 通过image的色块方法识别色块，第一个参数是颜色阈值，使用阈值分割器得到，后面的是抄的不知道什么意思
    hole=img.find_blobs([hole_color_threshold],area_threshold=50,pixels_threshold=50)
    if  hole!=null_list:
        hole_max=find_max(hole)
        img.draw_rectangle(hole_max.rect())
    ## 上面的那个返回值的列表，所有色块的列表
    #for i in hole:
        #img.draw_rectangle(i.rect())
    #############################################
    lcd.display(img)#显示图像到 LCD
    print(clock.fps())
