#pragma once

#include "widget.hpp"

class SmallWidget : public Widget {
    public:
        SmallWidget();
        SmallWidget(lv_obj_t* parent);
        void createSmallWidget(int col, int row, const char * type);
    private:
        int posX;
        int posY;
};