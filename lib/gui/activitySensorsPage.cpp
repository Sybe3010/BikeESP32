#include "activitySensorsPage.hpp"

lv_obj_t *bleActivityScreen;


void makeBleActivityScreen(){
    bleActivityScreen = lv_table_create(NULL);
    lv_obj_set_size(bleActivityScreen, 320, 455);
    lv_obj_set_pos(bleActivityScreen, 0, 25);

    lv_table_set_column_count(bleActivityScreen, 2);  // ← 2 kolommen (naam + bestand)

    lv_table_set_cell_value(bleActivityScreen, 0, 0, "Name");
    lv_table_set_cell_value(bleActivityScreen, 0, 1, "MAC Address");
    lv_table_set_column_width(bleActivityScreen, 0, 160);
    lv_table_set_column_width(bleActivityScreen, 1, 160);

    lv_obj_add_event_cb(bleActivityScreen, bleActivityDeviceEvent, LV_EVENT_ALL, NULL);

    lv_obj_set_style_pad_ver(bleActivityScreen, 15, LV_PART_ITEMS);
    lv_obj_set_style_border_width(bleActivityScreen, 1, LV_PART_ITEMS);
    lv_obj_set_style_border_color(bleActivityScreen, lv_color_hex(0x303030), LV_PART_ITEMS);
    lv_obj_set_style_border_side(bleActivityScreen, LV_BORDER_SIDE_BOTTOM, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bleActivityScreen, lv_color_hex(0x303030), LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(bleActivityScreen, LV_OPA_100, LV_PART_ITEMS | LV_STATE_PRESSED);

}

void updateBleActivityScreen(){
    lv_table_set_row_count(bleActivityScreen, 1);
    // bleSensors.startScan();

    uint8_t devicesCount = 1;

    for(int i = 0; i < bleSensors.clients.size(); i++){
        BLEAdvertisedDevice* device = bleSensors.clients[i].targetDevice;

        lv_table_set_row_count(bleActivityScreen, devicesCount + 1);

        lv_table_set_cell_value(bleActivityScreen, devicesCount, 0, device->getName().c_str());
        lv_table_set_cell_value(bleActivityScreen, devicesCount, 1, device->getAddress().toString().c_str());
        devicesCount++;
    }
}

void bleActivityDeviceEvent(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = (lv_obj_t*)lv_event_get_target(e);
    uint32_t row; // 32 bit omdat lv_table_get_selected_cell dit type nodig heeft
    uint32_t col;
    if(code == LV_EVENT_LONG_PRESSED) {
        lv_table_get_selected_cell(obj, &row, &col);
        Serial.print("Selected device: ");
        Serial.println(lv_table_get_cell_value(obj, row, 0));
        Serial.print("MAC Address: ");
        Serial.println(lv_table_get_cell_value(obj, row, 1));
        bleSensors.targetDevice = new BLEAdvertisedDevice(bleSensors.getScanResults().getDevice(row - 1));
        bleSensors.connectToDevice(bleSensors.targetDevice);

        lv_screen_load(homeScreen);
    }

}


