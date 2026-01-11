#include "activityDataPage.hpp"

ActivityDataPage::ActivityDataPage(){}

ActivityDataPage::ActivityDataPage(DataPageLayout layout){
    _layout = layout;
}

ActivityDataPage::ActivityDataPage(DataPageLayout layout, bool customData){
    _customData = customData;
    _layout = layout;
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

    //lv_obj_add_event_cb(dataPage, ActivityDataPage::updateDataPageInformation, LV_EVENT_VALUE_CHANGED, NULL);
}

void ActivityDataPage::updateDataPageInformation(lv_event_t *e){

}

void ActivityDataPage::setDataPageIndex(uint8_t index){
    _dataPageIndex = index;
}

uint8_t ActivityDataPage::getDataPageIndex(){
    return _dataPageIndex;
}