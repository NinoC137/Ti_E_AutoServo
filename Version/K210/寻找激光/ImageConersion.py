import numpy as np

def otsu_threshold(image, width, height):
    GrayScale = 256  # ��߻Ҷȼ�
    pixelCount = np.zeros(GrayScale, dtype=int)  # ÿ���Ҷ�ֵ��ռ���ظ���
    pixelPro = np.zeros(GrayScale, dtype=float)  # ÿ���Ҷ�ֵ��ռ�����ر���
    Sumpix = 0
    threshold = 0

    # ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
    for i in range(30, height-30):
        for j in range(70, width-70):
            Sumpix += 1
            pixelCount[image[i, j]] += 1

    u = 0
    for i in range(GrayScale):
        pixelPro[i] = pixelCount[i] / Sumpix  # ����ÿ������������ͼ���еı���
        u += i * pixelPro[i]  # ��ƽ���Ҷ�

    maxVariance = 0.0  # �����䷽��
    w0 = 0
    avgValue = 0  # w0 ǰ��������avgValue ǰ��ƽ���Ҷ�
    for i in range(256):
        w0 += pixelPro[i]  # ���赱ǰ�Ҷ�iΪ��ֵ, 0~i �Ҷ�������ռ����ͼ��ı�����ǰ������
        avgValue += i * pixelPro[i]

        variance = ((avgValue / w0 - u) ** 2) * w0 / (1 - w0)  # ��䷽��
        if variance > maxVariance:
            maxVariance = variance
            threshold = i

    return threshold