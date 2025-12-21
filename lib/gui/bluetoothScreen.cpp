#include "bluetoothScreen.hpp"

lv_obj_t *bleScreen;

lv_obj_t *messageBox;

Bluetooth bleSensors;


void makeBleScreen(){
    bleScreen = lv_table_create(NULL);
    lv_obj_set_size(bleScreen, 320, 455);
    lv_obj_set_pos(bleScreen, 0, 25);

    lv_table_set_column_count(bleScreen, 2);  // ← 2 kolommen (naam + bestand)

    lv_table_set_cell_value(bleScreen, 0, 0, "Name");
    lv_table_set_cell_value(bleScreen, 0, 1, "MAC Address");
    lv_table_set_column_width(bleScreen, 0, 160);
    lv_table_set_column_width(bleScreen, 1, 160);

    lv_obj_add_event_cb(bleScreen, bleDeviceEvent, LV_EVENT_ALL, NULL);

    lv_obj_set_style_pad_ver(bleScreen, 15, LV_PART_ITEMS);
    lv_obj_set_style_border_width(bleScreen, 1, LV_PART_ITEMS);
    lv_obj_set_style_border_color(bleScreen, lv_color_hex(0x303030), LV_PART_ITEMS);
    lv_obj_set_style_border_side(bleScreen, LV_BORDER_SIDE_BOTTOM, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bleScreen, lv_color_hex(0x303030), LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(bleScreen, LV_OPA_100, LV_PART_ITEMS | LV_STATE_PRESSED);

}

void updateBleScreen(){
    // uint8_t sensorCount = 1;

    // bleSensors.scanAvailableDevices();

    // for(int i = 0; i < bleSensors.devices.size(); i++){
    //     lv_table_set_cell_value_fmt(bleScreen, sensorCount, 0, LV_SYMBOL_BLUETOOTH " - %s", bleSensors.devices[i].name.c_str());
    //     lv_table_set_cell_value_fmt(bleScreen, sensorCount, 1, "%s", bleSensors.devices[i].deviceMacAddress.c_str());
    //     sensorCount++;
    // }
}

void bleDeviceEvent(lv_event_t *e){
    
}


