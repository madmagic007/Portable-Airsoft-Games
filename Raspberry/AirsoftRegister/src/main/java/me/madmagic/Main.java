package me.madmagic;

import com.diozero.devices.MFRC522;
import com.diozero.devices.RgbLed;
import com.diozero.util.Hex;

import java.util.Scanner;

public class Main {

    private static Thread thread;

    public static void main(String[] args) throws Exception {
        MFRC522 scanner = new MFRC522(0, 22);
        Scanner inScanner = new Scanner(System.in);

        RgbLed led = new RgbLed(24, 23, 25);
        led.setValues(false, false,  true);

        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            thread.interrupt();
            led.close();
            inScanner.close();
            scanner.close();
        }));

        thread = new Thread(() -> {
            while (!Thread.currentThread().isInterrupted()) {
                if (!scanner.isNewCardPresent()) continue;

                MFRC522.UID uid = scanner.readCardSerial();
                if (uid == null) continue;

                new Thread(() -> {
                    led.setValues(false, true, false);
                    try {
                        Thread.sleep(200);
                    } catch (InterruptedException ignored) {
                    }
                    led.off();
                }).start();

                String uidStr = Hex.encodeHexString(uid.getUidBytes());

                System.out.printf("Nickname for (%s): ", uidStr);
                String nickName = inScanner.nextLine();

                System.out.printf("Team for (%s): ", uidStr);
                int team = inScanner.nextInt();
                inScanner.nextLine();

                System.out.println(nickName);
                System.out.println(team);


                led.setValues(false, false, true);
            }
        });
        thread.start();
    }
}