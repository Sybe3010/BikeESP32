#include "smallWidget.hpp"

SmallWidget::SmallWidget(lv_obj_t * parent) : Widget(parent){

}

void SmallWidget::createSmallWidget(int col, int row, typeData type){
    switch (col)
    {
        case 1:
            posX = 20;
            break;
        case 2:
            posX = 174;
            break;
    }
    switch (row)
    {
        case 1:
            posY = 32;
            break;
        case 2:
            posY = 182;
            break;
        case 3:
            posY = 332;
            break;
    }
    createWidget(128, 115, posX, posY, type);
}