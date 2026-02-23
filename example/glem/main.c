/**
 * @file main.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright mcublog Copyright (c) 2026
 *
 */
#include <stdint.h>
#include <unistd.h>

#include "src/dev/glem_dev.h"
#include "src/gmg12864lib.h"

int main()
{
    const gmg_dev_t *kDev = glem_dev();
    GMG12864_Init(kDev);

    GMG12864_logo_demonstration();

    usleep(500 * 1000);

    GMG12864_Puts(0, 0, "Hello");
    GMG12864_Update();

    return 0;
}