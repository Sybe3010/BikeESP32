#pragma once
#include "BLEServer.h"

static BLEUUID deviceControllServiceUUID("6426b11d-a756-42ce-9e7b-aa2b7e6a43b5");
static BLEUUID fileTransferServiceUUID("02871e9a-1386-45a7-9c24-e8da07aadbe4");
static BLEUUID deviceInfoServiceUUID("d638977b-13c6-417b-9009-14fc73a64e54");
static BLEUUID accountInfoServiceUUID("d8b4747f-b646-4e60-990a-e1c912500de9");

//device control service
static BLECharacteristic connectionStateChar("a2d73008-51b2-4f5d-bba0-c19f666ef65c", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE);
static BLECharacteristic bikeCompStateChar("986a1316-8116-42d2-bd7f-26aca7f20ade", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
//device contol descriptors
static BLEDescriptor connectionStateDisc(BLEUUID((uint16_t)0x2902));
static BLEDescriptor bikeCompStateDisc(BLEUUID((uint16_t)0x2902));

//file transfer service
static BLECharacteristic fileStateChar("23b80b43-cfa2-46b8-a75e-eed5cc844b8b", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
static BLECharacteristic fileNameChar("15f0536b-9bf8-41b7-b3bb-8ba4898af0d4", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
static BLECharacteristic transferControlChar("839f4695-7ffe-4400-933f-41efd970bc32", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
static BLECharacteristic transferProgressChar("cad48b51-41ef-401e-bfdb-4f63268993d2", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
//file transfer discriptors
static BLEDescriptor fileStateDisc(BLEUUID((uint16_t)0x2902));
static BLEDescriptor fileNameDisc(BLEUUID((uint16_t)0x2902));
static BLEDescriptor transferControlDisc(BLEUUID((uint16_t)0x2902));
static BLEDescriptor transferProgressDisc(BLEUUID((uint16_t)0x2902));

//device info service
static BLECharacteristic sdCardChar("56a2e27e-340b-4616-a1a8-c339febce6d1");
//device info descriptor
static BLEDescriptor sdCardDisc(BLEUUID((uint16_t)0x2902));

//account info service
static BLECharacteristic dataRequestTypeChar("6a090c3e-bb5e-4ebe-a7e7-6ca1708f6747", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
static BLECharacteristic totalKmsChar("67663da4-7ae5-4a34-9608-6fb5635fb784", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
static BLECharacteristic totalRidesChar("e9e552ef-2112-41c6-9a45-a6a71378fa0f", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
static BLECharacteristic lastRideChar("11f227d6-afcc-4dfc-a607-692520a3717c", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
//account info descriptor
static BLEDescriptor dataRequestTypeDisc(BLEUUID((uint16_t)0x2902));
static BLEDescriptor totalKmsDisc(BLEUUID((uint16_t)0x2902));
static BLEDescriptor totalRidesDisc(BLEUUID((uint16_t)0x2902));
static BLEDescriptor lastRideDisc(BLEUUID((uint16_t)0x2902));

enum ConnectionState{
    IDLE, //0
    BLE_CONNECTED, //1
    SYNC_REQUESTED, //2
    WIFI_STARTING, //3
    WIFI_READY, //4
    TRANSFERRING, //5
    TRANSFER_COMPLETE, //6
    ERROR //7
};

enum FileState{
    NO_FILESTATE,
    REQUEST_UPLOAD,
    REQUEST_DOWNLOAD,
    SENDING_METADATA,
    RECIEVING_METADATA,
    TRANSFERRING_FILE,
    VERIFYING,
    SUCCESS,
    FAILED,
    CANCELLED
};

enum BikeCompState{
    HOME,
    IN_ACTIVITY,
    IN_END_SCREEN,
    SLEEP,
    ERROR_BIKECOMP
};

enum AccountInfoState{
    COMPLETED,
    TRANSFERRING_ACCOUNT,
    REQUEST_FROM_PHONE,
    REQUEST_FROM_BIKEESP,
    NONE,
    ERROR_ACCOUNTINFO
};