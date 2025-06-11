// ================= ESP32-WROOM =================
#define RELAY_A 18  
#define RELAY_B 19
#define RELAY_C 14
#define RELAY_D 12
#define RELAY_E 26
#define RELAY_F 27

#define LED_SAFE    33
#define LED_OBJECT  32
#define LED_CAR     25

#define RX_PIN 13
HardwareSerial mySerial(1);

bool current_relay_state[6] = {false, false, false, false, false, false};  // A-F
bool newRelayState[6] = {false, false, false, false, false, false};
bool hasNewRelay = false;
String inputString = "";

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, RX_PIN, -1); // RX only

  pinMode(RELAY_A, OUTPUT); pinMode(RELAY_B, OUTPUT);
  pinMode(RELAY_C, OUTPUT); pinMode(RELAY_D, OUTPUT);
  pinMode(RELAY_E, OUTPUT); pinMode(RELAY_F, OUTPUT);

  pinMode(LED_SAFE, OUTPUT);
  pinMode(LED_OBJECT, OUTPUT);
  pinMode(LED_CAR, OUTPUT);

  allRelaysOff();
  allLEDsOff();

  Serial.println("ESP32-WROOM đã sẵn sàng, chờ tín hiệu từ ESP32-CAM...");
}

void loop() {
  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == '\n') {
      inputString.trim();
      inputString.toLowerCase();

      Serial.print("Tín hiệu nhận được: ");
      Serial.println(inputString);

      if (inputString == "an_toan") {
        allRelaysOff();
        allLEDsOff();
        digitalWrite(LED_SAFE, HIGH);
        memset(current_relay_state, 0, sizeof(current_relay_state));
        Serial.println("→ Trạng thái: AN_TOAN - Bật tất cả đèn");
      }
      else if (inputString == "vat_can") {
        digitalWrite(LED_OBJECT, HIGH);
        digitalWrite(LED_SAFE, LOW);
        digitalWrite(LED_CAR, LOW);
        Serial.println("→ Trạng thái: VẬT CẢN ");
      }
      else if (inputString.startsWith("oto-")) {
        digitalWrite(LED_CAR, HIGH);
        digitalWrite(LED_SAFE, LOW);
        digitalWrite(LED_OBJECT, LOW);

        char region = inputString.charAt(4);
        int idx = region - 'a';
        if (idx >= 0 && idx < 6) {
          newRelayState[idx] = true;
          hasNewRelay = true;
          Serial.print("→ Ghi nhận có xe tại vùng: ");
          Serial.println(region);
        }
      }
      else if (inputString == "end") {
        if (hasNewRelay) {
          Serial.println("→ Cập nhật vùng có xe:");
          for (int i = 0; i < 6; i++) {
            if (newRelayState[i]) {
              if (!current_relay_state[i]) {
                digitalWrite(relayPin(i), LOW);
                current_relay_state[i] = true;
                Serial.print("  - Tắt đèn vùng ");
                Serial.println((char)('A' + i));
              }
            } else {
              if (current_relay_state[i]) {
                digitalWrite(relayPin(i), HIGH);
                current_relay_state[i] = false;
                Serial.print("  -Bật đèn vùng ");
                Serial.println((char)('A' + i));
              }
            }
          }
        } else {
          Serial.println("→ Không phát hiện xe → Bật lại đèn");
          allRelaysOff();
          memset(current_relay_state, 0, sizeof(current_relay_state));
        }

        memset(newRelayState, 0, sizeof(newRelayState));
        hasNewRelay = false;
      }

      inputString = "";
    } else {
      inputString += c;
    }
  }
}

void allRelaysOff() {
  for (int i = 0; i < 6; i++) {
    digitalWrite(relayPin(i), HIGH);
  }
}

void allLEDsOff() {
  digitalWrite(LED_SAFE, LOW);
  digitalWrite(LED_OBJECT, LOW);
  digitalWrite(LED_CAR, LOW);
}

int relayPin(int idx) {
  switch (idx) {
    case 0: return RELAY_A;
    case 1: return RELAY_B;
    case 2: return RELAY_C;
    case 3: return RELAY_D;
    case 4: return RELAY_E;
    case 5: return RELAY_F;
    default: return -1;
  }
}
