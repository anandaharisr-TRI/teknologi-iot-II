#include <painlessMesh.h>
#include <ArduinoJson.h>

#define MESH_PREFIX   "Tugas5_IOT"       
#define MESH_PASSWORD "123456789"        
#define MESH_PORT     5555               

Scheduler userScheduler;
painlessMesh mesh;

// Prototipe fungsi pengiriman pesan
void sendMessage();
// Jadwalkan pengiriman setiap 3 detik
Task taskSendMessage(TASK_SECOND * 3, TASK_FOREVER, &sendMessage);

void sendMessage() {
  // 1. Membaca tegangan dari sensor LDR di pin A0
  int nilaiLDR = analogRead(A0); 

  // 2. Mengemas data ke format string JSON
  StaticJsonDocument<200> doc;
  doc["tipe"] = "cahaya_node";
  doc["adc"] = nilaiLDR;

  String msg;
  serializeJson(doc, msg);
  
  // 3. Menyiarkan (broadcast) paket JSON ke seluruh jaringan mesh
  mesh.sendBroadcast(msg);
  
  // 4. Menampilkan log di Serial Monitor komputer Anda
  Serial.print("[KIRIM MESH] ");
  Serial.println(msg);
}

// Callback otomatis saat ada pesan masuk (Dibiarkan kosong karena Node 2 hanya pengirim)
void receivedCallback(uint32_t from, String &msg) {}

// Callback otomatis saat ada perangkat baru yang terhubung ke jaringan
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> Koneksi Baru Terdeteksi! Node ID: %u\n", nodeId);
}

// Callback otomatis saat ada perangkat yang terputus atau bergabung
void changedConnectionCallback() {
  Serial.println("--> Topologi rantai mesh telah diperbarui");
}

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi jaringan mesh
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  
  // Mendaftarkan fungsi-fungsi callback di atas agar berjalan otomatis
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  // Menyalakan timer pengiriman data berkala
  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
  
  Serial.println("Node 2 (Sensor LDR) Siap dan Berjalan...");
}

void loop() {
  // Fungsi utama mesh yang wajib dipanggil secara berulang tanpa henti (tanpa delay)
  mesh.update();
}