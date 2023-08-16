import numpy as np

def otsu_threshold(image, width, height):
    GrayScale = 256  # 最高灰度级
    pixelCount = np.zeros(GrayScale, dtype=int)  # 每个灰度值所占像素个数
    pixelPro = np.zeros(GrayScale, dtype=float)  # 每个灰度值所占总像素比例
    Sumpix = 0
    threshold = 0

    # 统计灰度级中每个像素在整幅图像中的个数
    for i in range(30, height-30):
        for j in range(70, width-70):
            Sumpix += 1
            pixelCount[image[i, j]] += 1

    u = 0
    for i in range(GrayScale):
        pixelPro[i] = pixelCount[i] / Sumpix  # 计算每个像素在整幅图像中的比例
        u += i * pixelPro[i]  # 总平均灰度

    maxVariance = 0.0  # 最大类间方差
    w0 = 0
    avgValue = 0  # w0 前景比例，avgValue 前景平均灰度
    for i in range(256):
        w0 += pixelPro[i]  # 假设当前灰度i为阈值, 0~i 灰度像素所占整幅图像的比例即前景比例
        avgValue += i * pixelPro[i]

        variance = ((avgValue / w0 - u) ** 2) * w0 / (1 - w0)  # 类间方差
        if variance > maxVariance:
            maxVariance = variance
            threshold = i

    return threshold