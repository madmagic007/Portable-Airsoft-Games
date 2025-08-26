#include <Arduino.h>
#include "painlessMesh.h"

#define MESH_PREFIX     "AirsoftMesh"
#define MESH_PASSWORD   "AirsoftPassword"
#define MESH_PORT       5555

Scheduler userScheduler;
painlessMesh mesh;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("MSG_FROM:%u:%s\n", from, msg.c_str());

  if (msg != "ping") return;
  mesh.sendSingle(from, "ack");
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("NEW_NODE:%u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.println("CHANGED_CONNECTION");
}

void setup() {
  Serial.begin(115200);

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onReceive(&receivedCallback);
  
}

void loop() {
  mesh.update();

  if (Serial.available()) {
    String read = Serial.readString();
    mesh.sendBroadcast("ping");
  }
}