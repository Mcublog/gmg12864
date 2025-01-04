
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
        send_command(((x) & 0xf0) >> 4 | 0x10);                                 \
        send_command((x) & 0x0f);                                               \
    } while (0)

#define ST7565_SetY(y)                                                                   \
    do                                                                                   \
    {                                                                                    \
        send_command(((y) & 0x07) | 0xB0);                                      \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif // GMG_MACROS_H