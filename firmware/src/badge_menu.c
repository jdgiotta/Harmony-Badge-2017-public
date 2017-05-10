#include "app.h"
#include "queue.h"

// badge
#include "colors.h"
#include "fb.h"
#include "rgb_led.h"
#include "badge_menu.h"
#include "badge_apps.h"
//#include "touch_ctmu.h"
#include "buttons.h"
//#include "timers.h"

struct menuStack_t {
   struct menu_t *selectedMenu;
   struct menu_t *currMenu;
};

#define MAX_MENU_DEPTH 8
static unsigned char G_menuCnt=0; // index for G_menuStack

struct menuStack_t G_menuStack[MAX_MENU_DEPTH] = { {0,0} }; // track user traversing menus
struct menu_t *G_selectedMenu = NULL; /* item the cursor is on */
struct menu_t *G_currMenu = NULL; /* init */

#define MAIN_MENU_BKG_COLOR GREY2
#define MAIN_MENU_SCRATCH_X 3
#define MAIN_MENU_SCRATCH_Y 52
#define MAIN_MENU_SCRATCH_WIDTH 120
#define MAIN_MENU_SCRATCH_HEIGHT 73

unsigned char menu_left = 5;
const unsigned char menu_msg_alert[] = "New Message!";
/* these should all be variables or part of a theme */
#define MENU_LEFT menu_left
#define CHAR_WIDTH 9//assetList[G_Fb.font].x
#define CHAR_HEIGHT 8
#define SCAN_BLANK 1 /* blank lines between text entries */
#define TEXTRECT_OFFSET 1 /* text offset within rectangle */

#define RGBPACKED(R,G,B) ( ((unsigned short)(R)<<11) | ((unsigned short)(G)<<6) | (unsigned short)(B) )


struct menu_t *getSelectedMenu() {
    return G_selectedMenu;
}

struct menu_t *getCurrMenu() {
    return G_currMenu;
}

struct menu_t *getMenuStack(unsigned char item) {
    if (item > G_menuCnt) return 0;

    return G_menuStack[G_menuCnt - item].currMenu;
}

struct menu_t *getSelectedMenuStack(unsigned char item) {
    if (item > G_menuCnt) return 0;

    return G_menuStack[G_menuCnt - item].selectedMenu;
}

struct menu_t *display_menu(struct menu_t *menu,
                            struct menu_t *selected,
                            MENU_STYLE style)
{
    static unsigned char cursor_x, cursor_y;
    unsigned char c;
    struct menu_t *root_menu; /* keep a copy in case menu has a bad structure */

    root_menu = menu;

    switch (style)
    {
        //case MAIN_MENU_WITH_TIME_DATE_STYLE:
        case MAIN_MENU_STYLE:
            FbClear();
            FbBackgroundColor(MAIN_MENU_BKG_COLOR);

            FbColor(GREEN);
            FbMove(2,5);
            FbRectangle(123, 120);

            FbColor(CYAN);
            FbMove(1,4);
            FbRectangle(125, 122);

            break;
        case WHITE_ON_BLACK:
            FbClear();
            FbBackgroundColor(BLACK);
            FbTransparentIndex(0);
            break;
        case BLANK:
            break;
    }

    cursor_x = MENU_LEFT;
    cursor_y = 2; // CHAR_HEIGHT;
    FbMove(cursor_x, cursor_y);

    while (1) {
        unsigned char rect_w=0;

        if (menu->attrib & HIDDEN_ITEM) {
            // don't jump out of the menu array if this is the last item!
            if(menu->attrib & LAST_ITEM)
                break;
            else
                menu++;

            continue;
        }

        for (c=0, rect_w=0; (menu->name[c] != 0); c++) rect_w += CHAR_WIDTH;

        if (menu->attrib & VERT_ITEM)
                cursor_y += (CHAR_HEIGHT + 2 * SCAN_BLANK);

        if (!(menu->attrib & HORIZ_ITEM))
                cursor_x = MENU_LEFT;

        // extra decorations for menu items
        switch(style)
        {
            case MAIN_MENU_STYLE:
                break;
            case WHITE_ON_BLACK:
                //FbMove(cursor_x, cursor_y);
                //FbFilledRectangle(rect_w, CHAR_HEIGHT + 2 * SCAN_BLANK);
                break;
            case BLANK:
                break;
        }
        if (selected == menu) {
            // If we happen to be on a skip ITEM, just increment off it
            // The menus() method mostly avoids this, except for some cases
            if (menu->attrib & SKIP_ITEM) selected++;
        }

        if (selected == NULL) {
            if (menu->attrib & DEFAULT_ITEM)
            selected = menu;
        }

        // Determine selected item color
        switch(style)
        {
            case MAIN_MENU_STYLE:
                if (menu == selected)
                {
                    FbColor(YELLOW);

                    FbMove(3, cursor_y+1);
                    FbFilledRectangle(2,8);

                    // Set the selected color for the coming writeline
                    FbColor(GREEN);
                }
                else
                    // unselected writeline color
                    FbColor(GREY16);
                break;
            case WHITE_ON_BLACK:
                FbColor((menu == selected) ? GREEN : WHITE);
                break;
            case BLANK:
                break;
        }


        FbMove(cursor_x+1, cursor_y+1);
        FbWriteLine(menu->name);
        cursor_x += (rect_w + CHAR_WIDTH);
        if (menu->attrib & LAST_ITEM) break;
            menu++;
    }// END WHILE

//    if (G_menuStack[G_menuCnt].currMenu == settings_m) { /* need a setting to enable/disable */
//        unsigned char time[16], date[16], out[32];
//        extern rtccTime G_time;
//        extern rtccDate G_date;
//        getRTCC();
//        time[0] = ((G_time.hour >> 4) & 0xF) + 48;
//        time[1] =  (G_time.hour       & 0xF) + 48;
//        time[2] = ':';
//        time[3] = ((G_time.min >> 4)  & 0xF) + 48;
//        time[4] =  (G_time.min        & 0xF) + 48;
//        time[5] = ' ';
//        time[6] = 0;
//
//        date[0] = ((G_date.mon >> 4) & 0xF) + 48;
//        date[1] =  (G_date.mon       & 0xF) + 48;
//        date[2] = '-';
//        date[3] = ((G_date.mday >> 4) & 0xF) + 48;
//        date[4] =  (G_date.mday       & 0xF) + 48;
//        date[5] = '-';
//        date[6] = ((G_date.year >> 4) & 0xF) + 48;
//        date[7] =  (G_date.year       & 0xF) + 48;
//        date[8] = 0;
//        if (menu->attrib & VERT_ITEM)
//            cursor_y += (CHAR_HEIGHT + 2 * SCAN_BLANK);
//
//        strcpy(out, time);
//        strcat(out, date);
//
//
//        if (!(menu->attrib & HORIZ_ITEM))
//            cursor_x = MENU_LEFT;
//
//    #define FUDGE 4
//        FbColor(WHITE);
//        FbMove(MENU_LEFT, 128 - (CHAR_HEIGHT + 2 * SCAN_BLANK) - FUDGE);
//        FbWriteLine(out);
//    }
    // Display the badge ID in the upper right hand corner of the main menu
    // - This condition doesn't pass on boot, which of these isn't True
//    if((G_currMenu == main_m) && (runningApp == NULL || runningApp == splash_cb)){
//        unsigned char bid[4] = {'0' + G_sysData.badgeId/100 % 10,
//                                '0' + G_sysData.badgeId/10 % 10,
//                                '0' + G_sysData.badgeId % 10,
//                                0};
//        FbColor(WHITE);
//        FbMove(102,7);
//        FbWriteLine(bid);
//
//        FbColor(GREEN);
//        FbMove(100,5);
//        FbRectangle(25, 11);
//    }

    // Display a notification to the user if they have a message
    // But only if they are in the main menu
//    if(new_message && G_menuStack[G_menuCnt].currMenu == main_m && runningApp == NULL){
//        FbMove(4, 69);
//        FbColor(WHITE);
//        FbFilledRectangle(95, 10);
//
//        FbMove(5, 70);
//        FbColor(BLACK);
//        FbWriteWrappedLine(menu_msg_alert);
//
//        FbMove(2, 79);
//        FbColor(WHITE);
//        FbRectangle(123, 46);
//
//        FbMove(5, 81);
//        FbColor(YELLOW);
//        unsigned char c_str[MAX_MSG_LENGTH];
//        // lol - map back to C chars so buffer code can map back to LCD chars :(
//        map_to_c_chars(incoming_message, c_str, MAX_MSG_LENGTH, 0);
//
//        FbWriteWrappedLine(c_str);
//
//        if(msg_relay_remaining){
//            unsigned char tmp_relay[] = {'0' + (msg_relay_remaining/10)%10,
//                                         '0' + msg_relay_remaining%10,
//                                         0};
//            FbMove(100, 69);
//            FbColor(RED);
//            FbFilledRectangle(20, 10);
//
//            FbMove(103, 69);
//
//            FbColor(WHITE);
//            //FbCharacter(tmp_relay);
//            FbWriteLine(tmp_relay);
//        }
//
//        red(20);
//        green(100);
//        blue(20);
//        main_m[4].attrib = VERT_ITEM;
//    }
//    else if(G_menuStack[G_menuCnt].currMenu == main_m){
//        main_m[4].attrib = VERT_ITEM|LAST_ITEM|HIDDEN_ITEM;
//        red(0);
//        green(0);
//        blue(0);
//    }

    /* in case last menu item is a skip */
    if (selected == NULL) selected = root_menu;


    return (selected);
}



struct menu_t main_m[] = {
    
    //{"Badgelandia", VERT_ITEM|DEFAULT_ITEM, FUNCTION,
    //    {badgelandia_task}},            
    {"Badgey Bird", VERT_ITEM|DEFAULT_ITEM, FUNCTION,
        {badgey_bird_task}},
    //{"Screensavers", VERT_ITEM|DEFAULT_ITEM, FUNCTION,
    //    {screensaver_task}},        
        

   //{"Arcade",       VERT_ITEM|DEFAULT_ITEM, MENU, 
   //     {games_m}},
   //{"Transmitters",       VERT_ITEM, MENU, 
   //     {gadgets_m}},
   //{"Schedule",    VERT_ITEM, MENU,
   //     {schedule_main_m}},
   //{"Settings",    VERT_ITEM, MENU, 
   //     {settings_m}},
   //{"Clear Message", VERT_ITEM|LAST_ITEM|HIDDEN_ITEM, FUNCTION, 
   //     {(struct menu_t *)clear_msg_cb}},
   {"", VERT_ITEM|LAST_ITEM|HIDDEN_ITEM, BACK, 
       {NULL}},
} ;


void returnToMenus(){
    TaskHandle_t xHandle = xTaskGetHandle("APP Tasks");
    BaseType_t notify_ret;
    if(xHandle == NULL)
        led(1, 0, 0);
    
    notify_ret = xTaskNotify(xHandle,
                             1u,
                             eSetBits);
    vTaskSuspend(NULL);    
}

//#define DEBUG_PRINT_TO_CDC
void menu_and_manage_task(void *p_arg){
    TaskHandle_t xHandle = NULL;
    uint32_t ulNotifiedValue;
    BaseType_t xReturned;
    struct menu_t *prev_selected_menu = main_m;
    G_currMenu = prev_selected_menu;
    //badgelandia_task(NULL);
    //boot_splash_task(NULL);
    
    for(;;){
        // No running task, display menu or something
        if(xHandle == NULL)
        {
            // Only redraw when we must
            if (prev_selected_menu != G_selectedMenu){
                G_selectedMenu = display_menu(G_currMenu, 
                                              G_selectedMenu, 
                                              MAIN_MENU_STYLE);
                prev_selected_menu = G_selectedMenu;
                FbSwapBuffers();
            }

            if (BUTTON_PRESSED_AND_CONSUME){//BUTTON_PRESSED_AND_CONSUME) {
                // action happened that will result in menu redraw
                //do_animation = 1;
                switch (G_selectedMenu->type) {
//                    case MORE: /* jump to next page of menu */
//                        setNote(173, 2048); /* a */
//                        G_currMenu += PAGESIZE;
//                        G_selectedMenu = G_currMenu;
//                        break;
                    case BACK: /* return from menu */
                        //setNote(154, 2048);
                        if (G_menuCnt == 0) return; /* stack is empty, error or main menu */
                        G_menuCnt--;
                        G_currMenu = G_menuStack[G_menuCnt].currMenu;
                        G_selectedMenu = G_menuStack[G_menuCnt].selectedMenu;
                        //G_selectedMenu = G_currMenu;
                        break;

                    case TEXT: /* maybe highlight if clicked?? */
                        //setNote(145, 2048); /* c */
                        break;

                    case MENU: /* drills down into menu if clicked */
                        //setNote(129, 2048); /* d */
                        G_menuStack[G_menuCnt].currMenu = G_currMenu; /* push onto stack  */
                        G_menuStack[G_menuCnt].selectedMenu = G_selectedMenu;
                        G_menuCnt++;
                        if (G_menuCnt == MAX_MENU_DEPTH) G_menuCnt--; /* too deep, undo */
                        G_currMenu = (struct menu_t *) G_selectedMenu->data.menu; /* go into this menu */
                        //selectedMenu = G_currMenu;
                        G_selectedMenu = NULL;
                        break;

                    case FUNCTION: /* call the function pointer if clicked */
                        //setNote(115, 2048); /* e */
                        //runningApp = G_selectedMenu->data.func;
                        xReturned = xTaskCreate((TaskFunction_t) G_selectedMenu->data.func, //hello_world_task,
                                                "exec_app",
                                                512u, //may want to increase?
                                                NULL,
                                                1u,
                                                &xHandle);                        
                        //(*selectedMenu->data.func)();
                        break;
                    default:
                        break;
                }
            }
            else if (UP_TOUCH_AND_CONSUME) /* handle slider/soft button clicks */ {
                setNote(109, 800); /* f */

                /* make sure not on first menu item */
                if (G_selectedMenu > G_currMenu) {
                    G_selectedMenu--;

                    while (((G_selectedMenu->attrib & SKIP_ITEM) || (G_selectedMenu->attrib & HIDDEN_ITEM))
                            && G_selectedMenu > G_currMenu)
                        G_selectedMenu--;

                    G_selectedMenu = display_menu(G_currMenu, G_selectedMenu, MAIN_MENU_STYLE);
                }
            }/* *** PEB ***** not convinced this should be an else
               both sliders can be pressed then this one will never get handled
            */
            else if (DOWN_TOUCH_AND_CONSUME) {
                setNote(97, 1024); /* g */

                /* make sure not on last menu item */
                if (!(G_selectedMenu->attrib & LAST_ITEM)) {
                    G_selectedMenu++;

                    //Last item should never be a skipped item!!
                    while (((G_selectedMenu->attrib & SKIP_ITEM) || (G_selectedMenu->attrib & HIDDEN_ITEM))
                            && (!(G_selectedMenu->attrib & LAST_ITEM)))
                        G_selectedMenu++;

                    // at this point, may be on last item, if it's hidden, back off of it
                    if((G_selectedMenu->attrib & LAST_ITEM) && (G_selectedMenu->attrib & HIDDEN_ITEM))
                        G_selectedMenu--;

                    G_selectedMenu = display_menu(G_currMenu, G_selectedMenu, MAIN_MENU_STYLE);
                }
            }
        }
        // Manage a running task
        else if(xHandle !=NULL){
            // TODO: Might be able to let task kill itself
            if (xTaskNotifyWait(0, 1u, &ulNotifiedValue, 10 / portTICK_PERIOD_MS)){
                if(ulNotifiedValue & 0x01){
                    vTaskSuspend(xHandle); // doesn't hurt to call suspend here too?
                    vTaskDelete(xHandle);
                    xHandle = NULL;
                    prev_selected_menu = NULL;
                }
            }
        }
        
        //Handle other events? Marshal messages?        
#ifdef DEBUG_PRINT_TO_CDC
        // TODO: this will have a conflict in the notify field with the kill signal
        print_high_water_marks();
        vTaskDelay(200 / portTICK_PERIOD_MS);
#else
        // Doesn't need to be too responsive
        vTaskDelay(50 / portTICK_PERIOD_MS);
#endif
        
    }
}