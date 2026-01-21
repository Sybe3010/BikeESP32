#include "activityPage.hpp"

ActivityDataPage page1(MAP_AND_2_WIDGETS);
ActivityDataPage page2(NO_MAP_AND_5_WIDGETS);
ActivityDataPage page3(NO_MAP_AND_6_WIDGETS);

uint8_t currentPageIndex = 0;

lv_obj_t *activityPage;

void createActivityPage(){
    static lv_style_t activityPageStyle;
    lv_style_init(&activityPageStyle);
    lv_style_set_margin_bottom(&activityPageStyle, 0);
    lv_style_set_margin_top(&activityPageStyle, 0);
    lv_style_set_margin_left(&activityPageStyle, 0);
    lv_style_set_margin_right(&activityPageStyle, 0);
    lv_style_set_pad_all(&activityPageStyle, 0);

    activityPage = lv_obj_create(NULL);
    lv_obj_set_size(activityPage, 320, 480);
    lv_obj_set_pos(activityPage, 0, 0);
    lv_obj_remove_flag(activityPage, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(activityPage, &activityPageStyle, 0);
    
    page1.dataPage = lv_obj_create(activityPage);
    lv_obj_set_pos(page1.dataPage, 0, 0);
    lv_obj_remove_flag(page1.dataPage, LV_OBJ_FLAG_SCROLLABLE);

    page2.dataPage = lv_obj_create(activityPage);
    lv_obj_set_pos(page2.dataPage, 0, 0);
    lv_obj_remove_flag(page2.dataPage, LV_OBJ_FLAG_SCROLLABLE);

    page3.dataPage = lv_obj_create(activityPage);
    lv_obj_set_pos(page3.dataPage, 0, 0);
    lv_obj_remove_flag(page3.dataPage, LV_OBJ_FLAG_SCROLLABLE);

    currentPageIndex = 1;

    lv_obj_set_size(activityPage, 320, 480);
    lv_obj_set_size(page1.dataPage, 320, 480);
    lv_obj_set_size(page2.dataPage, 320, 480);
    lv_obj_set_size(page3.dataPage, 320, 480);

    //lv_obj_add_flag(page1.dataPage, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page2.dataPage, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(page3.dataPage, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *leftButton = lv_button_create(activityPage);
    lv_obj_set_size(leftButton, 50, 50);
    lv_obj_set_pos(leftButton, 10, 215);
    lv_obj_remove_flag(leftButton, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(leftButton, [](lv_event_t* e){
        switch (currentPageIndex)
        {
            case 1:
                break;
            case 2:
                lv_obj_remove_flag(page1.dataPage, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(page2.dataPage, LV_OBJ_FLAG_HIDDEN);
                currentPageIndex = 1;
                break;
            case 3: 
                lv_obj_remove_flag(page2.dataPage, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(page3.dataPage, LV_OBJ_FLAG_HIDDEN);
                currentPageIndex = 2;
                break;
            default:
                break;
        }
        
    }, LV_EVENT_CLICKED, NULL);
     
    lv_obj_t *rightButton = lv_button_create(activityPage);
    lv_obj_set_size(rightButton, 50, 50);
    lv_obj_set_pos(rightButton, 260, 215);
    lv_obj_remove_flag(leftButton, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(rightButton, [](lv_event_t* e){
        switch (currentPageIndex)
        {
            case 1:
                lv_obj_add_flag(page1.dataPage, LV_OBJ_FLAG_HIDDEN);
                lv_obj_remove_flag(page2.dataPage, LV_OBJ_FLAG_HIDDEN);
                currentPageIndex = 2;
                break;
            case 2:
                lv_obj_add_flag(page2.dataPage, LV_OBJ_FLAG_HIDDEN);
                lv_obj_remove_flag(page3.dataPage, LV_OBJ_FLAG_HIDDEN);
                currentPageIndex = 3;
            case 3: 
                break;
            default:
                break;
        }
    }, LV_EVENT_CLICKED, NULL);

    page1.setWidgetTypes(ACTIVITY_TIMER, ACTIVITY_SPEED);
    page1.createDataPage();

    page2.setWidgetTypes(ACTIVITY_TIMER, ACTIVITY_SPEED, ACTIVITY_TIMER, ACTIVITY_SPEED, ACTIVITY_TIMER);
    page2.createDataPage();

    page3.setWidgetTypes(ACTIVITY_TIMER, ACTIVITY_SPEED, ACTIVITY_TIMER, ACTIVITY_SPEED, ACTIVITY_TIMER, ACTIVITY_SPEED);
    page3.createDataPage();

    // // Maak een LVGL timer die elke seconde checkt of er nieuwe data is
    // lv_timer_t* uiTimer = lv_timer_create(updateScreenData, 1000, NULL);
}

// void updateScreenData(lv_timer_t *t){
//     lv_obj_t* tempActiveTile = lv_tileview_get_tile_active(activityPage);
//     if(tempActiveTile == page1.dataPage){
//         lv_obj_send_event(page1.dataPage, LV_EVENT_VALUE_CHANGED, NULL);
//     } else if(tempActiveTile == page2.dataPage){
//         lv_obj_send_event(page2.dataPage, LV_EVENT_VALUE_CHANGED, NULL);
//     } else if(tempActiveTile == page3.dataPage){
//         lv_obj_send_event(page3.dataPage, LV_EVENT_VALUE_CHANGED, NULL);
//     }
// }