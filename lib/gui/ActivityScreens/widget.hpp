#pragma once

#include "globalGuiDef.h"

#include "Arduino.h"

class Widget{
    public:
        Widget(){}
        Widget(lv_obj_t* parent);

        void createWidget(int sizeX, int sizeY, int posX, int posY, const char* widgetType);
        void createWidgetLabel(int posX, int posY);
        void changeWidgetData(const char* data);
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