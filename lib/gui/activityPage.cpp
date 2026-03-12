#include "activityPage.hpp"

Maps activityMap = Maps();
                
GpxParser *activityTrack = nullptr;

std::string trackFileName;

lv_obj_t *activityPage;

lv_obj_t *activityPageTileView;
lv_obj_t *activityPageMapTile;
lv_obj_t *activityPageDataTile; // geeft de huidige data weer
lv_obj_t *activityPageStatsTile; // geeft de statistieken weer: totale afstand, gemiddelde snelheid, max snelheid, totale klimming
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

lv_obj_t *climbViewWidget1;
lv_obj_t *climbViewWidget2;
lv_obj_t *climbGraph;

lv_obj_t *browseNextBtn;
lv_obj_t *browsePrevBtn;
lv_obj_t *actMenuBtn;

lv_obj_t *ActivityZoomInBtn;
lv_obj_t *ActivityZoomOutBtn;

lv_chart_series_t * elevationSerieAct;

int currentZoomLevel = 15;
bool activityStarted = false;
bool trackLoaded = false;
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
    activityPageClimbView = lv_tileview_add_tile(activityPageTileView, 0, 2, LV_DIR_TOP);

    lv_tileview_set_tile(activityPageTileView, activityPageMapTile, LV_ANIM_OFF);

    createMapTile();
    createDataTile();
    createStatsTile();
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
    lv_label_set_text(mapWidget1Label, "Heart Rate");
    lv_obj_t *mapWidget1Value = lv_label_create(mapDataWidget1);
    lv_label_set_text(mapWidget1Value, "Heart Rate");
    lv_obj_set_pos(mapWidget1Value, 10, 30);
    

    mapDataWidget2 = lv_obj_create(activityPageMapTile);
    lv_obj_set_size(mapDataWidget2, 128, 115);
    lv_obj_set_pos(mapDataWidget2, 174, 32);
    lv_obj_add_event_cb(mapDataWidget2,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *mapWidget2Label = lv_label_create(mapDataWidget2);
    lv_label_set_text(mapWidget2Label, "Speed");
    lv_obj_t *mapWidget2Value = lv_label_create(mapDataWidget2);
    lv_label_set_text(mapWidget2Value, "Value 1");
    lv_obj_set_pos(mapWidget2Value, 10, 30);

    ActivityZoomInBtn = lv_button_create(activityPageMapTile);
    lv_obj_set_size(ActivityZoomInBtn, 40, 40);
    lv_obj_set_pos(ActivityZoomInBtn, 10, 380);
    lv_obj_t *zoomInLabel = lv_label_create(ActivityZoomInBtn);
    lv_label_set_text(zoomInLabel, "+");
    lv_obj_add_event_cb(ActivityZoomInBtn,   activityPageController, LV_EVENT_ALL, NULL);

    ActivityZoomOutBtn = lv_button_create(activityPageMapTile);
    lv_obj_set_size(ActivityZoomOutBtn, 40, 40);
    lv_obj_set_pos(ActivityZoomOutBtn, 10, 340);
    lv_obj_t *zoomOutLabel = lv_label_create(ActivityZoomOutBtn);
    lv_label_set_text(zoomOutLabel, "-");
    lv_obj_add_event_cb(ActivityZoomOutBtn,   activityPageController, LV_EVENT_ALL, NULL);
}

void createDataTile(){
    dataWidget1 = lv_obj_create(activityPageDataTile);
    lv_obj_set_size(dataWidget1, 128, 115);
    lv_obj_set_pos(dataWidget1, 20, 32);
    lv_obj_add_event_cb(dataWidget1,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *mapDataWidget2Label = lv_label_create(dataWidget1);
    lv_label_set_text(mapDataWidget2Label, "Timer");
    lv_obj_t *mapDataWidget2Value = lv_label_create(dataWidget1);
    lv_label_set_text(mapDataWidget2Value, "Value 2");
    lv_obj_set_pos(mapDataWidget2Value, 10, 30);



    dataWidget2 = lv_obj_create(activityPageDataTile);
    lv_obj_set_size(dataWidget2, 128, 115);
    lv_obj_set_pos(dataWidget2, 174, 32);
    lv_obj_add_event_cb(dataWidget2,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *dataWidget2Label = lv_label_create(dataWidget2);
    lv_label_set_text(dataWidget2Label, "Distance");
    lv_obj_t *dataWidget2Value = lv_label_create(dataWidget2);
    lv_label_set_text(dataWidget2Value, "Value 2");
    lv_obj_set_pos(dataWidget2Value, 10, 30);



    dataWidget3 = lv_obj_create(activityPageDataTile);
    lv_obj_set_size(dataWidget3, 128, 115);
    lv_obj_set_pos(dataWidget3, 20, 182);
    lv_obj_add_event_cb(dataWidget3,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *dataWidget3Label = lv_label_create(dataWidget3);
    lv_label_set_text(dataWidget3Label, "Cadance");
    lv_obj_t *dataWidget3Value = lv_label_create(dataWidget3);
    lv_label_set_text(dataWidget3Value, "Value 3");
    lv_obj_set_pos(dataWidget3Value, 10, 30);



    dataWidget4 = lv_obj_create(activityPageDataTile);
    lv_obj_set_size(dataWidget4, 128, 115);
    lv_obj_set_pos(dataWidget4, 174, 182);
    lv_obj_add_event_cb(dataWidget4,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *dataWidget4Label = lv_label_create(dataWidget4);
    lv_label_set_text(dataWidget4Label, "Speed");
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
    lv_label_set_text(statWidget1Label, "Avg Speed");
    lv_obj_t *statWidget1Value = lv_label_create(statsWidget1);
    lv_label_set_text(statWidget1Value, "Value 1");
    lv_obj_set_pos(statWidget1Value, 10, 30);


    statsWidget2 = lv_obj_create(activityPageStatsTile);
    lv_obj_set_size(statsWidget2, 128, 115);
    lv_obj_set_pos(statsWidget2, 174, 32);
    lv_obj_add_event_cb(statsWidget2,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *statWidget2Label = lv_label_create(statsWidget2);
    lv_label_set_text(statWidget2Label, "Avg Cadance");
    lv_obj_t *statWidget2Value = lv_label_create(statsWidget2);
    lv_label_set_text(statWidget2Value, "Value 2");
    lv_obj_set_pos(statWidget2Value, 10, 30);


    statsWidget3 = lv_obj_create(activityPageStatsTile);
    lv_obj_set_size(statsWidget3, 128, 115);
    lv_obj_set_pos(statsWidget3, 20, 182);
    lv_obj_add_event_cb(statsWidget3,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *statWidget3Label = lv_label_create(statsWidget3);
    lv_label_set_text(statWidget3Label, "Distance left");
    lv_obj_t *statWidget3Value = lv_label_create(statsWidget3);
    lv_label_set_text(statWidget3Value, "Value 3");
    lv_obj_set_pos(statWidget3Value, 10, 30);


    statsWidget4 = lv_obj_create(activityPageStatsTile);
    lv_obj_set_size(statsWidget4, 128, 115);
    lv_obj_set_pos(statsWidget4, 174, 182);
    lv_obj_add_event_cb(statsWidget4,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *statWidget4Label = lv_label_create(statsWidget4);
    lv_label_set_text(statWidget4Label, "Timer");
    lv_obj_t *statWidget4Value = lv_label_create(statsWidget4);
    lv_label_set_text(statWidget4Value, "Value 4");
    lv_obj_set_pos(statWidget4Value, 10, 30);
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

    lv_obj_t *climbView1Label = lv_label_create(climbViewWidget1);
    lv_label_set_text(climbView1Label, "Climb Label 1");
    lv_obj_t *climbView1Value = lv_label_create(climbViewWidget1);
    lv_label_set_text(climbView1Value, "Value 1");
    lv_obj_set_pos(climbView1Value, 10, 30);


    climbViewWidget2 = lv_obj_create(activityPageClimbView);
    lv_obj_set_size(climbViewWidget2, 128, 115);
    lv_obj_set_pos(climbViewWidget2, 174, 320);
    lv_obj_add_event_cb(climbViewWidget2,   activityPageController, LV_EVENT_ALL, NULL);

    lv_obj_t *climbView2Label = lv_label_create(climbViewWidget2);
    lv_label_set_text(climbView2Label, "Climb Label 2");
    lv_obj_t *climbView2Value = lv_label_create(climbViewWidget2);
    lv_label_set_text(climbView2Value, "Value 2");
    lv_obj_set_pos(climbView2Value, 10, 30);
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
    lv_label_set_text(menuLabel, "Stop");
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

        if(target == ActivityZoomInBtn && currentZoomLevel < 17){
            currentZoomLevel++;
            return;
        }

        if(target == ActivityZoomOutBtn && currentZoomLevel > 7){
            currentZoomLevel--;
            return;
        }
        
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
            lv_obj_send_event(activityPage, LV_EVENT_VALUE_CHANGED, NULL);
            return;
        }
    }
    if(code == LV_EVENT_SCREEN_LOADED){
        if(target == activityPage && activityStarted == false){
            activityStarted = true;
            activityUITimer = lv_timer_create(activityUIUpdateTimer, 1000, NULL);
            if(trackFileName != ""){
                activityTrack = new GpxParser(trackFileName.c_str());
                trackLoaded = true;
            } else {
                trackLoaded = false;
            }
            if(trackLoaded == true){
                log_e("Track wordt geladen.");
                activityTrack->loadTrack();
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
                if(bleSensors.speedValue > 0){
                    lv_label_set_text_fmt(label, "%.1f", (double)bleSensors.speedValue);
                } else {
                    lv_label_set_text_fmt(label, "%.1f", (double)gps.gpsData.speed);
                }
            }
        }

        if(activeTileController == activityPageDataTile){
            if(target == dataWidget1){
                lv_obj_t* label = lv_obj_get_child(target, 1);
                int sec, min, hours;
                sec = newActivity->getActivityData().timer;
                min = sec/60;
                hours = min/60;
                lv_label_set_text_fmt(label, "%d:%d:%d", hours, int(min%60), int(sec%60));
                log_e("sec: %d", sec);
            }
            if(target == dataWidget2){
                lv_obj_t* label = lv_obj_get_child(target, 1);
                lv_label_set_text_fmt(label, "%.1f", newActivity->getActivityData().distance);
            }
            if(target == dataWidget3){
                lv_obj_t* label = lv_obj_get_child(target, 1);
                lv_label_set_text_fmt(label, "%d", bleSensors.cadanceValue);
            }
            if(target == dataWidget4){
                lv_obj_t* label = lv_obj_get_child(target, 1);
                if(bleSensors.speedValue > 0){
                    lv_label_set_text_fmt(label, "%.1f", (double)bleSensors.speedValue);
                } else {
                    lv_label_set_text_fmt(label, "%.1f", (double)gps.gpsData.speed);
                }
            }
        }

        if(activeTileController == activityPageStatsTile){
            if(target == statsWidget1){
                
            }
            if(target == statsWidget2){
                // Update widget 2 data
            }
            if(target == statsWidget3){
                // Distance left
            }
            if(target == statsWidget4){
                lv_obj_t* label = lv_obj_get_child(target, 1);
                int sec, min, hours;
                sec = newActivity->getActivityData().timer;
                min = sec/60;
                hours = min/60;
                lv_label_set_text_fmt(label, "%d:%d:%d", hours, int(min%60), int(sec%60));
            }
        }

        if(activeTileController == activityPageClimbView && climbViewActive){
            if(target == climbGraph){
                if(activityTrack != nullptr){
                    size_t startIndexClimb;
                    size_t endIndexClimb;
                    if(activityTrack->climbs.size() > 0){
                        log_e("climbs beschikbaar: %d", activityTrack->climbs.size());
                        startIndexClimb = activityTrack->climbs[0].startIndex;
                        endIndexClimb = activityTrack->climbs[0].endIndex;

                        // zet de climbs op de grafiek
                        lv_chart_set_point_count(climbGraph, endIndexClimb - startIndexClimb + 1);
                        for(uint32_t i = startIndexClimb; i < endIndexClimb; i++) {
                            lv_chart_set_next_value(climbGraph, elevationSerieAct, activityTrack->elevationProfile[i].elevation);
                        }
                    } else {
                        log_e("geen climbs beschikbaar");
                    }
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
        activityMap.generateMap(currentZoomLevel);
        if(activityTrack != nullptr){
            activityMap.displayGpxRoute(activityTrack->trackData);
        }
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

    if(activeTile == activityPageClimbView && climbViewActive){
        lv_obj_send_event(climbGraph, LV_EVENT_REFRESH, NULL);
        
        lv_obj_send_event(climbViewWidget1, LV_EVENT_REFRESH, NULL);
        lv_obj_send_event(climbViewWidget2, LV_EVENT_REFRESH, NULL);
        return;
    }
    return;
}