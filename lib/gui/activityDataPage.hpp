#pragma once

#include "globalGuiDef.h"
#include "widget.hpp"

#include <vector>

class ActivityDataPage{ // 1 pagina object
    private:
        enum DataPageLayout{
            MAP_AND_2_WIDGETS,
            NO_MAP_AND_5_WIDGETS,
            NO_MAP_AND_6_WIDGETS
        };

        enum typeData{
            ACTIVITY_TIMER,
            ACTIVITY_HEART_RATE,
            ACTIVITY_CADANCE,
            ACTIVITY_SPEED,
            ACTIVITY_POWER,
            ACTIVITY_REMAINING_KM_ON_ROUTE,
            ACTIVITY_ELAPSED_DISTANCE,
            ACTIVITY_AVG_SPEED,
            ACTIVITY_AVG_CADANCE
        };

        bool isDataPageLoaded;
        bool isMapVisible;
    
        bool _customData = false; // for widgets

        uint8_t _dataPageIndex;

        DataPageLayout _layout = NO_MAP_AND_6_WIDGETS;

        lv_obj_t *dataPage;

        struct WidgetsOnDataPage{
            Widget widget;
            typeData typeOfData;
            uint8_t widgetId;
        };

        std::vector<WidgetsOnDataPage> widgets;

        void create2WidgetAndMap();
        void create5Widget();
        void create6Widget();
    public:
        ActivityDataPage();
        ActivityDataPage(DataPageLayout layout);
        ActivityDataPage(DataPageLayout layout, bool customData);

        void setWidgetTypes(typeData widget1, typeData widget2);
        void setWidgetTypes(typeData widget1, typeData widget2, typeData widget3, typeData widget4, typeData widget5);
        void setWidgetTypes(typeData widget1, typeData widget2, typeData widget3, typeData widget4, typeData widget5, typeData widget6);

        void createDataPage();
        void updateDataPageInformation(lv_event_t *e);

        void setDataPageIndex(uint8_t index);
        uint8_t getDataPageIndex();
};