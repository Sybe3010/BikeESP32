#pragma once

#include "widget.hpp"
#include "activityGlobal.h"

class SmallWidget : public Widget {
    public:
        SmallWidget();
        SmallWidget(lv_obj_t* parent);
        void createSmallWidget(int col, int row, typeData type);
    private:
        int posX;
        int posY;
};