#include "activityPage.hpp"

ActivityDataPage page1(MAP_AND_2_WIDGETS);

lv_obj_t *activityPage;

void createActivityPage(){
    activityPage = lv_tileview_create(NULL);
    
    page1.dataPage = lv_tileview_add_tile(activityPage, 0, 0, LV_DIR_LEFT);

    page1.setWidgetTypes(ACTIVITY_TIMER, ACTIVITY_SPEED);
    page1.createDataPage();
}