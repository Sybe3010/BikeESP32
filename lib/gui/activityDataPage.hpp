#pragma once

#include "globalGuiDef.h"

class ActivityDataPage{
    private:
        bool isDataPageLoaded;
        bool isMapVisible;
    public:
        ActivityDataPage();

        void createDataPage();
        void updateDataPageInformation();
};