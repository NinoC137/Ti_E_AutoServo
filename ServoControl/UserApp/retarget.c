//
// Created by nino on 23-6-18.
//

#include "retarget.h"

Point coordinates[200]; //接收边框坐标

#define Usart_Number 20

void uart_printf(const char *format, ...) {
    osMutexWait(printfMutex, portMAX_DELAY);
    char buffer[64];  // 缓冲区用于存储格式化后的字符串
    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);  // 格式化字符串到缓冲区
    va_end(args);

    for (size_t i = 0; buffer[i] != '\0'; ++i) {
        HAL_UART_Transmit(&huart1, (uint8_t *) &buffer[i], 1, HAL_MAX_DELAY);
    }
    osMutexRelease(printfMutex);
}

void uart2_printf(const char *format, ...) {
//    osMutexWait(printfMutex, portMAX_DELAY);
    char buffer[64];  // 缓冲区用于存储格式化后的字符串
    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);  // 格式化字符串到缓冲区
    va_end(args);

    for (size_t i = 0; buffer[i] != '\0'; ++i) {
        HAL_UART_Transmit(&huart2, (uint8_t *) &buffer[i], 1, HAL_MAX_DELAY);
    }
//    osMutexRelease(printfMutex);
}

void uart4_printf(const char *format, ...) {
//    osMutexWait(printfMutex, portMAX_DELAY);
    char buffer[64];  // 缓冲区用于存储格式化后的字符串
    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);  // 格式化字符串到缓冲区
    va_end(args);

    for (size_t i = 0; buffer[i] != '\0'; ++i) {
        HAL_UART_Transmit(&huart4, (uint8_t *) &buffer[i], 1, HAL_MAX_DELAY);
    }
//    osMutexRelease(printfMutex);
}


void ReformatBuffer(uint8_t *buffer, float *afterReformat) {
    uint16_t i, j;
    uint8_t array_flag = 0;//标志位初始为0
    float a_first = 0, a_sec = 0, a_trd = 0, a_4, a_5, a_6;
    float b_first = 0, b_sec = 0, b_trd = 0, b_4, b_5, b_6;
    uint8_t *temp_buf;
    static float temp_1 = 0, temp_2 = 0;
//    static int8_t cnt_1 = 0,cnt_2 = 0;

    temp_buf = buffer;

    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET) {
        /*当出现“a:”和“，“，则输出a：和，之间的数据，出现b：和，类似*/
        for (i = 0; i < Usart_Number; i++) {
            //字符转义, 对照ASCII表 字符型数字-'0'后成为原数字.
            if (temp_buf[i] == 'x' && temp_buf[i + 1] == ':') {
                if (temp_buf[i + 2] == '-') {
                    a_first = (float) temp_buf[i + 3] - '0';//逗号后面的第一个数据被赋值给a_first
                    a_sec = (float) temp_buf[i + 4] - '0';
                    a_trd = (float) temp_buf[i + 5] - '0';
                    a_4 = (float) temp_buf[i + 6] - '0';
                    a_5 = (float) temp_buf[i + 7] - '0';
                    a_6 = (float) temp_buf[i + 8] - '0';
                    afterReformat[0] =
                            -(a_first * 100000 + a_sec * 10000 + a_trd * 1000 + a_4 * 100 + a_5 * 10 + a_6) / 1000;
                } else {
                    a_first = (float) temp_buf[i + 3] - '0';//逗号后面的第一个数据被赋值给a_first
                    a_sec = (float) temp_buf[i + 4] - '0';
                    a_trd = (float) temp_buf[i + 5] - '0';
                    a_4 = (float) temp_buf[i + 6] - '0';
                    a_5 = (float) temp_buf[i + 7] - '0';
                    a_6 = (float) temp_buf[i + 8] - '0';
                    afterReformat[0] =
                            (a_first * 100000 + a_sec * 10000 + a_trd * 1000 + a_4 * 100 + a_5 * 10 + a_6) / 1000;
                }
            } else if (temp_buf[i] == 'y' && temp_buf[i + 1] == ':') {
                if (temp_buf[i + 2] == '-') {
                    b_first = (float) temp_buf[i + 3] - '0';//逗号后面的第一个数据被赋值给b_first
                    b_sec = (float) temp_buf[i + 4] - '0';
                    b_trd = (float) temp_buf[i + 5] - '0';
                    b_4 = (float) temp_buf[i + 6] - '0';
                    b_5 = (float) temp_buf[i + 7] - '0';
                    b_6 = (float) temp_buf[i + 8] - '0';
                    afterReformat[1] =
                            -(b_first * 100000 + b_sec * 10000 + b_trd * 1000 + b_4 * 100 + b_5 * 10 + b_6) / 1000;
                } else {
                    b_first = (float) temp_buf[i + 3] - '0';//逗号后面的第一个数据被赋值给b_first
                    b_sec = (float) temp_buf[i + 4] - '0';
                    b_trd = (float) temp_buf[i + 5] - '0';
                    b_4 = (float) temp_buf[i + 6] - '0';
                    b_5 = (float) temp_buf[i + 7] - '0';
                    b_6 = (float) temp_buf[i + 8] - '0';
                    afterReformat[1] =
                            (b_first * 100000 + b_sec * 10000 + b_trd * 1000 + b_4 * 100 + b_5 * 10 + b_6) / 1000;
                }
            }
        }

        if (afterReformat[0] == 48 || afterReformat[0] == -48 || afterReformat[1] == 48 || afterReformat[1] == -48) {
            afterReformat[0] = 0;
            afterReformat[1] = 0;
        }
        if (afterReformat[0] == 38 || afterReformat[0] == -38 || afterReformat[1] == 38 || afterReformat[1] == -38) {
            afterReformat[0] = 0;
            afterReformat[1] = 0;
        }

        if (afterReformat[1] > 53 && afterReformat[1] < 54){
            afterReformat[1] = 0;
        }
        else if (afterReformat[1] < -53 && afterReformat[1] > -54){
            afterReformat[1] = 0;
        }

        if (afterReformat[0] > 53 && afterReformat[0] < 54){
            afterReformat[0] = 0;
        }
        else if (afterReformat[0] < -53 && afterReformat[0] > -54){
            afterReformat[0] = 0;
        }

        if (afterReformat[1] > 43 && afterReformat[1] < 44){
            afterReformat[1] = 0;
        }
        else if (afterReformat[1] < -43 && afterReformat[1] > -44){
            afterReformat[1] = 0;
        }

        if (afterReformat[0] > 43 && afterReformat[0] < 44){
            afterReformat[0] = 0;
        }
        else if (afterReformat[0] < -43 && afterReformat[0] > -44){
            afterReformat[0] = 0;
        }

        if (afterReformat[0] > 160) {
            afterReformat[0] = 0;
        } else if (afterReformat[0] < -160) {
            afterReformat[0] = 0;
        } else
            temp_1 = afterReformat[0];
        if (afterReformat[1] > 120) {
            afterReformat[1] = 0;
        } else if (afterReformat[1] < -120) {
            afterReformat[1] = 0;
        } else
            temp_2 = afterReformat[1];
    }
}

float Reformat_Float(const char *format) {
    // find ":" or sth else
    char *xPtr = strchr(format, 'c');
    char *colonPtr = strchr(format, ':');
    if (xPtr == NULL && colonPtr == NULL) {
        uart_printf("Invalid input format.\n");
        return 0;
    }

    // 提取冒号后的浮点数部分
    char *floatStr = colonPtr + 1;
    float value = atof(floatStr); // 使用 atof 函数将字符串转换为浮点数

    /*
     * input: uint8_t uartBuffer[];
     * example:
     * uart_printf("Value: %.2f\r\n",Reformat_Float((char*)uartBuffer));
     * */

    return value;
}

uint8_t hexCharToUint8(char c1, char c2) {
    uint8_t value = 0;

    if (c1 >= '0' && c1 <= '9')
        value = (c1 - '0') << 4;
    else if (c1 >= 'A' && c1 <= 'F')
        value = (c1 - 'A' + 10) << 4;

    if (c2 >= '0' && c2 <= '9')
        value |= (c2 - '0');
    else if (c2 >= 'A' && c2 <= 'F')
        value |= (c2 - 'A' + 10);

    return value;
}

uint32_t Reformat_TOF(const char *format) {
    uint8_t dataArray[7] = {0};

    int dataIndex = 0;
    char buffer[3];
    for (int i = 0; i < strlen(format); i += 3) {
        strncpy(buffer, format + i, 2);
        buffer[2] = '\0';  // 添加字符串结尾
        dataArray[dataIndex++] = hexCharToUint8(buffer[0], buffer[1]);
    }

    uint32_t combinedData = 0;
    // 将第一个数组的值放入高8位
    combinedData = ((uint32_t) dataArray[3]) << 24;
    // 将第二个数组的值放入次高8位
    combinedData |= ((uint32_t) dataArray[4]) << 16;
    // 将第三个数组的值放入次低8位
    combinedData |= ((uint32_t) dataArray[5]) << 8;
    // 将第四个数组的值放入低8位
    combinedData |= dataArray[6];

    return combinedData / (1 << 16);
}

int extractCoordinates(const char *inputString, Point *buffer, int bufferSize) {
    int numCoordinates = 0; // 记录已提取的坐标数量
    int i = 0;

    // 检查字符串是否以"@"开头
    if (inputString[i] != '@') {
        return 0;
    }
    i = i + 2;

    // 依次提取坐标
    while (inputString[i] != '\0' && inputString[i] != '$' && numCoordinates < bufferSize) {
        // 检查是否是合法的坐标格式
        if (inputString[i] == '(' && inputString[i + 8] == ')' &&
            (inputString[i + 1] == '+' || inputString[i + 1] == '-') &&
            (inputString[i + 5] == '+' || inputString[i + 5] == '-')) {

            // 提取x和y的值，并存入缓冲区
            int sign_x = (inputString[i + 1] == '+') ? 1 : -1;
            int sign_y = (inputString[i + 5] == '+') ? 1 : -1;
            buffer[numCoordinates].x = sign_x * ((inputString[i + 2] - '0') * 10 + (inputString[i + 3] - '0'));
            buffer[numCoordinates].y = sign_y * ((inputString[i + 6] - '0') * 10 + (inputString[i + 7] - '0'));
            numCoordinates++;
            i += 8; // 跳过已提取的坐标，指向下一个坐标的起始位置
        } else {
            // 坐标格式不正确，继续查找下一个坐标的起始位置
            i++;
        }
    }

    // 检查字符串是否以"$"结尾
    if (inputString[i] != '$') {
        return 0;
    }

    return numCoordinates;

    //example
//
//    const char *inputString = "@:(-019,+000)(-019,-001)(-019,-002)(-019,-003)(-019,-004)(-019,-005)
//    (-019,-006)(-019,-007)(-019,-008)(-019,-009)(-019,-010)(-019,-011)(-019,-012)
//    (-019,-013)(-019,-014)(-019,-015)(-019,-016)(-019,-017)(-020,-018)(-020,-019)(-020,-020)
//    (-020,-021)(-020,-022)(-020,-023)(-020,-024)(-020,-025)(-020,-026)(-020,-027)(-020,-028)
//    (-020,-029)(-020,-030)(-020,-031)(-020,-032)(-020,-033)(-020,-034)(-020,-035)(-020,-036)
//    (-020,-037)(-020,-038)(-020,-039)(-020,-040)(-020,-041)(-020,-042)(-020,-043)(-020,-044)
//    (-020,-045)(-021,-046)(-020,-047)(-019,-047)(-018,-047)(-017,-047)(-016,-046)(-015,-046)
//    (-014,-046)(-013,-045)(-012,-045)(-011,-045)(-010,-044)(-011,-043)(-010,-044)(-009,-044)
//    (-008,-044)(-007,-044)(-006,-043)(-005,-042)(-006,-041)(-005,-040)(-005,-039)(-004,-040)
//    (-003,-039)(-003,-038)(-002,-038)(-001,-037)(-001,-036)(-001,-037)(-001,-038)(+000,-039)
//    (+001,-039)(+002,-038)(+003,-037)(+002,-036)(+001,-036)(+001,-035)(+002,-036)(+003,-036)
//    (+004,-035)(+005,-034)(+005,-033)(+005,-032)(+005,-031)(+004,-030)(+004,-029)(+004,-028)
//    (+005,-027)(+005,-026)(+005,-025)(+005,-024)(+005,-023)(+005,-022)(+005,-021)(+005,-020)
//    (+005,-019)(+005,-018)(+005,-017)(+006,-016)(+006,-015)(+006,-014)(+006,-013)(+006,-012)
//    (+006,-011)(+007,-010)(+007,-009)(+007,-008)(+007,-007)(+007,-006)(+007,-005)(+007,-004)
//    (+007,-003)(+008,-002)(+008,-001)(+008,+000)(+008,+001)(+008,+002)(+008,+003)(+008,+004)
//    (+008,+005)(+008,+006)(+008,+007)(+008,+008)(+008,+009)(+008,+010)(+009,+011)(+009,+012)
//    (+009,+013)(+009,+014)(+009,+015)(+009,+016)(+009,+017)(+009,+018)(+009,+019)(+009,+020)
//    (+008,+021)(+008,+022)(+009,+023)(+009,+024)(+009,+025)(+008,+026)(+008,+027)(+009,+028)
//    (+009,+029)(+009,+030)(+009,+031)(+008,+032)(+008,+033)(+007,+034)(+006,+035)(+005,+035)
//    (+004,+036)(+003,+036)(+002,+037)(+001,+037)(+000,+038)(-001,+038)(-002,+039)(-003,+039)
//    (-004,+039)(-005,+040)(-006,+040)(-007,+040)(-008,+041)(-009,+041)(-010,+042)(-011,+042)
//    (-012,+042)(-013,+043)(-014,+043)(-015,+044)(-016,+043)(-017,+042)(-017,+041)(-017,+040)
//    (-017,+039)(-017,+038)(-017,+037)(-017,+036)(-017,+035)$";
//    extractCoordinates(inputString, coordinates, 200);

}