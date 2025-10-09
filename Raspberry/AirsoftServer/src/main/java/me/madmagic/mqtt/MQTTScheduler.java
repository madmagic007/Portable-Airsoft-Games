package me.madmagic.mqtt;

import java.util.concurrent.*;

public class MQTTScheduler {

    private static final BlockingQueue<MQTTMessageTask> queue = new LinkedBlockingQueue<>();
    private static final ScheduledExecutorService scheduler = Executors.newSingleThreadScheduledExecutor();
    private static final long delayMillis = 500;
    private static boolean running = false;

    public static void schedule(String device, String topic, String message) {
        queue.add(new MQTTMessageTask(device, topic, message));

        if (!running) {
            running = true;
            scheduleNext();
        }
    }

    private static void scheduleNext() {
        MQTTMessageTask task = queue.poll();

        if (task != null) {
            scheduler.execute(() -> {
                try {
                    MQTTHandler.publish(task);
                } finally {
                    scheduler.schedule(MQTTScheduler::scheduleNext, delayMillis, TimeUnit.MILLISECONDS);
                }
            });
        } else {
            running = false;
        }
    }

    public record MQTTMessageTask(String device, String topic, String message) {}
}
