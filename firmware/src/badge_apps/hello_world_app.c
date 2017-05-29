#include "app.h"
#include "queue.h"
#include "buttons.h"
#include "ir.h"
#include "flash.h"
// badge
#include "colors.h"
#include "fb.h"
#include "rgb_led.h"
#include "task.h"
#include "badge_menu.h"

#define LED_LVL 50
void hello_world_task(void* p_arg)
{
    //static unsigned char call_count = 0;
    const TickType_t xDelay = 20 / portTICK_PERIOD_MS;
    BaseType_t notify_ret;
    //TaskHandle_t xHandle = xTaskGetHandle("APP Tasks");
    unsigned char redraw = 0;
    union IRpacket_u pkt;
    pkt.p.command = IR_WRITE;
    pkt.p.address = IR_LED;
    pkt.p.badgeId = G_sysData.badgeId;
    //pkt.p.data    = PING_PAIR_REQUEST;
    pkt.p.data = PACKRGB(0, 100, 100);
    
    
    //if(xHandle == NULL)
    //    led(1, 0, 0);
    
    FbTransparentIndex(0);
    FbColor(GREEN);
    FbClear();
    FbMove(45, 45);
    FbWriteLine("QC!");
    FbMove(5, 55);
    FbWriteLine("Do things");
    FbSwapBuffers();
    led(0, 30, 0);
    
    for(;;)
    {
        if(G_touch_pct >0){
            FbMove(16, 16);
            //FbWriteLine((unsigned char) "TOUCH");
            FbWriteLine("TOUCH");
            //print_to_com1("TOUCH\n\r");
                       
            FbMove(100, 132 - (G_touch_pct + 10));
            FbColor(WHITE);
            FbFilledRectangle(5, 15);
            redraw = 1;
            led(0, 0, 0);
            setNote(100 + (G_touch_pct>>1), 768);

        }      
        
        if(BUTTON_PRESSED_AND_CONSUME){          
    
            //IRPair();
            IRqueueSend(pkt);
            setNote(102, 1024);
            FbMove(16, 16);
            FbWriteLine("BTN");
            led(LED_LVL, 0, LED_LVL);
            //print_to_com1("DOWN\n\r");
            redraw = 1;
        }
                
        
        if(DOWN_BTN_AND_CONSUME){
            FbMove(16, 16);
            FbWriteLine("DOWN");
            setNote(103, 1024);
            led(0, LED_LVL, 0);
            //print_to_com1("DOWN\n\r");
            redraw = 1;
        }
        
        if(G_button_cnt > 100){
            FbMove(16, 26);
            FbWriteLine("EXITING");
            FbSwapBuffers();
            led(0,0,0);
            vTaskDelay(xDelay);
            returnToMenus();
        }
        
        if(UP_BTN_AND_CONSUME){
            FbMove(16, 16);
            FbWriteLine("UP");
            setNote(104, 1024);
            led(LED_LVL, LED_LVL, LED_LVL);
            //print_to_com1("UP\n\r");
            redraw = 1;
        }

        if(LEFT_BTN_AND_CONSUME){
            FbMove(16, 16);
            FbWriteLine("LEFT");
            setNote(105, 1024);
            led(LED_LVL, 0, 0);
            //print_to_com1("LEFT\n\r");
            redraw = 1;
        }
        
        if(RIGHT_BTN_AND_CONSUME){
            FbMove(16, 16);
            FbWriteLine("RIGHT");
            led(0, 0, LED_LVL);
            setNote(106, 1024);
            //print_to_com1("RIGHT");
            redraw = 1;
        }        
        
        if(redraw){
            redraw = 0;
            FbSwapBuffers(); 
        }
        
        vTaskDelay(xDelay);

        
    }
    vTaskDelete( NULL );
}

void QC_task(void *p_arg){
    
}