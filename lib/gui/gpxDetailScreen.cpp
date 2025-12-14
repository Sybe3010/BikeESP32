#include "gpxDetailScreen.hpp"

String gpxFileFolder;

lv_obj_t *gpxDetailsScreen;

lv_obj_t *routePreviewCanvas;
lv_obj_t *gpxDetailsScreenBackButton;
lv_obj_t *routeLenghtBox;
lv_obj_t *routeAscentBox;
lv_obj_t *routeElevationProfileCanvas;
lv_obj_t *routeElevationChart;
lv_obj_t *routeStartButton;
lv_obj_t *moreDetailsButton;

lv_obj_t *routeLenghtLabel;
lv_obj_t *routeAscentLabel;

lv_chart_series_t * elevationSerie;

void gpxDetailsBackButtonEvent(lv_event_t *e){
    lv_screen_load(gpxListScreen);
}

void makeGpxDetailsScreen(){
    gpxDetailsScreen = lv_obj_create(NULL);
    lv_obj_set_size(gpxDetailsScreen, 320, 455);
    lv_obj_set_pos(gpxDetailsScreen, 0, 25);

    routePreviewCanvas = lv_obj_create(gpxDetailsScreen);
    lv_obj_set_size(routePreviewCanvas, 320, 150);
    lv_obj_set_pos(routePreviewCanvas, 0, 25);
    lv_obj_remove_flag(routePreviewCanvas, LV_OBJ_FLAG_SCROLLABLE);

    gpxDetailsScreenBackButton = lv_button_create(gpxDetailsScreen);
    lv_obj_set_size(gpxDetailsScreenBackButton, 50, 50);
    lv_obj_set_pos(gpxDetailsScreenBackButton, 10, 10);
    lv_obj_add_event_cb(gpxDetailsScreenBackButton, gpxDetailsBackButtonEvent, LV_EVENT_CLICKED, NULL);
    lv_obj_t* backLabel = lv_label_create(gpxDetailsScreenBackButton);
    lv_label_set_text(backLabel, "<");
    lv_obj_set_style_text_font(backLabel, &lv_font_montserrat_28, 0);

    routeLenghtBox = lv_obj_create(gpxDetailsScreen);
    lv_obj_set_size(routeLenghtBox, 160, 75);
    lv_obj_set_pos(routeLenghtBox, 0, 175);
    lv_obj_remove_flag(routeLenghtBox, LV_OBJ_FLAG_SCROLLABLE);

    routeAscentBox = lv_obj_create(gpxDetailsScreen);
    lv_obj_set_size(routeAscentBox, 160, 75);
    lv_obj_set_pos(routeAscentBox, 160, 175);
    lv_obj_remove_flag(routeAscentBox, LV_OBJ_FLAG_SCROLLABLE);

    routeElevationProfileCanvas = lv_obj_create(gpxDetailsScreen);
    lv_obj_set_size(routeElevationProfileCanvas, 320, 100);
    lv_obj_set_pos(routeElevationProfileCanvas, 0, 250);
    lv_obj_remove_flag(routeElevationProfileCanvas, LV_OBJ_FLAG_SCROLLABLE);



    routeElevationChart = lv_chart_create(routeElevationProfileCanvas);
    lv_obj_set_size(routeElevationChart, 300, 80);
    lv_obj_set_pos(routeElevationChart, 10, 10);
    lv_obj_set_style_pad_all(routeElevationChart, 0, 0);
    lv_obj_set_style_radius(routeElevationChart, 0, 0);
    lv_obj_center(routeElevationChart);
    lv_obj_remove_flag(routeElevationChart, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(routeElevationChart, drawElevationChartEvent, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(routeElevationChart, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    elevationSerie = lv_chart_add_series(routeElevationChart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);

    moreDetailsButton = lv_button_create(gpxDetailsScreen);
    lv_obj_set_size(moreDetailsButton, 120, 70);
    lv_obj_set_pos(moreDetailsButton, 20, 375);

    routeStartButton = lv_button_create(gpxDetailsScreen);
    lv_obj_set_size(routeStartButton, 120, 70);
    lv_obj_set_pos(routeStartButton, 180, 375);

    //Labels
    routeLenghtLabel = lv_label_create(routeLenghtBox);
    lv_label_set_text(routeLenghtLabel, "Lenght: --");

    routeAscentLabel = lv_label_create(routeAscentBox);
    lv_label_set_text(routeAscentLabel, "Ascent: --");
}

void fillGpxDetailsScreen(){
    GpxParser details = GpxParser(gpxFileFolder.c_str());

    details.loadTrack();
    lv_label_set_text_fmt(routeLenghtLabel, "Lenght: %.1f km", details.trackLenght / 1000);
    lv_label_set_text_fmt(routeAscentLabel, "Ascent: %d m", details.totalAscent);
    
    lv_chart_set_point_count(routeElevationChart, details.elevationProfile.size());
    uint32_t i;
    for(i = 0; i < details.elevationProfile.size(); i++) {
        lv_chart_set_next_value(routeElevationChart, elevationSerie, details.elevationProfile[i].elevation);
    }
}

void drawElevationChartEvent(lv_event_t *e){
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);

    if(base_dsc->part == LV_PART_ITEMS && lv_draw_task_get_type(draw_task) == LV_DRAW_TASK_TYPE_LINE) {
        add_faded_area(e);

    }
}
void add_faded_area(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);

    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);

    const lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
    lv_color_t ser_color = lv_chart_get_series_color(obj, ser);

    /*Draw a triangle below the line witch some opacity gradient*/
    lv_draw_line_dsc_t * draw_line_dsc = (lv_draw_line_dsc_t *)lv_draw_task_get_draw_dsc(draw_task);
    lv_draw_triangle_dsc_t tri_dsc;

    lv_draw_triangle_dsc_init(&tri_dsc);
    tri_dsc.p[0].x = draw_line_dsc->p1.x;
    tri_dsc.p[0].y = draw_line_dsc->p1.y;
    tri_dsc.p[1].x = draw_line_dsc->p2.x;
    tri_dsc.p[1].y = draw_line_dsc->p2.y;
    tri_dsc.p[2].x = draw_line_dsc->p1.y < draw_line_dsc->p2.y ? draw_line_dsc->p1.x : draw_line_dsc->p2.x;
    tri_dsc.p[2].y = LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y);
    tri_dsc.bg_grad.dir = LV_GRAD_DIR_VER;

    int32_t full_h = lv_obj_get_height(obj);
    int32_t fract_uppter = (int32_t)(LV_MIN(draw_line_dsc->p1.y, draw_line_dsc->p2.y) - coords.y1) * 255 / full_h;
    int32_t fract_lower = (int32_t)(LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y) - coords.y1) * 255 / full_h;
    tri_dsc.bg_grad.stops[0].color = ser_color;
    tri_dsc.bg_grad.stops[0].opa = 255;
    //tri_dsc.bg_grad.stops[0].opa = (lv_opa_t)(255 - fract_uppter);
    tri_dsc.bg_grad.stops[0].frac = 0;
    tri_dsc.bg_grad.stops[1].color = ser_color;
    tri_dsc.bg_grad.stops[1].opa = 255;
    //tri_dsc.bg_grad.stops[1].opa = (lv_opa_t)(255 - fract_lower);
    tri_dsc.bg_grad.stops[1].frac = 255;

    lv_draw_triangle(base_dsc->layer, &tri_dsc);

    /*Draw rectangle below the triangle*/
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_grad.dir = LV_GRAD_DIR_VER;
    rect_dsc.bg_grad.stops[0].color = ser_color;
    rect_dsc.bg_grad.stops[0].frac = 0;
    rect_dsc.bg_grad.stops[0].opa = 255;
    //rect_dsc.bg_grad.stops[0].opa = (lv_opa_t)(255 - fract_lower);
    rect_dsc.bg_grad.stops[1].color = ser_color;
    rect_dsc.bg_grad.stops[1].frac = 255;
    //rect_dsc.bg_grad.stops[1].opa = 0;
    rect_dsc.bg_grad.stops[1].opa = 255;
    
    lv_area_t rect_area;
    rect_area.x1 = (int32_t)draw_line_dsc->p1.x;
    rect_area.x2 = (int32_t)draw_line_dsc->p2.x - 1;
    rect_area.y1 = (int32_t)LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y);
    rect_area.y2 = (int32_t)coords.y2;
    lv_draw_rect(base_dsc->layer, &rect_dsc, &rect_area);
}