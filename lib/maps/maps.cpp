#include "maps.hpp"
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cstring>

extern GPS gps;

Maps::Maps(){
}

uint16_t Maps::lon2posx(float f_lon, uint8_t zoom, uint16_t tileSize)
{
    return static_cast<uint16_t>(((f_lon + 180.0f) / 360.0f * (1 << zoom) * tileSize)) % tileSize;
}

uint16_t Maps::lat2posy(float f_lat, uint8_t zoom, uint16_t tileSize)
{
    float lat_rad = f_lat * static_cast<float>(M_PI) / 180.0f;
    float siny = tanf(lat_rad) + 1.0f / cosf(lat_rad);
    float merc_n = logf(siny);
    float scale = (1 << zoom) * tileSize;
    return static_cast<uint16_t>(((1.0f - merc_n / static_cast<float>(M_PI)) / 2.0f * scale)) % tileSize;
}

uint32_t Maps::lon2tilex(float f_lon, uint8_t zoom)
{
    float rawTile = (f_lon + 180.0f) / 360.0f * (1 << zoom);
    rawTile += 1e-6f;
    return static_cast<uint32_t>(rawTile);
}

uint32_t Maps::lat2tiley(float f_lat, uint8_t zoom)
{
    float lat_rad = f_lat * static_cast<float>(M_PI) / 180.0f;
    float siny = tanf(lat_rad) + 1.0f / cosf(lat_rad);
    float merc_n = logf(siny);
    float rawTile = (1.0f - merc_n / static_cast<float>(M_PI)) / 2.0f * (1 << zoom);
    rawTile += 1e-6f;
    return static_cast<uint32_t>(rawTile);
}

float Maps::tilex2lon(uint32_t tileX, uint8_t zoom)
{
    return static_cast<float>(tileX) * 360.0f / (1 << zoom) - 180.0f;
}

float Maps::tiley2lat(uint32_t tileY, uint8_t zoom)
{
    float scale = static_cast<float>(1 << zoom);
    float n = static_cast<float>(M_PI) * (1.0f - 2.0f * static_cast<float>(tileY) / scale);
    return 180.0f / static_cast<float>(M_PI) * atanf(sinhf(n));
}

Maps::MapTile Maps::getMapTile(float lon, float lat, uint8_t zoomLevel, int8_t offsetX, int8_t offsetY)
{
    MapTile data;
    data.tilex = Maps::lon2tilex(lon, zoomLevel) + offsetX;
    data.tiley = Maps::lat2tiley(lat, zoomLevel) + offsetY;
    data.zoom = zoomLevel;
    data.lat = lat; 
    data.lon = lon;

    snprintf(data.file, sizeof(data.file), mapRenderFolder, zoomLevel, data.tilex, data.tiley);

    return data;
}

Maps::tileBounds Maps::getTileBounds(uint32_t tileX, uint32_t tileY, uint8_t zoom)
{
    tileBounds bounds;
    bounds.lon_min = Maps::tilex2lon(tileX, zoom);
    bounds.lat_min = Maps::tiley2lat(tileY + 1, zoom);
    bounds.lon_max = Maps::tilex2lon(tileX + 1, zoom);
    bounds.lat_max = Maps::tiley2lat(tileY, zoom);
    return bounds;
}

bool Maps::isCoordInBounds(float lat, float lon, tileBounds bound)
{
    return (lat >= bound.lat_min && lat <= bound.lat_max &&
            lon >= bound.lon_min && lon <= bound.lon_max);
}

Maps::ScreenCoord Maps::coord2ScreenPos(float lon, float lat, uint8_t zoomLevel, uint16_t tileSize)
{
    ScreenCoord data;
    data.posX = Maps::lon2posx(lon, zoomLevel, tileSize);
    data.posY = Maps::lat2posy(lat, zoomLevel, tileSize);
    return data;
}

void Maps::coords2map(float lat, float lon, tileBounds bound, uint16_t *pixelX, uint16_t *pixelY)
{
    float lon_ratio = (lon - bound.lon_min) / (bound.lon_max - bound.lon_min);
    float lat_ratio = (bound.lat_max - lat) / (bound.lat_max - bound.lat_min);

    *pixelX = (uint16_t)(lon_ratio * Maps::tileWidth);
    *pixelY = (uint16_t)(lat_ratio * Maps::tileHeight);
}

void Maps::showNoMap(TFT_eSprite &map)
{
    map.drawPngFile(noMapFile, (Maps::mapScrWidth / 2) - 50, (Maps::mapScrHeight / 2) - 50);
    map.drawCenterString("NO MAP FOUND", (Maps::mapScrWidth / 2), (Maps::mapScrHeight >> 1) + 65, &fonts::DejaVu18);
}

void Maps::initMap(uint16_t mapWidth, uint16_t mapHeight)
{
    Maps::mapScrHeight = mapHeight;
    Maps::mapScrWidth = mapWidth;

    // Reserve PSRAM for buffer map
    Maps::mapTempSprite.deleteSprite();
    Maps::mapTempSprite.createSprite(tileHeight, tileWidth);

    Maps::oldMapTile = {};     // Old Map tile coordinates and zoom
    Maps::currentMapTile = {}; // Current Map tile coordinates and zoom
    Maps::roundMapTile = {};    // Boundaries Map tiles

    Maps::totalBounds = {90.0, -90.0, 180.0, -180.0};
}

void Maps::deleteMapScrSprites()
{
    Maps::mapSprite.deleteSprite();
}

void Maps::createMapScrSprites()
{
    Maps::mapBuffer = Maps::mapSprite.createSprite(Maps::mapScrWidth, Maps::mapScrHeight);
}

void Maps::generateMap(uint8_t zoom){
    Maps::zoomLevel = zoom;

    bool missingMap = false;
    bool foundRoundMap = false;

    const float lat = gps.gpsData.latitude;  //51.252376;
    const float lon = gps.gpsData.longitude;  //4.438024;

    Maps::currentMapTile = Maps::getMapTile(lon, lat, Maps::zoomLevel, 0, 0);

    if (strcmp(Maps::currentMapTile.file,  Maps::oldMapTile.file) != 0 ||
        Maps::currentMapTile.zoom != Maps::oldMapTile.zoom ||
        Maps::currentMapTile.tilex != Maps::oldMapTile.tilex ||
        Maps::currentMapTile.tiley != Maps::oldMapTile.tiley)
    {
        const int16_t size = Maps::mapTileSize;

        Maps::mapTempSprite.fillScreen(TFT_WHITE);

        Maps::isMapFound = Maps::mapTempSprite.drawPngFile(Maps::currentMapTile.file, size, size);

        Maps::oldMapTile = Maps::currentMapTile;

        strcpy(Maps::oldMapTile.file, Maps::currentMapTile.file);

        if(!Maps::isMapFound){
            Maps::isMapFound = false;
            Maps::mapTempSprite.fillScreen(TFT_BLACK);
            Maps::showNoMap(Maps::mapTempSprite);
        }
        else {
            Maps::totalBounds = Maps::getTileBounds(Maps::currentMapTile.tilex, Maps::currentMapTile.tiley, Maps::zoomLevel);

            const int8_t startX = -1;
            const int8_t startY = -1;

            for(int8_t y = startY; y <= startY + 2; y++){
                const int16_t offSetY = (y - startY) * size;

                for(int8_t x = startX; x <= startX + 2; x++){
                    if(x == 0 && y == 0) continue; // Skip center tile (already loaded)

                    const int16_t offSetX = (x - startX) * size;
                    
                    Maps::roundMapTile = Maps::getMapTile(Maps::currentMapTile.lon, Maps::currentMapTile.lat, Maps::zoomLevel, x, y);

                    foundRoundMap = Maps::mapTempSprite.drawPngFile(Maps::roundMapTile.file, offSetX, offSetY);

                    if(!foundRoundMap){
                        Maps::mapTempSprite.fillRect(offSetX, offSetY, size, size, TFT_BLACK);
                        Maps::mapTempSprite.drawPngFile(noMapFile, offSetX + (size / 2) - 50, offSetY + (size / 2) - 50);
                        missingMap = true;
                    }
                    else {
                        const tileBounds currentBounds = Maps::getTileBounds(Maps::roundMapTile.tilex, Maps::roundMapTile.tiley, Maps::zoomLevel);
                        
                        if (currentBounds.lat_min < Maps::totalBounds.lat_min)
                            Maps::totalBounds.lat_min = currentBounds.lat_min;
                        if (currentBounds.lat_max > Maps::totalBounds.lat_max)
                            Maps::totalBounds.lat_max = currentBounds.lat_max;
                        if (currentBounds.lon_min < Maps::totalBounds.lon_min)
                            Maps::totalBounds.lon_min = currentBounds.lon_min;
                        if (currentBounds.lon_max > Maps::totalBounds.lon_max)
                            Maps::totalBounds.lon_max = currentBounds.lon_max;
                    }
                    
                    
                }
            }

            Maps::redrawMap = true;
        }
    }
}

void Maps::displayMap(){
    if(!Maps::isMapFound){
        Maps::mapTempSprite.pushSprite(&mapSprite, 0, 0, TFT_TRANSPARENT);
    }

    uint16_t mapHeading = 0;

    mapHeading = gps.gpsData.heading;

    const uint16_t size = Maps::mapTileSize;

    if(Maps::followGps){
        const float lat = gps.gpsData.latitude;    //51.252376;
        const float lon = gps.gpsData.longitude;   //4.438024;
        Maps::navArrowPosition = Maps::coord2ScreenPos(lon, lat, Maps::zoomLevel, Maps::mapTileSize);
        if(Maps::zoomLevel >= 15){
            Maps::mapTempSprite.fillCircle(Maps::mapTileSize + Maps::navArrowPosition.posX,
                                        Maps::mapTileSize + Maps::navArrowPosition.posY,
                                         7, TFT_RED);
        }
        else {
            Maps::mapTempSprite.fillCircle(Maps::mapTileSize + Maps::navArrowPosition.posX,
                                        Maps::mapTileSize + Maps::navArrowPosition.posY,
                                         4, TFT_RED);
        }
        if(Maps::turnOnGpsHeading){
            Maps::mapTempSprite.setPivot(Maps::mapTileSize + Maps::navArrowPosition.posX,
                                     Maps::mapTileSize + Maps::navArrowPosition.posY);
            Maps::mapTempSprite.pushRotated(&mapSprite, 360 - mapHeading);
        } else {
            Maps::mapTempSprite.setPivot(Maps::mapTileSize + Maps::navArrowPosition.posX,
                                     Maps::mapTileSize + Maps::navArrowPosition.posY);
            Maps::mapTempSprite.pushRotated(&mapSprite, 0);
        };
    }
}   

void Maps::updateMap()
{
    Maps::oldMapTile = {};
}

void Maps::centerOnGps(float lat, float lon)
{
    Maps::followGps = true;
    Maps::currentMapTile.tilex = Maps::lon2tilex(lon, Maps::currentMapTile.zoom);
    Maps::currentMapTile.tiley = Maps::lat2tiley(lat, Maps::currentMapTile.zoom);
    Maps::currentMapTile.lat = lat;
    Maps::currentMapTile.lon = lon;
}