#pragma once

#include "globalGuiDef.h"
#include "activityGlobal.h"

#include "Arduino.h"
#include <vector>

#include "bluetooth.hpp"
#include "activity.hpp"

extern Bluetooth bleSensors;
extern Activity* newActivity;

class Widget{
    public:
        Widget(){}
        Widget(lv_obj_t* parent);

        void createWidget(int sizeX, int sizeY, int posX, int posY, typeData type);
        void createWidgetLabel(int posX, int posY);

        void setBgOpacity(lv_opa_t opa);
        void updateWidgetData();
        lv_obj_t* widget;

        // Statische Widget Manager functies
        static void initializeWidgetTimer();
        static void addWidget(Widget* w);
        static void removeWidget(Widget* w);
        
    private:
        int _sizeX;
        int _sizeY;
        int _posX;
        int _posY;

        int labelX;
        int labelY;
        typeData _type;
        std::string typeName;

        lv_obj_t* _parent;
        lv_obj_t* widgetLabel;
        lv_obj_t* widgetDataLabel;

        // Statische gedeelde timer en widget lijst
        static lv_timer_t* globalWidgetTimer;
        static std::vector<Widget*> activeWidgets;
        static void updateAllWidgets(lv_timer_t* t);

};