package me.madmagic.mqtt;

import org.eclipse.paho.client.mqttv3.IMqttClient;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttMessage;
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

    public static void subscribe(String endpoint, BiConsumer<String, JSONObject> consumer) throws Exception {
        mqtt.subscribe("airsoft/" + endpoint, (topic, msg) -> {
            JSONObject o = new JSONObject(msg.toString());
            consumer.accept(o.getString("device"), o);
        });
    }

    public static void publish(String device, JSONObject post) {
        MqttMessage msg = new MqttMessage(new JSONObject().put("write", post).toString().getBytes());

        try {
            mqtt.publish("airsoft/" + device + "/set", msg);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
