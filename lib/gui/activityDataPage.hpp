#pragma once

#include "globalGuiDef.h"
#include "smallWidget.hpp"
#include "activityGlobal.h"
#include "maps.hpp"

#include <vector>
#include <freertos/portmacro.h>

extern portMUX_TYPE canvasLock;

class ActivityDataPage{ // 1 pagina object
    private:
        bool isDataPageLoaded;
        bool isMapVisible;
    
        bool _customData = false; // for widgets

        uint8_t _dataPageIndex;

        DataPageLayout _layout = NO_MAP_AND_6_WIDGETS;

        lv_obj_t* activityMapCanvas = nullptr;
        lv_timer_t* activityMapTimer = nullptr;

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
        
        lv_obj_t* activityMapCanvas_public() { return activityMapCanvas; }

        ActivityDataPage();
        ActivityDataPage(DataPageLayout layout);
        ActivityDataPage(DataPageLayout layout, bool customData);
        ~ActivityDataPage();

        void setWidgetTypes(typeData widget1, typeData widget2);
        void setWidgetTypes(typeData widget1, typeData widget2, typeData widget3, typeData widget4, typeData widget5);
        void setWidgetTypes(typeData widget1, typeData widget2, typeData widget3, typeData widget4, typeData widget5, typeData widget6);

        void createDataPage();
        void updateDataPageInformation(lv_event_t *e);

        void setDataPageIndex(uint8_t index);
        uint8_t getDataPageIndex();
};