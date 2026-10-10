#include <painlessMesh.h>
#include <ArduinoJson.h>

#define MESH_PREFIX   "Tugas5_IOT"
#define MESH_PASSWORD "123456789"
#define MESH_PORT     5555

const int ledPin = 12; // Pin D6 (GPIO 12)
const char* nodeName = "Node-3";

Scheduler userScheduler;
painlessMesh mesh;

// Variabel memori penyimpan data terakhir dari sensor
float currentSuhu = 0;
int currentAdc = 500;

void evaluateRuleEngine() {
  // Aturan: JIKA suhu > 31.0 ATAU adc < 300 (gelap), nyalakan LED
  if (currentSuhu > 31.0 || currentAdc < 300) {
    digitalWrite(ledPin, HIGH);
    Serial.println("[RULE ENGINE] Kondisi Kritis! LED Menyala (HIGH)");
  } else {
    digitalWrite(ledPin, LOW);
    Serial.println("[RULE ENGINE] Kondisi Normal. LED Padam (LOW)");
  }
}

void receivedCallback(uint32_t from, String &msg) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, msg);

  if (!error) {
    const char* tipe = doc["tipe"];
    if (strcmp(tipe, "suhu_node") == 0) {
      currentSuhu = doc["suhu"];
      Serial.printf("[TERIMA SUHU] %.2f °C\n", currentSuhu);
    } else if (strcmp(tipe, "cahaya_node") == 0) {
      currentAdc = doc["adc"];
      Serial.printf("[TERIMA CAHAYA] ADC: %d\n", currentAdc);
    }
    evaluateRuleEngine();
  }
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> Koneksi Baru Terdeteksi! Node ID: %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.println("--> Topologi rantai mesh telah diperbarui (Self-Healing aktif)");
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  mesh.setDebugMsgTypes(ERROR | STARTUP);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  Serial.println("Hub Aktuator Node-3 Siap Mengawasi Jaringan Mesh...");
}

void loop() {
  mesh.update();
}