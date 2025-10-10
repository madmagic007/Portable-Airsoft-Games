package me.madmagic.mqtt;

import java.util.concurrent.*;

public class MQTTScheduler {

    private static final BlockingQueue<MQTTMessageTask> queue = new LinkedBlockingQueue<>();
    private static final ScheduledExecutorService scheduler = Executors.newSingleThreadScheduledExecutor();
    private static final long delayMillis = 100;
    private static boolean running = false;
    private static boolean stopping = false;

    public static synchronized void schedule(String device, String topic, String message) {
        queue.add(new MQTTMessageTask(device, topic, message));
        startIfNotRunning();
    }

    private static synchronized void startIfNotRunning() {
        if (running) return;

        running = true;
        scheduleNext();
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

            if (stopping) {
                System.out.println("Message queue is empty, stopping program...");
                MQTTHandler.stop();
                System.exit(0);
            }
        }
    }

    public static void stop() {
        stopping = true;
    }

    public record MQTTMessageTask(String device, String topic, String message) {}
}
