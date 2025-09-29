package me.madmagic;

import me.madmagic.device.DeviceHandler;
import me.madmagic.gamemode.GamemodesHandler;
import me.madmagic.mqtt.MQTTHandler;

public class Main {

    public static void main(String[] args) throws Exception {
        String fileArg = args[0];
        System.out.println(fileArg);

        MQTTHandler.init();
        DeviceHandler.init();
        GamemodesHandler.init();
    }
}