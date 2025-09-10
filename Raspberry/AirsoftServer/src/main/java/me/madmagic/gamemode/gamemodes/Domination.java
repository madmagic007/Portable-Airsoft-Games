package me.madmagic.gamemodes.modes;

import me.madmagic.gamemodes.GamemodeBase;

public class Domination extends GamemodeBase {

    public Domination() {

    }

    @Override
    public void start() {
        
    }

    @Override
    public void onTagScanned(String device, String value) {
        System.out.println(value);
    }
}
