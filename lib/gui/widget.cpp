#include "widget.hpp"

Widget::Widget(lv_obj_t* parent){
    _parent = parent;
}

void Widget::changeWidgetData(const char* data){
    lv_label_set_text(widgetLabel, data);
}
void Widget::createWidget(int sizeX, int sizeY, int posX, int posY, const char* widgetType){
    _sizeX = sizeX;
    _sizeY = sizeY;
    _posX = posX;
    _posY = posY;
    _widgetName = widgetType;

    widget = lv_obj_create(_parent);
    lv_obj_set_size(widget, _sizeX, _sizeY);
    lv_obj_set_pos(widget, _posX, _posY);

    lv_obj_set_style_bg_color(widget, lv_color_hex(0x737373), LV_STATE_DEFAULT);

    
}

void Widget::createWidgetLabel(int posX, int posY){
    labelX = posX;
    labelY = posY;

    widgetLabel = lv_label_create(widget);
    lv_obj_set_pos(widgetLabel, labelX, labelY);
}