#pragma once

#include "globalGuiDef.h"
#include "activityGlobal.h"

#include "Arduino.h"

class Widget{
    public:
        Widget(){}
        Widget(lv_obj_t* parent);

        void createWidget(int sizeX, int sizeY, int posX, int posY, typeData type);
        void createWidgetLabel(int posX, int posY);
        void changeWidgetData(const char* data);

        void setBgOpacity(lv_opa_t opa);
    private:
        int _sizeX;
        int _sizeY;
        int _posX;
        int _posY;

        int labelX;
        int labelY;
        std::string _widgetName;

        lv_obj_t* _parent;
        lv_obj_t* widget;
        lv_obj_t* widgetLabel;
};