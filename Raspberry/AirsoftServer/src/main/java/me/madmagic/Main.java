package me.madmagic;

import me.madmagic.mqtt.MQTTHandler;
import org.json.JSONObject;

public class Main {

    public static void main(String[] args) throws Exception {
        MQTTHandler.init();
        System.out.println("subsribing");
        MQTTHandler.subscribe("airsoftPoint", (device, payload) -> {
            System.out.println("received");
            JSONObject ack = new JSONObject().put("data", "");

            MQTTHandler.publish(device, ack);
        });
    }
}