# GPX LIBRARY

De code in deze libary zorgt voor het lezen en schrijven van de gpx bestanden in het project. Het lezen is voor het uitlezen van gpx bestanden, het schrijven wordt gebruikt om de activiteiten op te slaan.

## Reading
Het lezen van de gpx bestand gebeurt met behulp van de [tinyXml2 Library](https://github.com/leethomason/tinyxml2.git). Bij het uitlezen wordt alle data in een c++ vector opgeslagen en nadien gebruikt. Standaard wordt het hoogteprofiel uit de track gehaalt.

#### hoogteprofiel
Het hoogteprofiel wordt uit de track gehaald. Om de juiste altitude data uit de track te halen moet deze data wel beschikbaar zijn in het bestand. Anders zal het hoogteprofiel niet juist uitgelezen worden. 

#### Climbs
Er kunnen climbs uit de tracks gehaald worden. Hiervoor wordt de data uit het hoogteprofiel gebruikt. Dus als deze niet juist is kunnen er ook geen juiste climbs uit de track gehaald worden. Deze hebben standaard volgende filtering.
* langer dan 100m, gemiddeld 3% stijging.
* korter dan 100m, gemiddeld 5% stijging.
* De climb mag 5 meter dalen of -1%. Is dit groter dan wordt de climb afgesloten en wordt er een nieuwe gemaakt.

## Acknowledgements
 - [IceNav-v3 by jgauchia](https://github.com/jgauchia/IceNav-v3/tree/devel/lib/gpx/src)


