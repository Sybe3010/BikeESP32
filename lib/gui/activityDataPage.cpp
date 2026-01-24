#include "activityDataPage.hpp"

Maps activityMap = Maps();

// Lock voor thread-safe canvas access
portMUX_TYPE canvasLock = portMUX_INITIALIZER_UNLOCKED;

ActivityDataPage::ActivityDataPage(){
    _layout = NO_MAP_AND_6_WIDGETS;
}

ActivityDataPage::ActivityDataPage(DataPageLayout layout){
    _layout = layout;
}

ActivityDataPage::ActivityDataPage(DataPageLayout layout, bool customData){
    _customData = customData;
    _layout = layout;
}

ActivityDataPage::~ActivityDataPage(){
    // Opruimen van timer indien nodig
    if(activityMapTimer != nullptr){
        lv_timer_del(activityMapTimer);
        activityMapTimer = nullptr;
    }
}

void ActivityDataPage::setWidgetTypes(typeData widget1, typeData widget2){
    WidgetsOnDataPage newDataPageWidget1;
    Widget newWidget1(dataPage);
    newDataPageWidget1.widget = newWidget1;
    newDataPageWidget1.typeOfData = widget1;
    newDataPageWidget1.widgetId = 1;
    widgets.push_back(newDataPageWidget1);

    WidgetsOnDataPage newDataPageWidget2;
    Widget newWidget2(dataPage);
    newDataPageWidget2.widget = newWidget2;
    newDataPageWidget2.typeOfData = widget2;
    newDataPageWidget2.widgetId = 2;
    widgets.push_back(newDataPageWidget2);
}

void ActivityDataPage::setWidgetTypes(typeData widget1, typeData widget2, typeData widget3, typeData widget4, typeData widget5){
    setWidgetTypes(widget1, widget2);
    WidgetsOnDataPage newDataPageWidget3;
    Widget newWidget3(dataPage);
    newDataPageWidget3.widget = newWidget3;
    newDataPageWidget3.typeOfData = widget3;
    newDataPageWidget3.widgetId = 3;
    widgets.push_back(newDataPageWidget3);

    WidgetsOnDataPage newDataPageWidget4;
    Widget newWidget4(dataPage);
    newDataPageWidget4.widget = newWidget4;
    newDataPageWidget4.typeOfData = widget4;
    newDataPageWidget4.widgetId = 4;
    widgets.push_back(newDataPageWidget4);

    WidgetsOnDataPage newDataPageWidget5;
    Widget newWidget5(dataPage);
    newDataPageWidget5.widget = newWidget5;
    newDataPageWidget5.typeOfData = widget5;
    newDataPageWidget5.widgetId = 5;
    widgets.push_back(newDataPageWidget5);
}

void ActivityDataPage::setWidgetTypes(typeData widget1, typeData widget2, typeData widget3, typeData widget4, typeData widget5, typeData widget6){
    setWidgetTypes(widget1, widget2, widget3, widget4, widget5);

    WidgetsOnDataPage newDataPageWidget6;
    Widget newWidget6(dataPage);
    newDataPageWidget6.widget = newWidget6;
    newDataPageWidget6.typeOfData = widget6;
    newDataPageWidget6.widgetId = 6;
    widgets.push_back(newDataPageWidget6);
}

void ActivityDataPage::createDataPage(){
    if(_layout == NO_MAP_AND_6_WIDGETS){
        create6Widget();
    }
    if(_layout == NO_MAP_AND_5_WIDGETS){
        create5Widget();
    }
    if(_layout == MAP_AND_2_WIDGETS){
        create2WidgetAndMap();
    }

    //lv_obj_add_event_cb(dataPage, event_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

void ActivityDataPage::create2WidgetAndMap(){
    activityMap.initMap(320, 480);  // Match canvas size
    activityMap.createMapScrSprites();

    activityMapCanvas = lv_canvas_create(dataPage);
    lv_obj_set_size(activityMapCanvas, 320, 480);
    lv_obj_set_pos(activityMapCanvas, 0, 0);    

    activityMap.generateMap(15);
    
    if(activityMap.redrawMap){
        activityMap.displayMap();
        // Byte swap toepassen op de RGB565 buffer voor juiste kleurweergave
        lv_draw_sw_rgb565_swap(activityMap.mapBuffer, 320 * 480);

        // Buffer instellen op canvas - beschermd met lock
        portENTER_CRITICAL(&canvasLock);
        lv_canvas_set_buffer(activityMapCanvas, activityMap.mapBuffer, 320, 480, LV_COLOR_FORMAT_RGB565);
        portEXIT_CRITICAL(&canvasLock);
    }

    widgets[0].widget.createWidget(128, 115, 20, 32, widgets[0].typeOfData);
    widgets[0].widget.createWidgetLabel(5, 2);

    widgets[1].widget.createWidget(128, 115, 174, 32, widgets[1].typeOfData);
    widgets[1].widget.createWidgetLabel(5, 2);
    
    // Opruimen van oude timer indien nog bestaand
    if(activityMapTimer != nullptr){
        lv_timer_del(activityMapTimer);
        activityMapTimer = nullptr;
    }
    
    // Pointer naar dit object voor gebruik in de timer lambda
    ActivityDataPage* pThis = this;
    activityMapTimer = lv_timer_create([](lv_timer_t* t){
        ActivityDataPage* self = (ActivityDataPage*)lv_timer_get_user_data(t);
        if(self && self->activityMapCanvas != nullptr){
            // Lock voor thread-safe access
            portENTER_CRITICAL(&canvasLock);
            
            activityMap.generateMap(15);
            if(activityMap.redrawMap){
                activityMap.displayMap();
                lv_draw_sw_rgb565_swap(activityMap.mapBuffer, 320 * 480);

                lv_canvas_set_buffer(self->activityMapCanvas, activityMap.mapBuffer, 320, 480, LV_COLOR_FORMAT_RGB565);
            }
            
            portEXIT_CRITICAL(&canvasLock);
        }
    }, 1000, pThis);
}

void ActivityDataPage::create5Widget(){
    widgets[0].widget.createWidget(128, 115, 20, 32, widgets[0].typeOfData);
    widgets[0].widget.createWidgetLabel(5, 2);

    widgets[1].widget.createWidget(128, 115, 174, 32, widgets[1].typeOfData);
    widgets[1].widget.createWidgetLabel(5, 2);

    widgets[2].widget.createWidget(282, 115, 20, 182, widgets[2].typeOfData);
    widgets[2].widget.createWidgetLabel(5, 2);

    widgets[3].widget.createWidget(128, 115, 20, 332, widgets[3].typeOfData);
    widgets[3].widget.createWidgetLabel(5, 2);

    widgets[4].widget.createWidget(128, 115, 174, 332, widgets[4].typeOfData);
    widgets[4].widget.createWidgetLabel(5, 2);
}

void ActivityDataPage::create6Widget(){
    widgets[0].widget.createWidget(128, 115, 20, 32, widgets[0].typeOfData);
    widgets[0].widget.createWidgetLabel(5, 2);

    widgets[1].widget.createWidget(128, 115, 174, 32, widgets[1].typeOfData);
    widgets[1].widget.createWidgetLabel(5, 2);

    widgets[2].widget.createWidget(128, 115, 20, 182, widgets[2].typeOfData);
    widgets[2].widget.createWidgetLabel(5, 2);

    widgets[3].widget.createWidget(128, 115, 174, 182, widgets[3].typeOfData);
    widgets[3].widget.createWidgetLabel(5, 2);

    widgets[4].widget.createWidget(128, 115, 20, 332, widgets[4].typeOfData);
    widgets[4].widget.createWidgetLabel(5, 2);

    widgets[5].widget.createWidget(128, 115, 174, 332, widgets[5].typeOfData);
    widgets[5].widget.createWidgetLabel(5, 2);
}

// void ActivityDataPage::updateDataPageInformation(lv_event_t *e){
//     for(auto& w : widgets) {
//         w.widget.updateWidgetData();
//     }
// }

// void ActivityDataPage::event_cb(lv_event_t *e){
//     ActivityDataPage* self = static_cast<ActivityDataPage*>(lv_event_get_user_data(e));
//     if(self) self->updateDataPageInformation(e);
// }
void ActivityDataPage::setDataPageIndex(uint8_t index){
    _dataPageIndex = index;
}

uint8_t ActivityDataPage::getDataPageIndex(){
    return _dataPageIndex;
}