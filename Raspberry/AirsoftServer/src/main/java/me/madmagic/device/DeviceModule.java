package me.madmagic.device;

public class DeviceModule {
    public static final int SCANNER = 0;
    public static final int LED_DRIVER = 1;
    public static final int BUZZER = 2;
    public static final int BATTERY_MONITOR = 3;
    public static final int GENERIC_LED = 4;

    public static final int[] CAPTURE_POINT = new int[] {SCANNER, LED_DRIVER, BUZZER, BATTERY_MONITOR};
    public static final int[] GENERIC = new int[] {SCANNER, GENERIC_LED};
}
