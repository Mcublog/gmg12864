
#ifndef GMG_MACROS_H
#define GMG_MACROS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
    extern "C"
    {
#endif

#define ST7565_SetX(x)                                                                   \
    do                                                                                   \
    {                                                                                    \
        GMG12864_Send_command(((x) & 0xf0) >> 4 | 0x10);                                 \
        GMG12864_Send_command((x) & 0x0f);                                               \
    } while (0)

#define ST7565_SetY(y)                                                                   \
    do                                                                                   \
    {                                                                                    \
        GMG12864_Send_command(((y) & 0x07) | 0xB0);                                      \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif // GMG_MACROS_H