#include "activityPage.hpp"

Maps activityMap = Maps();
                
GpxParser activityTrack("/sdcard/TRK/2462188412.gpx");

lv_obj_t *activityPage;

lv_obj_t *activityPageTileView;
lv_obj_t *activityPageMapTile;
lv_obj_t *activityPageDataTile; // geeft de huidige data weer
lv_obj_t *activityPageStatsTile; // geeft de statistieken weer: totale afstand, gemiddelde snelheid, max snelheid, totale klimming
lv_obj_t *activityPageQuickSettingsTile; // geeft snelkoppelingen naar instellingen
lv_obj_t *activityPageClimbView; // geeft een grafiek weer van de beklimming

lv_obj_t *mapWidget;
lv_obj_t *mapDataWidget1;
lv_obj_t *mapDataWidget2;

lv_obj_t *dataWidget1;
lv_obj_t *dataWidget2;
lv_obj_t *dataWidget3;
lv_obj_t *dataWidget4;

lv_obj_t *statsWidget1;
lv_obj_t *statsWidget2;
lv_obj_t *statsWidget3;
lv_obj_t *statsWidget4;

lv_obj_t *quickSettingWidget1;
lv_obj_t *quickSettingWidget2;
lv_obj_t *quickSettingWidget3;
lv_obj_t *quickSettingWidget4;

lv_obj_t *climbViewWidget1;
lv_obj_t *climbViewWidget2;
lv_obj_t *climbGraph;

lv_obj_t *browseNextBtn;
lv_obj_t *browsePrevBtn;
lv_obj_t *actMenuBtn;

lv_chart_series_t * elevationSerieAct;

bool activityStarted = false;
bool trackLoaded = true;
bool climbViewActive = true;
lv_timer_t* activityUITimer;

void createActivityPage(){
    activityPage = lv_obj_create(NULL);
    lv_obj_set_pos(activityPage, 0, 0);
    lv_obj_set_size(activityPage, 320, 460);
    lv_obj_add_event_cb(activityPage,   activityPageController, LV_EVENT_ALL, NULL);


    activityPageTileView = lv_tileview_create(activityPage);
    activityPageMapTile = lv_tileview_add_tile(activityPageTileView, 0, 1, (lv_dir_t)(LV_DIR_RIGHT | LV_DIR_BOTTOM | LV_DIR_TOP));
    activityPageDataTile = lv_tileview_add_tile(activityPageTileView, 1, 1, (lv_dir_t)(LV_DIR_LEFT | LV_DIR_RIGHT));
    activityPageStatsTile = lv_tileview_add_tile(activityPageTileView, 2, 1, LV_DIR_LEFT);
    activityPageQuickSettingsTile = lv_tileview_add_tile(activityPageTileView, 0, 0, LV_DIR_BOTTOM);
    activityPageClimbView = lv_tileview_add_tile(activityPageTileView, 0, 2, LV_DIR_TOP);

    lv_tileview_set_tile(activityPageTileView, activityPageMapTile, LV_ANIM_OFF);

    createMapTile();
    createDataTile();
    createStatsTile();
    createQuickSettingsTile();
    createClimbViewTile();

    createBrowseButtons();
}

void createMapTile(){
    activityMap.initMap(320, 480);
    activityMap.createMapScrSprites();
    mapWidget = lv_canvas_create(activityPageMapTile);
    lv_obj_set_size(mapWidget, 320, 480);
    lv_obj_add_event_cb(mapWidget,   activityPageController, LV_EVENT_ALL, NULL);

    
    mapDataWidget1 = lv_obj_create(activityPageMapTile);
    lv_obj_set_size(mapDataWidget1, 128, 115);
    lv_obj_set_pos(mapDataWidget1, 20, 32);
    lv_obj_add_event_cb(mapDataWidget1,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *mapWidget1Label = lv_label_create(mapDataWidget1);
    lv_label_set_text(mapWidget1Label, "Map Widget 1");
    lv_obj_t *mapWidget1Value = lv_label_create(mapDataWidget1);
    lv_label_set_text(mapWidget1Value, "Value 1");
    lv_obj_set_pos(mapWidget1Value, 10, 30);
    

    mapDataWidget2 = lv_obj_create(activityPageMapTile);
    lv_obj_set_size(mapDataWidget2, 128, 115);
    lv_obj_set_pos(mapDataWidget2, 174, 32);
    lv_obj_add_event_cb(mapDataWidget2,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *mapWidget2Label = lv_label_create(mapDataWidget2);
    lv_label_set_text(mapWidget2Label, "Map Widget 1");
    lv_obj_t *mapWidget2Value = lv_label_create(mapDataWidget2);
    lv_label_set_text(mapWidget2Value, "Value 1");
    lv_obj_set_pos(mapWidget2Value, 10, 30);

}
void createDataTile(){
    dataWidget1 = lv_obj_create(activityPageDataTile);
    lv_obj_set_size(dataWidget1, 128, 115);
    lv_obj_set_pos(dataWidget1, 20, 32);
    lv_obj_add_event_cb(dataWidget1,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *mapDataWidget2Label = lv_label_create(dataWidget1);
    lv_label_set_text(mapDataWidget2Label, "Map Widget 2");
    lv_obj_t *mapDataWidget2Value = lv_label_create(dataWidget1);
    lv_label_set_text(mapDataWidget2Value, "Value 2");
    lv_obj_set_pos(mapDataWidget2Value, 10, 30);



    dataWidget2 = lv_obj_create(activityPageDataTile);
    lv_obj_set_size(dataWidget2, 128, 115);
    lv_obj_set_pos(dataWidget2, 174, 32);
    lv_obj_add_event_cb(dataWidget2,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *dataWidget2Label = lv_label_create(dataWidget2);
    lv_label_set_text(dataWidget2Label, "Data Widget 2");
    lv_obj_t *dataWidget2Value = lv_label_create(dataWidget2);
    lv_label_set_text(dataWidget2Value, "Value 2");
    lv_obj_set_pos(dataWidget2Value, 10, 30);



    dataWidget3 = lv_obj_create(activityPageDataTile);
    lv_obj_set_size(dataWidget3, 128, 115);
    lv_obj_set_pos(dataWidget3, 20, 182);
    lv_obj_add_event_cb(dataWidget3,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *dataWidget3Label = lv_label_create(dataWidget3);
    lv_label_set_text(dataWidget3Label, "Data Widget 3");
    lv_obj_t *dataWidget3Value = lv_label_create(dataWidget3);
    lv_label_set_text(dataWidget3Value, "Value 3");
    lv_obj_set_pos(dataWidget3Value, 10, 30);



    dataWidget4 = lv_obj_create(activityPageDataTile);
    lv_obj_set_size(dataWidget4, 128, 115);
    lv_obj_set_pos(dataWidget4, 174, 182);
    lv_obj_add_event_cb(dataWidget4,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *dataWidget4Label = lv_label_create(dataWidget4);
    lv_label_set_text(dataWidget4Label, "Data Widget 4");
    lv_obj_t *dataWidget4Value = lv_label_create(dataWidget4);
    lv_label_set_text(dataWidget4Value, "Value 4");
    lv_obj_set_pos(dataWidget4Value, 10, 30);
}
void createStatsTile(){
    statsWidget1 = lv_obj_create(activityPageStatsTile);
    lv_obj_set_size(statsWidget1, 128, 115);
    lv_obj_set_pos(statsWidget1, 20, 32);
    lv_obj_add_event_cb(statsWidget1,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *statWidget1Label = lv_label_create(statsWidget1);
    lv_label_set_text(statWidget1Label, "Stat Label 1");
    lv_obj_t *statWidget1Value = lv_label_create(statsWidget1);
    lv_label_set_text(statWidget1Value, "Value 1");
    lv_obj_set_pos(statWidget1Value, 10, 30);


    statsWidget2 = lv_obj_create(activityPageStatsTile);
    lv_obj_set_size(statsWidget2, 128, 115);
    lv_obj_set_pos(statsWidget2, 174, 32);
    lv_obj_add_event_cb(statsWidget2,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *statWidget2Label = lv_label_create(statsWidget2);
    lv_label_set_text(statWidget2Label, "Stat Label 2");
    lv_obj_t *statWidget2Value = lv_label_create(statsWidget2);
    lv_label_set_text(statWidget2Value, "Value 2");
    lv_obj_set_pos(statWidget2Value, 10, 30);


    statsWidget3 = lv_obj_create(activityPageStatsTile);
    lv_obj_set_size(statsWidget3, 128, 115);
    lv_obj_set_pos(statsWidget3, 20, 182);
    lv_obj_add_event_cb(statsWidget3,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *statWidget3Label = lv_label_create(statsWidget3);
    lv_label_set_text(statWidget3Label, "Stat Label 3");
    lv_obj_t *statWidget3Value = lv_label_create(statsWidget3);
    lv_label_set_text(statWidget3Value, "Value 3");
    lv_obj_set_pos(statWidget3Value, 10, 30);


    statsWidget4 = lv_obj_create(activityPageStatsTile);
    lv_obj_set_size(statsWidget4, 128, 115);
    lv_obj_set_pos(statsWidget4, 174, 182);
    lv_obj_add_event_cb(statsWidget4,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *statWidget4Label = lv_label_create(statsWidget4);
    lv_label_set_text(statWidget4Label, "Stat Label 4");
    lv_obj_t *statWidget4Value = lv_label_create(statsWidget4);
    lv_label_set_text(statWidget4Value, "Value 4");
    lv_obj_set_pos(statWidget4Value, 10, 30);
}
void createQuickSettingsTile(){
    quickSettingWidget1 = lv_button_create(activityPageQuickSettingsTile);
    lv_obj_set_size(quickSettingWidget1, 128, 115);
    lv_obj_set_pos(quickSettingWidget1, 20, 32);
    lv_obj_t *quickLabel1 = lv_label_create(quickSettingWidget1);
    lv_label_set_text(quickLabel1, "Stop Activity");
    lv_obj_add_event_cb(quickSettingWidget1,   activityPageController, LV_EVENT_ALL, NULL);

    quickSettingWidget2 = lv_button_create(activityPageQuickSettingsTile);
    lv_obj_set_size(quickSettingWidget2, 128, 115);
    lv_obj_set_pos(quickSettingWidget2, 174, 32);
    lv_obj_t *quickLabel2 = lv_label_create(quickSettingWidget2);
    lv_label_set_text(quickLabel2, "Pas route aan");
    lv_obj_add_event_cb(quickSettingWidget2,   activityPageController, LV_EVENT_ALL, NULL);

    quickSettingWidget3 = lv_button_create(activityPageQuickSettingsTile);
    lv_obj_set_size(quickSettingWidget3, 128, 115);
    lv_obj_set_pos(quickSettingWidget3, 20, 182);
    lv_obj_t *quicklabel3 = lv_label_create(quickSettingWidget3);
    lv_label_set_text(quicklabel3, "Sensors");
    lv_obj_add_event_cb(quickSettingWidget3,   activityPageController, LV_EVENT_ALL, NULL);

    quickSettingWidget4 = lv_button_create(activityPageQuickSettingsTile);
    lv_obj_set_size(quickSettingWidget4, 128, 115);
    lv_obj_set_pos(quickSettingWidget4, 174, 182);
    lv_obj_t *quicklabel4 = lv_label_create(quickSettingWidget4);
    lv_label_set_text(quicklabel4, "Brightness");
    lv_obj_add_event_cb(quickSettingWidget4,   activityPageController, LV_EVENT_ALL, NULL);
}
void createClimbViewTile(){
    climbGraph = lv_chart_create(activityPageClimbView);
    lv_obj_set_size(climbGraph, 300, 300);
    lv_obj_set_pos(climbGraph, 10, 10);
    lv_obj_add_event_cb(climbGraph,  activityPageController, LV_EVENT_ALL, NULL);
    elevationSerieAct = lv_chart_add_series(climbGraph, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);

    climbViewWidget1 = lv_obj_create(activityPageClimbView);
    lv_obj_set_size(climbViewWidget1, 128, 115);
    lv_obj_set_pos(climbViewWidget1, 10, 320);
    lv_obj_add_event_cb(climbViewWidget1,   activityPageController, LV_EVENT_ALL, NULL);

    climbViewWidget2 = lv_obj_create(activityPageClimbView);
    lv_obj_set_size(climbViewWidget2, 128, 115);
    lv_obj_set_pos(climbViewWidget2, 174, 320);
    lv_obj_add_event_cb(climbViewWidget2,   activityPageController, LV_EVENT_ALL, NULL);
}
void createBrowseButtons(){
    browseNextBtn = lv_button_create(activityPage);
    lv_obj_set_size(browseNextBtn, 90, 40);
    lv_obj_set_pos(browseNextBtn, 210, 420);
    lv_obj_t *nextLabel = lv_label_create(browseNextBtn);
    lv_label_set_text(nextLabel, "Next");
    lv_obj_add_event_cb(browseNextBtn,   activityPageController, LV_EVENT_ALL, NULL);

    actMenuBtn = lv_button_create(activityPage);
    lv_obj_set_size(actMenuBtn, 90, 40);
    lv_obj_set_pos(actMenuBtn, 110, 420);
    lv_obj_t *menuLabel = lv_label_create(actMenuBtn);
    lv_label_set_text(menuLabel, "Menu");
    lv_obj_add_event_cb(actMenuBtn, activityPageController, LV_EVENT_ALL, NULL);

    browsePrevBtn = lv_button_create(activityPage);
    lv_obj_set_size(browsePrevBtn, 90, 40);
    lv_obj_set_pos(browsePrevBtn, 10, 420);
    lv_obj_t *prevLabel = lv_label_create(browsePrevBtn);
    lv_label_set_text(prevLabel, "Prev");
    lv_obj_add_event_cb(browsePrevBtn,   activityPageController, LV_EVENT_ALL, NULL);
}


/// ActivityPage Logic ///

void activityPageController(lv_event_t *e){
    lv_obj_t* target = (lv_obj_t*)lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* activeTileController = NULL;
    if(activityStarted){
        activeTileController = lv_tileview_get_tile_active(activityPageTileView);
    }
    

    if(code == LV_EVENT_CLICKED){
        if(activeTileController == NULL) return;
        
        if(target == browseNextBtn){
            if(activeTileController == activityPageMapTile){
                lv_tileview_set_tile(activityPageTileView, activityPageDataTile, LV_ANIM_ON);
                return;
            }
            if(activeTileController == activityPageDataTile){
                lv_tileview_set_tile(activityPageTileView, activityPageStatsTile, LV_ANIM_ON);
                return;
            }
        } else if(target == browsePrevBtn){
            if(activeTileController == activityPageStatsTile){
                lv_tileview_set_tile(activityPageTileView, activityPageDataTile, LV_ANIM_ON);
                return;
            }
            if(activeTileController == activityPageDataTile){
                lv_tileview_set_tile(activityPageTileView, activityPageMapTile, LV_ANIM_ON);
                return;
            }
        } else if(target == actMenuBtn){
            lv_tileview_set_tile(activityPageTileView, activityPageQuickSettingsTile, LV_ANIM_OFF);
            return;
        }
    }
    if(code == LV_EVENT_SCREEN_LOADED){
        if(target == activityPage && activityStarted == false){
            activityStarted = true;
            activityUITimer = lv_timer_create(activityUIUpdateTimer, 1000, NULL);
            if(trackLoaded == true){
                log_e("Track wordt geladen.");
                activityTrack.loadTrack();
            }
        }
    }
    if(code == LV_EVENT_VALUE_CHANGED){
        if(target == activityPage && activityStarted == true){
            activityStarted = false;
            lv_timer_delete(activityUITimer);
            newActivity->stopActivity();
            lv_screen_load(homeScreen);
        }
    }
    if(code == LV_EVENT_REFRESH){
        if(activeTileController == NULL) return;
        
        if(activeTileController == activityPageMapTile){
            if(target == mapDataWidget1){
                lv_obj_t* label = lv_obj_get_child(target, 1);
                lv_label_set_text_fmt(label, "%hhu", bleSensors.hrValue);
            }
            if(target == mapDataWidget2){
                lv_obj_t* label = lv_obj_get_child(target, 1);
                lv_label_set_text_fmt(label, "%.1f", (double)gps.gpsData.speed);
            }
        }

        if(activeTileController == activityPageDataTile){
            if(target == dataWidget1){
                // Update widget 1 data
            }
            if(target == dataWidget2){
                // Update widget 2 data
            }
            if(target == dataWidget3){
                // Update widget 3 data
            }
            if(target == dataWidget4){
                // Update widget 4 data
            }
        }

        if(activeTileController == activityPageStatsTile){
            if(target == statsWidget1){
                
            }
            if(target == statsWidget2){
                // Update widget 2 data
            }
            if(target == statsWidget3){
                // Update widget 3 data
            }
            if(target == statsWidget4){
                // Update widget 4 data
            }
        }

        if(activeTileController == activityPageClimbView && climbViewActive){
            if(target == climbGraph){
                size_t startIndexClimb;
                size_t endIndexClimb;
                if(activityTrack.climbs.size() > 0){
                    log_e("climbs beschikbaar: %d", activityTrack.climbs.size());
                    startIndexClimb = activityTrack.climbs[0].startIndex;
                    endIndexClimb = activityTrack.climbs[0].endIndex;

                    // zet de climbs op de grafiek
                    lv_chart_set_point_count(climbGraph, endIndexClimb - startIndexClimb + 1);
                    for(uint32_t i = startIndexClimb; i < endIndexClimb; i++) {
                        lv_chart_set_next_value(climbGraph, elevationSerieAct, activityTrack.elevationProfile[i].elevation);
                    }
                } else {
                    log_e("geen climbs beschikbaar");
                }
            }
            if(target == climbViewWidget1){
                
            }
            if(target == climbViewWidget2){
                // Update widget 2 data
            }
        }
    }
}

void activityUIUpdateTimer(lv_timer_t* timer){
    lv_obj_t* activeTile = lv_tileview_get_tile_active(activityPageTileView);

    if(activeTile == activityPageMapTile){
        // Map updaten
        activityMap.generateMap(15);
        activityMap.displayGpxRoute(activityTrack.trackData);
        if(activityMap.redrawMap){
            activityMap.displayMap();
            // Byte swap toepassen op de RGB565 buffer voor juiste kleurweergave
            lv_draw_sw_rgb565_swap(activityMap.mapBuffer, 320 * 480);
            // Buffer instellen op canvas
            lv_canvas_set_buffer(mapWidget, activityMap.mapBuffer, 320, 480, LV_COLOR_FORMAT_RGB565);
        }

        lv_obj_send_event(mapDataWidget1, LV_EVENT_REFRESH, NULL);
        lv_obj_send_event(mapDataWidget2, LV_EVENT_REFRESH, NULL);
        return;
    }

    if(activeTile == activityPageDataTile){
        lv_obj_send_event(dataWidget1, LV_EVENT_REFRESH, NULL); 
        lv_obj_send_event(dataWidget2, LV_EVENT_REFRESH, NULL);
        lv_obj_send_event(dataWidget3, LV_EVENT_REFRESH, NULL);
        lv_obj_send_event(dataWidget4, LV_EVENT_REFRESH, NULL);
        return;
    }

    if(activeTile == activityPageStatsTile){
        lv_obj_send_event(statsWidget1, LV_EVENT_REFRESH, NULL);
        lv_obj_send_event(statsWidget2, LV_EVENT_REFRESH, NULL);
        lv_obj_send_event(statsWidget3, LV_EVENT_REFRESH, NULL);
        lv_obj_send_event(statsWidget4, LV_EVENT_REFRESH, NULL);
        return;
    }

    if(activeTile == activityPageQuickSettingsTile){
        return;
    }

    if(activeTile == activityPageClimbView && climbViewActive){
        lv_obj_send_event(climbGraph, LV_EVENT_REFRESH, NULL);
        
        lv_obj_send_event(climbViewWidget1, LV_EVENT_REFRESH, NULL);
        lv_obj_send_event(climbViewWidget2, LV_EVENT_REFRESH, NULL);
        return;
    }
    return;
}