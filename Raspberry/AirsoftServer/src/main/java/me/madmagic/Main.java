package me.madmagic;

import org.eclipse.paho.client.mqttv3.IMqttClient;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.json.JSONObject;

public class Main {

    public static void main(String[] args) throws Exception {
        IMqttClient mqtt = new MqttClient("tcp://127.0.0.1:1883", "server");

        MqttConnectOptions options = new MqttConnectOptions();
        options.setAutomaticReconnect(true);
        mqtt.connect(options);

        mqtt.subscribe("zigbee2mqtt/airsoftPointOnline", (topic, message) -> {
            String device = new JSONObject(message.toString()).getString("device");

            MqttMessage sMsg = new MqttMessage("{\"write\": {\"confirmed\": \"\"}}".getBytes());
            sMsg.setQos(1);
            sMsg.setRetained(false);

            mqtt.publish("zigbee2mqtt/" + device + "/set", sMsg);
            System.out.println("replied");
        });

//        mqtt.subscribe("zigbee2mqtt/bridge/event", (topic, msg) -> {
//            try {
//                JSONObject o = new JSONObject(msg.toString());
//                if (o.has("type") && o.getString("type").equals("device_joined")) { //initial join
//                    System.out.println("device initial join:");
//                    System.out.println(o.getJSONObject("data").get("friendly_name"));
//                }
//            } catch (Exception ignored) {}
//        });

        mqtt.subscribe("zigbee2mqtt/#", (topic, msg) -> {
            try {
                JSONObject o = new JSONObject(msg.toString());
                System.out.printf("%s: %s\n", topic, msg);

//                String deviceName;
//
//                //device started before z2m
//                if (o.has("linkquality")) {
//                    deviceName = topic.replaceFirst("zigbee2mqtt/", "");
//                }
//
//                System.out.println(deviceName);
            } catch (Exception ignored) {}
        });
    }
}