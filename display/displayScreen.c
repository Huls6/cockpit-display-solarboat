//
// Created by Thijs Hulshof
//

#include "displayScreen.h"
#include "display.h"

void initDashboardScreen(void) {
    for (int i =0; i < 8; i++) {
        drawText("|",i,30);
        drawText("|",i,98);
    }
    drawEmoji("E",0,108);
    drawText("%",1,120);

    drawText("Temp:", 0, 1);
    drawText("*C",1,18);

    drawText("Hoek:", 4,1);
    drawText("*", 5,18);

    drawText("----------------------------------------------",6,0);
    drawText("E: No CAN connected",7,0);
}

//TODO: HANDY INFO
//uint8_t cnt = 0;

// while (1) {
//     drawNumber(cnt, 1, 50);
//     drawNumber(cnt/2,1,1);
//     drawNumber(cnt/5, 5,1);
//     drawNumber(cnt/10,1,102);
//     vTaskDelay(pdMS_TO_TICKS(200));
//
//     cnt++;
//     if (cnt >= 200) {
//         cnt=0;
//     }
// }