package com.detectnotes.chord;

public class AudioAnalyzer {
    private final String[] notes = {"Do", "Ré", "Mi", "Fa", "Sol", "La", "Si"};
    private final String[] chords = {"Do majeur", "Sol majeur", "La mineur", "Fa majeur"};

    public String analyze() {
        int index = (int) (System.currentTimeMillis() / 1000) % notes.length;
        int chordIndex = (int) (System.currentTimeMillis() / 1500) % chords.length;

        return "Note détectée : " + notes[index] + "4\n"
                + "Accord probable : " + chords[chordIndex];
    }
}
