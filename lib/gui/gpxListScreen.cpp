#include "gpxListScreen.hpp"

lv_obj_t *gpxListScreen;

lv_obj_t *backScreen;

// Globale variabelen moeten elders correct gedeclareerd en geïnitialiseerd zijn:
// extern String trkFolder;
// extern String gpxFileFolder;
// extern lv_obj_t *gpxDetailsScreen;
// extern void fillGpxDetailsScreen();

void gpxListEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = (lv_obj_t *)lv_event_get_current_target(e);

    if(code == LV_EVENT_LONG_PRESSED) {
        uint32_t row, col;
        lv_table_get_selected_cell(obj, &row, &col);

        log_e("Row is %d", row);
        log_e("Col is: %d", col);

        // Header overslaan
        if(row == 0) return;

        const char* cellVal = lv_table_get_cell_value(obj, row, col);
        if(cellVal == NULL) return;  // lege cel → niets doen

        String sel(cellVal);

        // substring veilig maken
        String gpxName;
        if(sel.length() > 6) {
            gpxName = sel.substring(6);
        } else {
            gpxName = sel;  // fallback: hele string
        }

        const char* fileVal = lv_table_get_cell_value(obj, row, 1);
        if(fileVal == NULL) return;
        log_e("File name is: %s", fileVal);

        String gpxFile(fileVal);

        gpxFileFolder = String(trkFolder) + "/" + gpxFile;

        if(backScreen == activityHomeScreen){
            trackFileName = std::string(trkFolder) + "/" + (std::string)gpxFile.c_str();
        }
        // Zorg dat gpxDetailsScreen geldig is
        if(gpxDetailsScreen != NULL) {
            fillGpxDetailsScreen();
            lv_screen_load(gpxDetailsScreen);
        }
    } 
    if(code == LV_EVENT_GESTURE){
        if(backScreen == nullptr){
            lv_dir_t direction =  lv_indev_get_gesture_dir(lv_indev_active());
            if(direction = LV_DIR_LEFT){
                lv_screen_load(homeScreen);
            }
        }
        if(backScreen == activityHomeScreen){
            lv_dir_t direction =  lv_indev_get_gesture_dir(lv_indev_active());
            if(direction = LV_DIR_LEFT){
                lv_screen_load(backScreen);
            }
        }
        
    }
    if (code == LV_EVENT_REFRESH)
    {
        log_e("list screen refresh aangeroepen");
        backScreen = (lv_obj_t*)lv_event_get_param(e);
        log_e("backscreen pointer: %p", backScreen);
    }
    
}

void makeGpxListScreen() {
    gpxListScreen = lv_table_create(NULL);

    lv_obj_set_size(gpxListScreen, 320, 455);
    lv_obj_set_pos(gpxListScreen, 0, 25);
    lv_table_set_column_count(gpxListScreen, 2);

    // Header
    lv_table_set_cell_value(gpxListScreen, 0, 0, "Track Name");
    lv_table_set_cell_value(gpxListScreen, 0, 1, "File");
    lv_table_set_column_width(gpxListScreen, 0, 160);
    lv_table_set_column_width(gpxListScreen, 1, 160);

    // Alleen klikken‑events koppelen
    lv_obj_add_event_cb(gpxListScreen, gpxListEvent, LV_EVENT_ALL, NULL);

    // Styling
    lv_obj_set_style_pad_ver(gpxListScreen, 15, LV_PART_ITEMS);
    lv_obj_set_style_border_width(gpxListScreen, 1, LV_PART_ITEMS);
    lv_obj_set_style_border_color(gpxListScreen, lv_color_hex(0x303030), LV_PART_ITEMS);
    lv_obj_set_style_border_side(gpxListScreen, LV_BORDER_SIDE_BOTTOM, LV_PART_ITEMS);
    lv_obj_set_style_bg_color(gpxListScreen, lv_color_hex(0x303030), LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(gpxListScreen, LV_OPA_100, LV_PART_ITEMS | LV_STATE_PRESSED);
}

void updateGpxListScreen() {
    uint16_t totalGpx = 1;

    std::map<std::string, std::vector<std::string>> tracksByFile =
        GpxParser::getTagElementList(gpxTrackTag, gpxNameElem, trkFolder);

    for (const auto& it : tracksByFile) {
        const std::string& fileName = it.first;
        const std::vector<std::string>& trackNames = it.second;

        for (const std::string& trackName : trackNames) {
            lv_table_set_cell_value_fmt(gpxListScreen, totalGpx, 0,
                                        LV_SYMBOL_SHUFFLE " - %s", trackName.c_str());
            lv_table_set_cell_value_fmt(gpxListScreen, totalGpx, 1,
                                        "%s", fileName.c_str());
            totalGpx++;
        }
    }
}
