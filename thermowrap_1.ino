#include <SoftwareSerial.h>

// HC-05 TX → Arduino pin 10, HC-05 RX → Arduino pin 11
SoftwareSerial BT(10, 11);

// Pin Declarations
const int BUTTON_PIN = 2;   // Button input (with 1k pull-down)
const int RELAY_PIN  = 8;   // Relay coil control
const int DEPRE_PIN  = 4;   // Depressure pin
bool Pump_Active     = false; // Pump default not filled

void setup() {
    pinMode(BUTTON_PIN, INPUT);
    pinMode(RELAY_PIN,  OUTPUT);
    pinMode(DEPRE_PIN,  OUTPUT);

    Serial.begin(9600);   // USB Serial Monitor
    BT.begin(9600);       // HC-05 Bluetooth
}

void loop() {
    // Data coming FROM the phone/device → print to Serial Monitor
    if (BT.available()) {
        char val = BT.read();
        Serial.print("Received: ");
        Serial.println(val);

        if (val == 'A' && Pump_Active == false) {
            // --- Inflate: pump ON, valve OFF ---
            digitalWrite(RELAY_PIN, HIGH);
            digitalWrite(DEPRE_PIN, LOW);
            delay(5000);
            digitalWrite(RELAY_PIN, LOW);
            Pump_Active = true;
        }
        else if (val == 'A' && Pump_Active == true) {
            // --- Massage cycle: 10× pulse, then vent —

            digitalWrite(RELAY_PIN, LOW);
            digitalWrite(DEPRE_PIN, HIGH);
            delay(6000);
            for (int i = 0; i < 10; i++) {
                digitalWrite(RELAY_PIN, HIGH);
                digitalWrite(DEPRE_PIN, LOW);
                delay(2000);
                digitalWrite(RELAY_PIN, LOW);
                digitalWrite(DEPRE_PIN, HIGH);
                delay(2000);
            }
            digitalWrite(RELAY_PIN, LOW);
            digitalWrite(DEPRE_PIN, HIGH);
            delay(13000);
            digitalWrite(DEPRE_PIN, LOW);
            Pump_Active = false;
        }
        else if (val == 'B' && Pump_Active == true) {

            // --- Emergency deflate —

            digitalWrite(RELAY_PIN, LOW);
            digitalWrite(DEPRE_PIN, HIGH);
            delay(10000);
            Pump_Active = false;
        }
        else if (val == 'C') {

            // --- Continuous heat mode —

            digitalWrite(RELAY_PIN, HIGH);
            digitalWrite(DEPRE_PIN, HIGH);
            /* for (int i = 0; i < 10; i++) {
            digitalWrite(RELAY_PIN, HIGH);
            delay(500);                               // pump on and off 10 times
            digitalWrite(RELAY_PIN, LOW);
            delay(500);}   */
        }

        else if (val == 'D')
        {
        digitalWrite(RELAY_PIN, LOW);                      // Relay OFF
        digitalWrite(DEPRE_PIN, LOW);                     // Valve OFF

        }
        else {
            Serial.println("Unknown command");
        }
    }

    // Forward Serial Monitor input to HC-05
    if (Serial.available()) {
        char cmd = Serial.read();
        BT.write(cmd);
    }
}

