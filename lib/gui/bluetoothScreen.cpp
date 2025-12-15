#include "bluetoothScreen.hpp"

lv_obj_t *bleScreen;

Bluetooth bleSensors(false);

void makeBleScreen(){
    bleSensors.init();
    bleScreen = lv_table_create(NULL);
    lv_obj_set_size(bleScreen, 320, 455);
    lv_obj_set_pos(bleScreen, 0, 25);

    lv_table_set_column_count(bleScreen, 2);  // ← 2 kolommen (naam + bestand)

    lv_table_set_cell_value(bleScreen, 0, 0, "Name");
    lv_table_set_cell_value(bleScreen, 0, 1, "Service Id");
    lv_table_set_column_width(bleScreen, 0, 160);
    lv_table_set_column_width(bleScreen, 1, 160);

    lv_obj_add_event_cb(bleScreen, deviceClickEvent, LV_EVENT_ALL, NULL);

    lv_obj_set_style_pad_ver(bleScreen, 15, LV_PART_ITEMS);
    lv_obj_set_style_border_width(bleScreen, 1, LV_PART_ITEMS);
    lv_obj_set_style_border_color(bleScreen, lv_color_hex(0x303030), LV_PART_ITEMS);
    lv_obj_set_style_border_side(bleScreen, LV_BORDER_SIDE_BOTTOM, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bleScreen, lv_color_hex(0x303030), LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(bleScreen, LV_OPA_100, LV_PART_ITEMS | LV_STATE_PRESSED);
}

void updateBleScreen(){
    uint16_t totalBleDevices = 1;
    bleSensors.scanDevices();
    for(int i = 0; i < bleSensors.devices.size(); i++){
        lv_table_set_cell_value_fmt(gpxListScreen, totalBleDevices, 0, LV_SYMBOL_BLUETOOTH " - %s", bleSensors.devices[i].name);
        lv_table_set_cell_value_fmt(gpxListScreen, totalBleDevices, 1, "%s", bleSensors.devices[i].serviceId);
        totalBleDevices++;
    }
}

void deviceClickEvent(lv_event_t *e){

}


