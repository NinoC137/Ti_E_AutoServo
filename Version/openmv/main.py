# Untitled - By: 30516 - Thu Aug 3 2023

import sensor, image, time
import utime as t                   # 导入定时器模块
from pyb import UART,Timer               #串口模块
import math

# 摄像头初始化
sensor.reset()
#sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
# 串口
uart = UART(3, 115200, timeout_char=1000)
# 定时器
tim = Timer(2, freq=100)      # 使用定时器2创建定时器对象-以1Hz触发
# 时钟
clock = time.clock()
# 颜色阈值
red_color_threshold=(30, 100, 15, 127, -40, 127)
# 标志变量
tim2_flag=0
black_rect_flag=0 #是否要扫一遍黑框
status=3 # 四个直角拐点的状态变量
temp_step=0 #每条边线细分成多少点，用来记录走到第几个点了
# 点的处理
target_point_num=375 #每个边细分成多少个点移动
target_point=[0,0] #目标点
target_point_num_KP=4 #细分点的PID的比例系数
my_corner=[[0,0],[0,0],[0,0],[0,0]]
judge_thr=10

#--------------测试和滤波----------------------#
kernel_size = 1 # 3x3==1, 5x5==2, 7x7==3, etc.

kernel = [-2, -1,  0, \
          -1,  1,  1, \
           0,  1,  2]

grayscale_thres = [(170, 255)]
#--------------------------------------------#

# 定时器2中断函数
def tick(timer):
    global tim2_flag
    if tim2_flag==0:
        tim2_flag=1

# 得到目标点，返回target_point的值
def GetTargetPoint(corner,blobs):
    global target_point_num,status,temp_step
    # 计算细分的点
    target_point=[0,0]
    if status==0:
        target_point[0] = (((corner[3][0] - corner[status][0]) / target_point_num) *temp_step) +corner[status][0]
        target_point[1] = (((corner[3][1] - corner[status][1]) / target_point_num) *temp_step) +corner[status][1]
    else:
        target_point[0] = (((corner[status - 1][0] - corner[status][0]) / target_point_num) *temp_step) +corner[status][0]
        target_point[1] = (((corner[status - 1][1] - corner[status][1]) / target_point_num) *temp_step) +corner[status][1]
    img.draw_circle((int)( target_point[0]), (int)(target_point[1]), 2, color = (0, 0, 255), thickness = 2, fill = False)
    return target_point

# 获得坐标偏差
def GetBias(corner,blobs):
    global temp_step,status,target_point_num
    target_point=GetTargetPoint(corner,blobs)
    deta_x=target_point[0]-blobs[5]
    deta_y=target_point[1]-blobs[6]
    if abs(deta_x)<10 and abs(deta_y)<10:
        temp_step_acc=temp_step+1
        if temp_step_acc==(int)(target_point_num):# 切换角点
            # 动态细分点
            if status==0:
#                if (abs(corner[3][0] - corner[0][0]))>(abs(corner[3][1] - corner[0][1])):target_point_num=abs(corner[3][0] - corner[0][0])/target_point_num_KP
#                else: target_point_num=abs(corner[3][1] - corner[0][1])/target_point_num_KP
                status=3
            else:
#                if (abs(corner[status - 1][0] - corner[status][0]))>(abs(corner[status - 1][1] - corner[status][1])):target_point_num=abs(corner[status - 1][0] - corner[status][0])/target_point_num_KP
#                else: target_point_num=abs(corner[status - 1][1] - corner[status][1])/target_point_num_KP
                status=status-1
            temp_step=0
        else: temp_step=(temp_step_acc)%target_point_num
    return deta_x,deta_y

# 发送偏差给主控芯片
def SendErr(aim_x,aim_y):
   ## 帧格式封包
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
   ##发送字符串
   buff='x:'+str_aim_x+','+'y:'+str_aim_y+','
   print(buff,temp_x,temp_y,aim_x,aim_y)

   uart.write(buff)


# 返回最大的的色块区域
def find_max(blobs):
    max_size=0
    for blob_temp in blobs:
        if blob_temp[2]*blob_temp[3] > max_size:
            max_blob=blob_temp
            max_size = blob_temp[2]*blob_temp[3]
    return max_blob

#定时器中断回调函数
tim.callback(tick)

# ---------------------------识别铅笔框--------------------------- #
#while 1:
#    img = sensor.snapshot()
#    img.draw_line(0,60+1,160,60+1,color = (100, 255, 100),thickness = 2)
#    img.draw_line(80+2,120,80+2,0,color = (100, 255, 0),thickness = 2)
#    t.sleep_ms(500)#延迟一秒告诉已经识别到黑框
#    img.morph(kernel_size , kernel)
#    img = img.histeq(adaptive=True, clip_limit=20)
##    img.binary(grayscale_thres)
#    rects=img.find_rects(threshold=10000)
#    if rects:
#        print(rects)
#        for r in rects:
#            # 判断矩形边长是否符合要求
#            if r.w()<40 and r.h()<40:
#                # 在屏幕上框出矩形
#                img.draw_rectangle(r.rect(), color = (255, 0, 0), scale = 4)
#                # 在屏幕上圈出矩形角点
#                corner = r.corners()
#                img.draw_circle(corner[0][0], corner[0][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
#                img.draw_circle(corner[1][0], corner[1][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
#                img.draw_circle(corner[2][0], corner[2][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
#                img.draw_circle(corner[3][0], corner[3][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
# ------------------------------------------------------------- #
# --------------------------识别黑框----------------------------- #
while black_rect_flag==0:
    img=sensor.snapshot().lens_corr(strength = 1.3, zoom = 1.0)
    rects=img.find_rects(threshold=10000)
    if rects:
        for r in rects:
            # 判断矩形边长是否符合要求
            if r.w()>60 and r.w()<100 and r.h()>60 and r.h()<100:
                 print(r)
                 # 在屏幕上框出矩形
                 img.draw_rectangle(r.rect(), color = (255, 0, 0), scale = 4)
                 # 在屏幕上圈出矩形角点
                 corner = r.corners()
                 img.draw_circle(corner[0][0], corner[0][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                 img.draw_circle(corner[1][0], corner[1][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                 img.draw_circle(corner[2][0], corner[2][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                 img.draw_circle(corner[3][0], corner[3][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                 # 将矩形检测的宽内缩一点，使得目标在黑线中间
                 my_corner[0][0]=corner[0][0]+2
                 my_corner[0][1]=corner[0][1]-2
                 my_corner[1][0]=corner[1][0]-2
                 my_corner[1][1]=corner[1][1]-2
                 my_corner[2][0]=corner[2][0]-2
                 my_corner[2][1]=corner[2][1]+2
                 my_corner[3][0]=corner[3][0]+2
                 my_corner[3][1]=corner[3][1]+2
                 img.draw_circle(my_corner[0][0], my_corner[0][1], 2, color = (255, 0, 0), thickness = 2, fill = False)
                 img.draw_circle(my_corner[1][0], my_corner[1][1], 2, color = (255, 0, 0), thickness = 2, fill = False)
                 img.draw_circle(my_corner[2][0], my_corner[2][1], 2, color = (255, 0, 0), thickness = 2, fill = False)
                 img.draw_circle(my_corner[3][0], my_corner[3][1], 2, color = (255, 0, 0), thickness = 2, fill = False)
                # 求细分点的个数
#                 if abs(corner[status - 1][0] - corner[status][0])>abs(corner[status - 1][1] - corner[status][1]):target_point_num=abs(corner[status - 1][0] - corner[status][0])/target_point_num_KP
#                 else: target_point_num=abs(corner[status - 1][1] - corner[status][1])/target_point_num_KP
                 black_rect_flag=1
    if black_rect_flag==1:
        t.sleep_ms(1000)#延迟一秒告诉已经识别到黑框
# --------------------------------------------------------------

# ------------------------准备激光识别---------------------------- #
sensor.set_auto_gain(False)#关闭增益要加在前面才能调低曝光
sensor.skip_frames(time = 20)
sensor.set_auto_exposure(False, 1700)#在这里调节曝光度，调节完可以比较清晰地看清激光点
sensor.set_auto_whitebal(False) # turn this off.
sensor.set_auto_gain(False) # 关闭增益（色块识别时必须要关）
#-----------------------------------------------------------------#

while(True):
    clock.tick()
    deta_x=0
    deta_y=0
    img = sensor.snapshot().lens_corr(strength = 1.8, zoom = 1.0)
    red_blobs=img.find_blobs([red_color_threshold],x_stride = 1, y_stride = 1,pixels_threshold=1, area_threshold=1, merge=True,invert = 0)
    if red_blobs:
        blob_temp=find_max(red_blobs)
        if blob_temp:
            tmp=img.draw_rectangle(blob_temp[0:4], color = (0, 255, 0),thickness = 5, fill = False)
            tmp=img.draw_cross(blob_temp[5], blob_temp[6])
            deta_x,deta_y=GetBias(my_corner,blob_temp)
    if tim2_flag==1:
        SendErr(deta_x,deta_y)
        tim2_flag=0
#    print(clock.fps())

