

/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt
 *
 * Copyright (c) 2023 mobizt
 *
 */

// This example shows how to use TTGO T-A7670 (ESP32 with SIMCom SIMA7670) and TinyGSMClient This example to connect to Google API.

// To allow TinyGSM library integration, the following macro should be defined in src/ESP_Google_Sheet_Client_FS_Config.h.
//  #define TINY_GSM_MODEM_SIM7600

#define TINY_GSM_MODEM_SIM7600 // SIMA7670 Compatible with SIM7600 AT instructions

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[] = "YourAPN";
const char gprsUser[] = "";
const char gprsPass[] = "";

#define UART_BAUD 115200
#define PIN_DTR 25
#define PIN_TX 26
#define PIN_RX 27
#define PWR_PIN 4
#define BAT_ADC 35
#define BAT_EN 12
#define PIN_RI 33
#define PIN_DTR 25
#define RESET 5

#define SD_MISO 2
#define SD_MOSI 15
#define SD_SCLK 14
#define SD_CS 13

#include <ESP_Google_Sheet_Client.h>

#include <TinyGsmClient.h>

// For how to create Service Account and how to use the library, go to https://github.com/mobizt/ESP-Google-Sheet-Client

#define PROJECT_ID "PROJECT_ID"

// Service Account's client email
#define CLIENT_EMAIL "CLIENT_EMAIL"

// Service Account's private key
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----XXXXXXXXXXXX-----END PRIVATE KEY-----\n";

// Set serial for debug console
#define SerialMon Serial

// Set serial for AT commands (to the module)
#define SerialAT Serial1

TinyGsm modem(SerialAT);

TinyGsmClient gsm_client(modem);

bool gsheetSetupReady = false;

bool taskComplete = false;

void setupGsheet();

void tokenStatusCallback(TokenInfo info);

void initModem()
{

    if (modem.isGprsConnected())
    {
        modem.gprsDisconnect();
        SerialMon.println(F("GPRS disconnected"));
    }

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    DBG("Initializing modem...");
    if (!modem.init())
    {
        DBG("Failed to restart modem, delaying 10s and retrying");
        return;
    }

    /*
    2 Automatic
    13 GSM Only
    14 WCDMA Only
    38 LTE Only
    */
    modem.setNetworkMode(38);
    if (modem.waitResponse(10000L) != 1)
    {
        DBG(" setNetworkMode faill");
        return;
    }
}

void setup()
{

    SerialMon.begin(115200);

    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

    String name = modem.getModemName();
    DBG("Modem Name:", name);

    String modemInfo = modem.getModemInfo();
    DBG("Modem Info:", modemInfo);

    initModem();

    GSheet.printf("ESP Google Sheet Client v%s\n\n", ESP_GOOGLE_SHEET_CLIENT_VERSION);

    GSheet.setGSMClient(&gsm_client, &modem, GSM_PIN, apn, gprsUser, gprsPass);
}

void loop()
{
    if (!gsheetSetupReady)
        setupGsheet();

    bool ready = GSheet.ready();

    if (ready && !taskComplete)
    {

        // Google sheet code here

        taskComplete = true;
    }
}

void setupGsheet()
{
    // Set the callback for Google API access token generation status (for debug only)
    GSheet.setTokenCallback(tokenStatusCallback);

    // Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
    GSheet.setPrerefreshSeconds(10 * 60);

    // Begin the access token generation for Google API authentication
    GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

    gsheetSetupReady = true;
}

void tokenStatusCallback(TokenInfo info)
{
    if (info.status == token_status_error)
    {
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
        GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
    else
    {
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    }
}