package me.madmagic;

import org.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.URISyntaxException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.attribute.BasicFileAttributes;
import java.util.*;
import java.util.stream.Stream;

public class Main {
    public static void main(String[] args) throws Exception {
        if (args.length == 0) {
            System.out.println("Please specify a statistic");
            return;
        }
        String key = args[0];

        if (key.isEmpty()) return;

        File statsDir = new File(Main.getRelativePath("stats"));

        try (Stream<Path> files = Files.list(Path.of(statsDir.getAbsolutePath()))) {
            Optional<Path> latestFile = files
                    .filter(Files::isRegularFile)
                    .max(Comparator.comparingLong(p -> {
                        try {
                            return Files.readAttributes(p, BasicFileAttributes.class).creationTime().toMillis();
                        } catch (IOException e) {
                            return 0L;
                        }
                    }));

            latestFile.ifPresent(file -> {
                try (FileInputStream fis = new FileInputStream(file.toString())) {
                    JSONObject stats = new JSONObject(new org.json.JSONTokener(fis));

                    printSortedByStat(stats, key);
                } catch (Exception e) {
                    System.out.println("Failed to read stat json: " + e.getMessage());
                    return;
                }
            });
        }
    }

    private static String getRelativePath(String relative) throws URISyntaxException {
        Path jarDir = Paths.get(
                        Main.class.getProtectionDomain()
                                .getCodeSource()
                                .getLocation()
                                .toURI())
                .getParent();

        return jarDir.resolve(relative).normalize().toString();
    }

    private static void printSortedByStat(JSONObject json, String stat) {
        List<Map.Entry<String, Number>> entries = new ArrayList<>();

        for (String key : json.keySet()) {
            JSONObject value = json.getJSONObject(key);

            if (value.has(stat)) {
                Object val = value.get(stat);

                if (val instanceof Number) {
                    entries.add(Map.entry(key, (Number) val));
                }
            }
        }

        entries.sort((a, b) -> Double.compare(b.getValue().doubleValue(), a.getValue().doubleValue()));

        for (Map.Entry<String, Number> e : entries) {
            System.out.println(e.getKey() + ": " + e.getValue());
        }
    }
}