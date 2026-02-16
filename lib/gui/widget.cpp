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

    // Data label in het midden van de widget
    widgetDataLabel = lv_label_create(widget);
    lv_obj_set_pos(widgetDataLabel, _sizeX / 2, _sizeY / 2 + 10);
    lv_obj_set_style_text_align(widgetDataLabel, LV_TEXT_ALIGN_CENTER, 0);
    
    // Zorg dat labels boven de widget zelf staan
    lv_obj_move_to_index(widgetDataLabel, -1);

    // Voeg deze widget toe aan de globale lijst
    addWidget(this);
}

void Widget::createWidgetLabel(int posX, int posY){
    labelX = posX;
    labelY = posY;

    widgetLabel = lv_label_create(widget);
    lv_obj_set_pos(widgetLabel, labelX, labelY);
    lv_obj_set_style_text_font(widgetLabel, &lv_font_montserrat_12, 0);
    
    // Zorg dat dit label boven andere elementen staat
    lv_obj_move_to_index(widgetLabel, -1);
    
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
    if(widgetDataLabel == nullptr || lv_obj_has_flag(_parent, LV_OBJ_FLAG_HIDDEN)){
        return; // Widget is verborgen of niet geinitialiseerd
    }
    switch (_type)
    {
        case ACTIVITY_TIMER:
            lv_label_set_text_fmt(widgetDataLabel,"%d", newActivity->getActivityData().timer); 
        break;
        case ACTIVITY_HEART_RATE:
            lv_label_set_text_fmt(widgetDataLabel, "%d", bleSensors.hrValue);
        break;
        case ACTIVITY_CADANCE:
            lv_label_set_text_fmt(widgetDataLabel, "%d", bleSensors.cadanceValue);
        break;
        case ACTIVITY_SPEED:
            if(bleSensors.speedValue < 0.1){
                lv_label_set_text_fmt(widgetDataLabel, "%.1f Km/h", gps.gpsData.speed);
            } else {
                lv_label_set_text_fmt(widgetDataLabel, "%.1f Km/h", bleSensors.speedValue);
            }
        break;
        case ACTIVITY_POWER:
            lv_label_set_text_fmt(widgetDataLabel, "%d W", 0); // nog geen powermeting in BLE sensors
        break;
        case ACTIVITY_REMAINING_KM_ON_ROUTE:
            lv_label_set_text_fmt(widgetDataLabel, "%.1f Km ", 0); // nog te implementeren
        break;
        case ACTIVITY_ELAPSED_DISTANCE:
            lv_label_set_text_fmt(widgetDataLabel, "%.1f Km", newActivity->getActivityData().distance);
        break;
        case ACTIVITY_AVG_SPEED:
            lv_label_set_text_fmt(widgetDataLabel, "%.1f Km/h", newActivity->getActivityData().avgSpeed);
        break;
        case ACTIVITY_AVG_CADANCE:
            lv_label_set_text_fmt(widgetDataLabel, "%d", 0); // nog te implementeren
        break;
    }
}

// Statische Widget Manager functies
void Widget::initializeWidgetTimer(){
    if(globalWidgetTimer == nullptr){
        // Stagger de widget timer naar 1100ms om te voorkomen dat deze tegelijk met map timer loopt
        globalWidgetTimer = lv_timer_create([](lv_timer_t* t){
            Widget::updateAllWidgets(t);
        }, 1100, nullptr);
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