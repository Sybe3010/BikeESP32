#include "maps.hpp"
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cstring>

extern GPS gps; // Gebruik het GPS object dat al is aangemaakt

Maps::Maps(){
}

/// @brief Converteert lengtegraad naar een x-pixel voor een bepaald zoomniveau
/// @param f_lon Lengtegraad
/// @param zoom Zoomniveau
/// @param tileSize Grootte van de tile
/// @return x pixel in de tile
/// @note Deze functie is voor het berekenen van de pixelpositie binnen een tile.
uint16_t Maps::lon2posx(float f_lon, uint8_t zoom, uint16_t tileSize)
{
    return static_cast<uint16_t>(((f_lon + 180.0f) / 360.0f * (1 << zoom) * tileSize)) % tileSize;
}

/// @brief Converteert breedteligging naar een y-pixel voor een bepaald zoomniveau
/// @param f_lon Breedteligging
/// @param zoom Zoomniveau
/// @param tileSize Grootte van de tile
/// @return y pixel in de tile
/// @note Deze functie is voor het berekenen van de pixelpositie binnen een tile.
uint16_t Maps::lat2posy(float f_lat, uint8_t zoom, uint16_t tileSize)
{
    float lat_rad = f_lat * static_cast<float>(M_PI) / 180.0f;
    float siny = tanf(lat_rad) + 1.0f / cosf(lat_rad);
    float merc_n = logf(siny);
    float scale = (1 << zoom) * tileSize;
    return static_cast<uint16_t>(((1.0f - merc_n / static_cast<float>(M_PI)) / 2.0f * scale)) % tileSize;
}


/// @brief Converteert lengtegraad naar tile x voor een bepaald zoomniveau
/// @param f_lon Lengtegraad
/// @param zoom Zoomniveau
/// @return tile x coördinaat
/// @note Deze functie is voor het bepalen van welke tile geladen moet worden.
uint32_t Maps::lon2tilex(float f_lon, uint8_t zoom)
{
    float rawTile = (f_lon + 180.0f) / 360.0f * (1 << zoom);
    rawTile += 1e-6f;
    return static_cast<uint32_t>(rawTile);
}

/// @brief Converteert breedteligging naar tile y voor een bepaald zoomniveau
/// @param f_lat Breedteligging
/// @param zoom Zoomniveau
/// @return tile y coördinaat
/// @note Deze functie is voor het bepalen van welke tile geladen moet worden.
uint32_t Maps::lat2tiley(float f_lat, uint8_t zoom)
{
    float lat_rad = f_lat * static_cast<float>(M_PI) / 180.0f;
    float siny = tanf(lat_rad) + 1.0f / cosf(lat_rad);
    float merc_n = logf(siny);
    float rawTile = (1.0f - merc_n / static_cast<float>(M_PI)) / 2.0f * (1 << zoom);
    rawTile += 1e-6f;
    return static_cast<uint32_t>(rawTile);
}

/// @brief Converteert tile x naar lengtegraad voor een bepaald zoomniveau
/// @param tileX Tile x coördinaat
/// @param zoom Zoomniveau
/// @return Lengtegraad
/// @note Deze functie is voor het bepalen van de grenzen van een tile.
float Maps::tilex2lon(uint32_t tileX, uint8_t zoom)
{
    return static_cast<float>(tileX) * 360.0f / (1 << zoom) - 180.0f;
}

/// @brief Converteert tile y naar breedteligging voor een bepaald zoomniveau
/// @param tileY Tile y coördinaat
/// @param zoom Zoomniveau
/// @return Deze functie is voor het bepalen van de grenzen van een tile.
float Maps::tiley2lat(uint32_t tileY, uint8_t zoom)
{
    float scale = static_cast<float>(1 << zoom);
    float n = static_cast<float>(M_PI) * (1.0f - 2.0f * static_cast<float>(tileY) / scale);
    return 180.0f / static_cast<float>(M_PI) * atanf(sinhf(n));
}
 
/// @brief Haalt de map tile informatie op voor gegeven lengtegraad, breedteligging en zoomniveau
/// @param lon Lengtegraad
/// @param lat Breedteligging
/// @param zoomLevel Zoomniveau
/// @param offsetX Offset in x richting (voor naburige tiles)
/// @param offsetY Offset in y richting (voor naburige tiles)
/// @return MapTile struct met tile informatie
/// @note Deze functie berekent de tile coördinaten en bestandsnaam voor een specifieke locatie en zoomniveau. Met de offsets kunnen naburige tiles worden opgehaald.
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


/// @brief Haalt de grenzen van een tile op basis van tile coördinaten en zoomniveau
/// @param tileX Tile x coördinaat
/// @param tileY Tile y coördinaat
/// @param zoom  Zoomniveau
/// @return TileBounds struct met de grenzen van de tile
/// @note Deze functie berekent de geografische grenzen (lengte- en breedtegraad) van een specifieke tile.
Maps::tileBounds Maps::getTileBounds(uint32_t tileX, uint32_t tileY, uint8_t zoom)
{
    tileBounds bounds;
    bounds.lon_min = Maps::tilex2lon(tileX, zoom);
    bounds.lat_min = Maps::tiley2lat(tileY + 1, zoom);
    bounds.lon_max = Maps::tilex2lon(tileX + 1, zoom);
    bounds.lat_max = Maps::tiley2lat(tileY, zoom);
    return bounds;
}

/// @brief Controleert of gegeven coördinaten binnen de grenzen van een tile vallen
/// @param lat Breedteligging
/// @param lon Lengtegraad
/// @param bound TileBounds struct met de grenzen van de tile
/// @return true als de coördinaten binnen de grenzen vallen, anders false
/// @note Deze functie wordt gebruikt om te bepalen of een specifieke locatie binnen de grenzen van een tile ligt.
bool Maps::isCoordInBounds(float lat, float lon, tileBounds bound)
{
    return (lat >= bound.lat_min && lat <= bound.lat_max &&
            lon >= bound.lon_min && lon <= bound.lon_max);
}

/// @brief Converteert lengte- en breedtegraad naar scherm coördinaten voor een bepaald zoomniveau
/// @param lon Lengtegraad
/// @param lat Breedteligging
/// @param zoomLevel Zoomniveau
/// @param tileSize Grootte van de tile
/// @return ScreenCoord struct met scherm coördinaten
/// @note Deze functie berekent de pixelpositie op het scherm voor een specifieke geografische locatie en zoomniveau.
Maps::ScreenCoord Maps::coord2ScreenPos(float lon, float lat, uint8_t zoomLevel, uint16_t tileSize)
{
    ScreenCoord data;
    data.posX = Maps::lon2posx(lon, zoomLevel, tileSize);
    data.posY = Maps::lat2posy(lat, zoomLevel, tileSize);
    return data;
}

/// @brief Toont een "NO MAP FOUND" afbeelding op de kaart sprite
/// @param map Referentie naar de kaart sprite
/// @note Deze functie wordt aangeroepen wanneer een map tile niet gevonden kan worden. Het toont een standaard afbeelding en tekst op de kaart sprite.
void Maps::showNoMap(TFT_eSprite &map)
{
    map.drawPngFile(noMapFile, (Maps::mapScrWidth / 2) - 50, (Maps::mapScrHeight / 2) - 50);
    map.drawCenterString("NO MAP FOUND", (Maps::mapScrWidth / 2), (Maps::mapScrHeight >> 1) + 65, &fonts::DejaVu18);
}

/// @brief Initialiseert de kaart met gegeven breedte en hoogte
/// @param mapWidth Breedte van de kaart
/// @param mapHeight Hoogte van de kaart
/// @note Deze functie reserveert geheugen voor de kaart sprite en initialiseert de map tile structuren.
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

/// @brief Verwijdert de kaart sprite
/// @note Deze functie wordt gebruikt om geheugen vrij te maken door de kaart sprite te verwijderen.
void Maps::deleteMapScrSprites()
{
    Maps::mapSprite.deleteSprite();
}

/// @brief Maakt de kaart sprite aan met de juiste afmetingen
/// @note Deze functie initialiseert de kaart sprite met de breedte en hoogte die eerder zijn ingesteld.
void Maps::createMapScrSprites()
{
    Maps::mapBuffer = Maps::mapSprite.createSprite(Maps::mapScrWidth, Maps::mapScrHeight);
}

/// @brief Genereert de kaart op basis van het huidige GPS-coördinaat en zoomniveau
/// @param zoom Zoomniveau
/// @note Deze functie laadt de benodigde map tiles rondom de huidige GPS-locatie en tekent deze op de kaart sprite. Als een tile niet gevonden wordt, wordt een "NO MAP FOUND" afbeelding getoond.
void Maps::generateMap(uint8_t zoom){
    Maps::zoomLevel = zoom;

    bool missingMap = false;
    bool foundRoundMap = false;

    const float lat = gps.gpsData.latitude;  //51.252376;
    const float lon = gps.gpsData.longitude;  //4.438024;

    // Bepaal de huidige tile op basis van GPS-coördinaten
    Maps::currentMapTile = Maps::getMapTile(lon, lat, Maps::zoomLevel, 0, 0);

    // Alleen opnieuw genereren als de tile is veranderd
    if (strcmp(Maps::currentMapTile.file,  Maps::oldMapTile.file) != 0 || // vergelijk bestandsnamen: returns 0 als gelijk
        Maps::currentMapTile.zoom != Maps::oldMapTile.zoom ||
        Maps::currentMapTile.tilex != Maps::oldMapTile.tilex ||
        Maps::currentMapTile.tiley != Maps::oldMapTile.tiley)
    { 
        const int16_t size = Maps::mapTileSize;

        Maps::mapTempSprite.fillScreen(TFT_WHITE);

        Maps::isMapFound = Maps::mapTempSprite.drawPngFile(Maps::currentMapTile.file, size, size); // teken de centrale tile

        Maps::oldMapTile = Maps::currentMapTile;

        strcpy(Maps::oldMapTile.file, Maps::currentMapTile.file);

        if(!Maps::isMapFound){ // centrale tile niet gevonden
            Maps::isMapFound = false;
            Maps::mapTempSprite.fillScreen(TFT_BLACK);
            Maps::showNoMap(Maps::mapTempSprite); 
        }
        else {
            Maps::totalBounds = Maps::getTileBounds(Maps::currentMapTile.tilex, Maps::currentMapTile.tiley, Maps::zoomLevel);
            Maps::mainTileBounds = Maps::totalBounds;
            const int8_t startX = -1;
            const int8_t startY = -1;

            for(int8_t y = startY; y <= startY + 2; y++){ // Loop voor 3x3 tiles
                const int16_t offSetY = (y - startY) * size; // bereken offset voor naburige tiles

                for(int8_t x = startX; x <= startX + 2; x++){
                    if(x == 0 && y == 0) continue; // centrale tile is al getekend

                    const int16_t offSetX = (x - startX) * size; // bereken offset voor naburige tiles
                    
                    Maps::roundMapTile = Maps::getMapTile(Maps::currentMapTile.lon, Maps::currentMapTile.lat, Maps::zoomLevel, x, y);

                    foundRoundMap = Maps::mapTempSprite.drawPngFile(Maps::roundMapTile.file, offSetX, offSetY); // teken de naburige tile

                    if(!foundRoundMap){ // naburige tile niet gevonden
                        Maps::mapTempSprite.fillRect(offSetX, offSetY, size, size, TFT_BLACK);
                        Maps::mapTempSprite.drawPngFile(noMapFile, offSetX + (size / 2) - 50, offSetY + (size / 2) - 50);
                        missingMap = true;
                    }
                    else { // naburige tile gevonden
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

/// @brief Toont de gegenereerde kaart op het scherm
/// @note Deze functie tekent de kaart sprite op het scherm. Als de GPS-volgmodus is ingeschakeld, wordt ook een navigatiepijl getekend op de huidige GPS-locatie.
void Maps::displayMap(){
    if(!Maps::isMapFound){ // geen map gevonden
        Maps::mapTempSprite.pushSprite(&mapSprite, 0, 0, TFT_TRANSPARENT);
    }

    uint16_t mapHeading = 0;

    mapHeading = gps.gpsData.heading; // haal de GPS heading op

    const uint16_t size = Maps::mapTileSize; 

    if(Maps::followGps){ // GPS volgmodus aan
        float lat = gps.gpsData.latitude;    //51.252376;
        float lon = gps.gpsData.longitude;   //4.438024;
        Maps::navArrowPosition = Maps::coord2ScreenPos(lon, lat, Maps::zoomLevel, Maps::mapTileSize); // bereken positie van gebruiker op de kaart
        if(Maps::zoomLevel >= 15){ // grote zoomniveau, grotere pijl
            Maps::mapTempSprite.fillCircle(Maps::mapTileSize + Maps::navArrowPosition.posX,
                                        Maps::mapTileSize + Maps::navArrowPosition.posY,
                                         7, TFT_RED);
        }
        else { // kleinere zoomniveau, kleinere pijl
            Maps::mapTempSprite.fillCircle(Maps::mapTileSize + Maps::navArrowPosition.posX,
                                        Maps::mapTileSize + Maps::navArrowPosition.posY,
                                         4, TFT_RED);
        }

        if(_waypoints.size() > 2){
            for(size_t i = 0; i < _waypoints.size() - 1; i++){
                const auto& waypoint1 = _waypoints[i];
                const auto& waypoint2 = _waypoints[i + 1];

                bool waypoint1InTotalBounds = isCoordInBounds(waypoint1.lat, waypoint1.lon, totalBounds);
                bool waypoint2InTotalBounds = isCoordInBounds(waypoint2.lat, waypoint2.lon, totalBounds);

                if(!waypoint1InTotalBounds || !waypoint2InTotalBounds){
                    continue;
                }

                bool waypoint1InMainBounds = isCoordInBounds(waypoint1.lat, waypoint1.lon, mainTileBounds);
                bool waypoint2InMainBounds = isCoordInBounds(waypoint2.lat, waypoint2.lon, mainTileBounds);

                bool inRoundTiles = false;

                if(!waypoint1InMainBounds || !waypoint2InMainBounds){
                    inRoundTiles = true;
                }

                ScreenCoord begin = coord2ScreenPos(waypoint1.lon, waypoint1.lat, zoomLevel, Maps::mapTileSize);
                ScreenCoord einde = coord2ScreenPos(waypoint2.lon, waypoint2.lat, zoomLevel, Maps::mapTileSize);

                uint16_t offset = mapTileSize;

                uint16_t offSetX1 = mapTileSize;
                uint16_t offSetY1 = mapTileSize;
                uint16_t offSetX2 = mapTileSize;
                uint16_t offSetY2 = mapTileSize;
                if(!inRoundTiles){
                    mapTempSprite.drawWideLine( offset + begin.posX,
                                                offset + begin.posY,
                                                offset + einde.posX,
                                                offset + einde.posY,
                                                2,
                                                TFT_BLACK);
                } else {
                    if (waypoint1.lon < mainTileBounds.lon_min) {
                        offSetX1 = 0;
                    } else if (waypoint1.lon > mainTileBounds.lon_max) {
                        offSetX1 = 2 * mapTileSize;
                    } else if (waypoint1.lon == mainTileBounds.lon_min || waypoint1.lon == mainTileBounds.lon_max){
                        offSetX1 = mapTileSize;
                    }

                    if (waypoint1.lat > mainTileBounds.lat_max) {
                        offSetY1 = 0;
                    } else if (waypoint1.lat < mainTileBounds.lat_min) {
                        offSetY1 = 2 * mapTileSize;
                    } else if (waypoint1.lat == mainTileBounds.lat_min || waypoint1.lat == mainTileBounds.lat_max){
                        offSetY1 = mapTileSize;
                    }

                    if (waypoint2.lon < mainTileBounds.lon_min) {
                        offSetX2 = 0;
                    } else if (waypoint2.lon > mainTileBounds.lon_max) {
                        offSetX2 = 2 * mapTileSize;
                    } else if (waypoint2.lon == mainTileBounds.lon_min || waypoint2.lon == mainTileBounds.lon_max){
                        offSetX2 = mapTileSize;
                    }

                    if (waypoint2.lat > mainTileBounds.lat_max) {
                        offSetY2 = 0;
                    } else if (waypoint2.lat < mainTileBounds.lat_min) {
                        offSetY2 = 2 * mapTileSize;
                    } else if (waypoint2.lat == mainTileBounds.lat_min || waypoint2.lat == mainTileBounds.lat_max){
                        offSetY2 = mapTileSize;
                    }
                    mapTempSprite.drawWideLine( offSetX1 + begin.posX,
                                                offSetY1 + begin.posY,
                                                offSetX2 + einde.posX,
                                                offSetY2 + einde.posY,
                                                2,
                                                TFT_BLACK);
                }
            } 
        }
        
        if(Maps::turnOnGpsHeading){ // GPS heading aan
            Maps::mapTempSprite.setPivot(Maps::mapTileSize + Maps::navArrowPosition.posX,
                                     Maps::mapTileSize + Maps::navArrowPosition.posY);
            Maps::mapTempSprite.pushRotated(&mapSprite, 360 - mapHeading); // roteer de kaart op basis van GPS heading
        } else {
            Maps::mapTempSprite.setPivot(Maps::mapTileSize + Maps::navArrowPosition.posX,
                                     Maps::mapTileSize + Maps::navArrowPosition.posY);
            Maps::mapTempSprite.pushRotated(&mapSprite, 0);
        };
    }
}   

/// @brief Update de kaart door de oude tile informatie te wissen
void Maps::updateMap()
{
    Maps::oldMapTile = {};
}

/// @brief Centreert de kaart op gegeven GPS-coördinaten
/// @param lat Breedteligging
/// @param lon Lengtegraad
/// @note Deze functie stelt de huidige map tile coördinaten in op basis van de opgegeven GPS-locatie en schakelt de GPS-volgmodus in.
void Maps::centerOnGps(float lat, float lon)
{
    Maps::followGps = true;
    Maps::currentMapTile.tilex = Maps::lon2tilex(lon, Maps::currentMapTile.zoom);
    Maps::currentMapTile.tiley = Maps::lat2tiley(lat, Maps::currentMapTile.zoom);
    Maps::currentMapTile.lat = lat;
    Maps::currentMapTile.lon = lon;
}

void Maps::displayGpxRoute(std::vector<wayPoint> waypoints){
    _waypoints = waypoints;
} 