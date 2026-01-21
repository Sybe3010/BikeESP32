#pragma once

#include "globalGuiDef.h"
#include "smallWidget.hpp"
#include "activityGlobal.h"
#include "maps.hpp"

#include <vector>

class ActivityDataPage{ // 1 pagina object
    private:
        bool isDataPageLoaded;
        bool isMapVisible;
    
        bool _customData = false; // for widgets

        uint8_t _dataPageIndex;

        DataPageLayout _layout = NO_MAP_AND_6_WIDGETS;

        struct WidgetsOnDataPage{
            Widget widget;
            typeData typeOfData;
            uint8_t widgetId;
        };

        

        static void event_cb(lv_event_t *e);

        void create2WidgetAndMap();
        void create5Widget();
        void create6Widget();
    public:
        lv_obj_t *dataPage;
        std::vector<WidgetsOnDataPage> widgets;

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