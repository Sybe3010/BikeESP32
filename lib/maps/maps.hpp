#pragma once

#include <Arduino.h>
#include "tft.hpp"

#include "gps.hpp"

static const char *mapRenderFolder PROGMEM = "/sdcard/MAP/%u/%u/%u.png"; /**< De plaats waar de kaart tiles staan op de sd kaart. */
static const char *noMapFile PROGMEM = "/spiffs/NOMAP.png";              /**< De plaats waar de afbeelding staat als er geen tiles gevonden zijn: 
nog op de sd-kaart te zetten */

class Maps {
    private:
        uint8_t zoomLevel;                                                           /**< Huidige zoomniveau */
        static const uint16_t tileHeight = 768;                                      /**< Tile 9x9 hoogte  */
        static const uint16_t tileWidth = 768;                                       /**< Tile 9x9 breedte */
        static const uint16_t mapTileSize = 256;                             	     /**< Map tile size */

        struct MapTile{                                                              /**< Struct om tile informatie op te slaan */   
            char file[255];
            uint32_t tilex;
            uint32_t tiley;
            uint8_t zoom;
            float lon;
            float lat;
        };

        struct tileBounds{                                                           /**< Struct om de grenzen van een tile op te slaan */
            float lon_min;
            float lat_min;
            float lon_max;
            float lat_max;
        };

        struct ScreenCoord{                                                         /**< Struct om scherm coördinaten op te slaan */
            uint16_t posX;
            uint16_t posY;
        };

        TFT_eSprite mapTempSprite = TFT_eSprite(&tft);                              /**< volledige map sprite (not showed) */
        TFT_eSprite mapSprite = TFT_eSprite(&tft);                                  /**< getoonde map sprite */
        tileBounds totalBounds;                                                     /**< totale grenzen van de getoonde kaart */

        ScreenCoord navArrowPosition; 												/**< plaats van gebruiker op de kaart*/

        static uint16_t lon2posx(float f_lon, uint8_t zoom, uint16_t tileSize);
        static uint16_t lat2posy(float f_lat, uint8_t zoom, uint16_t tileSize);
        static uint32_t lon2tilex(float f_lon, uint8_t zoom);
        static uint32_t lat2tiley(float f_lat, uint8_t zoom);
        static float tilex2lon(uint32_t tileX, uint8_t zoom);
        static float tiley2lat(uint32_t tileY, uint8_t zoom);

        tileBounds getTileBounds(uint32_t tileX, uint32_t tileY, uint8_t zoom);
        bool isCoordInBounds(float lat, float lon, tileBounds bound);
        ScreenCoord coord2ScreenPos(float lon, float lat, uint8_t zoomLevel, uint16_t tileSize);
        void showNoMap(TFT_eSprite &map);
        void panMap(int8_t dx, int8_t dy);

    public:
        MapTile oldMapTile;                                            /**< Old Map tile coordinates and zoom */
        MapTile currentMapTile;                                         /**< Current Map tile coordinates and zoom */
        MapTile roundMapTile;

        int8_t tileX = 0;                                              /**< Map tile x counter */
        int8_t tileY = 0;                                              /**< Map tile y counter */
        int16_t offsetX = 0;                                           /**< Accumulative X scroll map offset */
        int16_t offsetY = 0;                                           /**< Accumulative Y scroll map offset */
        bool redrawMap = true;                                         /**< Flag to indicate need redraw Map */
        bool followGps = true;                                         /**< Flag to indicate if map follow GPS signal */
        bool isMapFound = false; 
        uint16_t mapScrHeight = 320;                                         /**< Screen map size height */
        uint16_t mapScrWidth = 455;   
        void* mapBuffer;   
        bool turnOnGpsHeading = false;                             /**< Flag to indicate if map rotate with GPS heading */

        Maps();
        MapTile getMapTile(float lon, float lat, uint8_t zoomLevel, int8_t offsetX, int8_t offsetY);
        void initMap(uint16_t mapWidth, uint16_t mapHeight);
        void deleteMapScrSprites();
        void createMapScrSprites();
        void generateMap(uint8_t zoom);
        void displayMap();
        void updateMap();
        void centerOnGps(float lat, float lon);
};