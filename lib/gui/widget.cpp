#include "widget.hpp"

// Statische variabelen initialiseren
lv_timer_t* Widget::globalWidgetTimer = nullptr;
std::vector<Widget*> Widget::activeWidgets;

Widget::Widget(lv_obj_t* parent){
    _parent = parent;
}

void Widget::createWidget(int sizeX, int sizeY, int posX, int posY, typeData type){
    _sizeX = sizeX;
    _sizeY = sizeY;
    _posX = posX;
    _posY = posY;
    _type = type;

    widget = lv_obj_create(_parent);
    lv_obj_set_size(widget, _sizeX, _sizeY);
    lv_obj_set_pos(widget, _posX, _posY);

    lv_obj_set_style_bg_color(widget, lv_color_hex(0x737373), LV_STATE_DEFAULT);

    widgetDataLabel = lv_label_create(widget);
    lv_obj_set_pos(widgetDataLabel, 23, 46);

    // Voeg deze widget toe aan de globale lijst
    addWidget(this);
}

void Widget::createWidgetLabel(int posX, int posY){
    labelX = posX;
    labelY = posY;

    widgetLabel = lv_label_create(widget);
    lv_obj_set_pos(widgetLabel, labelX, labelY);
    switch (_type)
    {
        case ACTIVITY_TIMER:
            typeName = "Timer";
        break;
        case ACTIVITY_HEART_RATE:
            typeName = "Heart Rate";
        break;
        case ACTIVITY_CADANCE:
            typeName = "Cadance";
        break;
        case ACTIVITY_SPEED:
            typeName = "Speed";
        break;
        case ACTIVITY_POWER:
            typeName = "Power";
        break;
        case ACTIVITY_REMAINING_KM_ON_ROUTE:
            typeName = "Remaining Km";
        break;
        case ACTIVITY_ELAPSED_DISTANCE:
            typeName = "Distance";
        break;
        case ACTIVITY_AVG_SPEED:
            typeName = "Avg Speed";
        break;
        case ACTIVITY_AVG_CADANCE:
            typeName = "Avg Cadance";
        break;
        default:
            _type = ACTIVITY_TIMER;
            typeName = "Timer";
        break;
    }
    lv_label_set_text(widgetLabel, typeName.c_str());
}

void Widget::setBgOpacity(lv_opa_t opa){
    lv_obj_set_style_opa(widget, opa, LV_STYLE_BG_GRAD_OPA);
}

void Widget::updateWidgetData(){
    switch (_type)
    {
        case ACTIVITY_TIMER:
            lv_label_set_text_fmt(widgetDataLabel,"%d", 0); 
        break;
        case ACTIVITY_HEART_RATE:
            lv_label_set_text_fmt(widgetDataLabel, "%d", 0);
        break;
        case ACTIVITY_CADANCE:
            lv_label_set_text_fmt(widgetDataLabel, "%d", 0);
        break;
        case ACTIVITY_SPEED:
            lv_label_set_text_fmt(widgetDataLabel, "%.1f Km/h", 0);
        break;
        case ACTIVITY_POWER:
            lv_label_set_text_fmt(widgetDataLabel, "%d W", 0);
        break;
        case ACTIVITY_REMAINING_KM_ON_ROUTE:
            lv_label_set_text_fmt(widgetDataLabel, "%.1f Km ", 0);
        break;
        case ACTIVITY_ELAPSED_DISTANCE:
            lv_label_set_text_fmt(widgetDataLabel, "%.1f Km", 0);
        break;
        case ACTIVITY_AVG_SPEED:
            lv_label_set_text_fmt(widgetDataLabel, "%.1f Km/h", 0);
        break;
        case ACTIVITY_AVG_CADANCE:
            lv_label_set_text_fmt(widgetDataLabel, "%d", 0);
        break;
    }
}

// Statische Widget Manager functies
void Widget::initializeWidgetTimer(){
    if(globalWidgetTimer == nullptr){
        globalWidgetTimer = lv_timer_create([](lv_timer_t* t){
            Widget::updateAllWidgets(t);
        }, 1000, nullptr);
    }
}

void Widget::addWidget(Widget* w){
    // Voeg widget toe aan de vector
    activeWidgets.push_back(w);
    
    // Maak de globale timer aan bij de eerste widget
    if(globalWidgetTimer == nullptr){
        initializeWidgetTimer();
    }
}

void Widget::removeWidget(Widget* w){
    // Verwijder widget uit de vector
    auto it = std::find(activeWidgets.begin(), activeWidgets.end(), w);
    if(it != activeWidgets.end()){
        activeWidgets.erase(it);
    }
    
    // Verwijder timer als er geen widgets meer zijn
    if(activeWidgets.empty() && globalWidgetTimer != nullptr){
        lv_timer_del(globalWidgetTimer);
        globalWidgetTimer = nullptr;
    }
}

void Widget::updateAllWidgets(lv_timer_t* t){
    // Update alle actieve widgets in één timer callback
    for(Widget* w : activeWidgets){
        if(w != nullptr){
            w->updateWidgetData();
        }
    }
}