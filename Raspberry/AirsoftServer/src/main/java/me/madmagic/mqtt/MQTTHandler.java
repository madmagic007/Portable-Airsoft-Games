package me.madmagic.mqtt;

import org.eclipse.paho.client.mqttv3.*;
import org.json.JSONObject;

import java.util.function.BiConsumer;

public class MQTTHandler {

    private static IMqttClient mqtt;

    public static void init() throws Exception {
        mqtt = new MqttClient("tcp://127.0.0.1:1883", "server");

        MqttConnectOptions options = new MqttConnectOptions();
        options.setAutomaticReconnect(true);
        mqtt.connect(options);
    }

    public static void stop() {
        try {
            mqtt.disconnect();
            mqtt.close();
        } catch (MqttException e) {
            System.out.println("Failed to stop mqtt: " + e.getMessage());
        }
    }

    public static void subscribe(String endpoint, BiConsumer<String, JSONObject> consumer) {
        try {
            mqtt.subscribe("airsoft/" + endpoint, (topic, msg) -> {
                JSONObject o = new JSONObject(msg.toString());
                consumer.accept(o.getString("device"), o);
            });
        } catch (Exception ignored) {}
    }

    public static void publish(String device, JSONObject post) {
        MqttMessage msg = new MqttMessage(new JSONObject().put("write", post).toString().getBytes());

        try {
            mqtt.publish("airsoft/" + device + "/set", msg);
            Thread.sleep(200);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void publish(String device, String topic, String value) {
        publish(device, new JSONObject().put(topic, value));
    }

    public static void publish(String device, String topic, Object value) {
        publish(device, topic, value.toString());
    }
}
