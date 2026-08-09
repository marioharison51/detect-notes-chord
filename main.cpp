#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sndfile.h>
#include <fftw3.h>
#include <algorithm>

#define BUFFER_SIZE 4096
#define SAMPLE_RATE 44100

// Table des notes (12 demi-tons)
struct Note {
    double freq;
    std::string name;
};

std::vector<Note> notesTable = {
    {261.63, "C"}, {277.18, "C#"}, {293.66, "D"}, {311.13, "D#"},
    {329.63, "E"}, {349.23, "F"}, {369.99, "F#"}, {392.00, "G"},
    {415.30, "G#"}, {440.00, "A"}, {466.16, "A#"}, {493.88, "B"}
};

// Associer une fréquence à une note
std::string map_to_note(double freq) {
    for (auto& n : notesTable) {
        if (fabs(freq - n.freq) < 10.0) return n.name;
    }
    return "Note inconnue";
}

// Structure d’accord
struct Chord {
    std::string name;
    std::vector<std::string> notes;
};

// Table des accords (majeurs, mineurs, 7, m7, sus4, 6, m6, dim, aug)
std::vector<Chord> chordTable = {
    {"C majeur", {"C", "E", "G"}}, {"C mineur", {"C", "D#", "G"}},
    {"C7", {"C", "E", "G", "A#"}}, {"Cm7", {"C", "D#", "G", "A#"}},
    {"Csus4", {"C", "F", "G"}}, {"C6", {"C", "E", "G", "A"}},
    {"Cm6", {"C", "D#", "G", "A"}}, {"Cdim", {"C", "D#", "F#"}},
    {"Caug", {"C", "E", "G#"}},

    {"D majeur", {"D", "F#", "A"}}, {"D mineur", {"D", "F", "A"}},
    {"D7", {"D", "F#", "A", "C"}}, {"Dm7", {"D", "F", "A", "C"}},
    {"Dsus4", {"D", "G", "A"}}, {"D6", {"D", "F#", "A", "B"}},
    {"Dm6", {"D", "F", "A", "B"}}, {"Ddim", {"D", "F", "G#"}},
    {"Daug", {"D", "F#", "A#"}},

    {"E majeur", {"E", "G#", "B"}}, {"E mineur", {"E", "G", "B"}},
    {"E7", {"E", "G#", "B", "D"}}, {"Em7", {"E", "G", "B", "D"}},
    {"Esus4", {"E", "A", "B"}}, {"E6", {"E", "G#", "B", "C#"}},
    {"Em6", {"E", "G", "B", "C#"}}, {"Edim", {"E", "G", "A#"}},
    {"Eaug", {"E", "G#", "C"}},

    {"F majeur", {"F", "A", "C"}}, {"F mineur", {"F", "G#", "C"}},
    {"F7", {"F", "A", "C", "D#"}}, {"Fm7", {"F", "G#", "C", "D#"}},
    {"Fsus4", {"F", "A#", "C"}}, {"F6", {"F", "A", "C", "D"}},
    {"Fm6", {"F", "G#", "C", "D"}}, {"Fdim", {"F", "G#", "B"}},
    {"Faug", {"F", "A", "C#"}},

    {"G majeur", {"G", "B", "D"}}, {"G mineur", {"G", "A#", "D"}},
    {"G7", {"G", "B", "D", "F"}}, {"Gm7", {"G", "A#", "D", "F"}},
    {"Gsus4", {"G", "C", "D"}}, {"G6", {"G", "B", "D", "E"}},
    {"Gm6", {"G", "A#", "D", "E"}}, {"Gdim", {"G", "A#", "C#"}},
    {"Gaug", {"G", "B", "D#"}},

    {"A majeur", {"A", "C#", "E"}}, {"A mineur", {"A", "C", "E"}},
    {"A7", {"A", "C#", "E", "G"}}, {"Am7", {"A", "C", "E", "G"}},
    {"Asus4", {"A", "D", "E"}}, {"A6", {"A", "C#", "E", "F#"}},
    {"Am6", {"A", "C", "E", "F#"}}, {"Adim", {"A", "C", "D#"}},
    {"Aaug", {"A", "C#", "F"}},

    {"B majeur", {"B", "D#", "F#"}}, {"B mineur", {"B", "D", "F#"}},
    {"B7", {"B", "D#", "F#", "A"}}, {"Bm7", {"B", "D", "F#", "A"}},
    {"Bsus4", {"B", "E", "F#"}}, {"B6", {"B", "D#", "F#", "G#"}},
    {"Bm6", {"B", "D", "F#", "G#"}}, {"Bdim", {"B", "D", "F"}},
    {"Baug", {"B", "D#", "G"}}
};

// Fonction de détection
std::string detect_chord(const std::vector<std::string>& notes) {
    for (auto& chord : chordTable) {
        bool match = true;
        for (auto& n : chord.notes) {
            if (std::find(notes.begin(), notes.end(), n) == notes.end()) {
                match = false;
                break;
            }
        }
        if (match) return chord.name;
    }
    return "Accord inconnu";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./detect fichier_audio.wav\n";
        return 1;
    }

    // Lecture audio
    SF_INFO sfinfo;
    SNDFILE* infile = sf_open(argv[1], SFM_READ, &sfinfo);
    if (!infile) {
        std::cerr << "Erreur ouverture fichier audio\n";
        return 1;
    }

    std::vector<double> buffer(BUFFER_SIZE);
    std::vector<double> samples(sfinfo.frames);
    sf_read_double(infile, samples.data(), sfinfo.frames);
    sf_close(infile);

    // FFT
    fftw_complex* out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * BUFFER_SIZE);
    fftw_plan plan = fftw_plan_dft_r2c_1d(BUFFER_SIZE, buffer.data(), out, FFTW_ESTIMATE);

    // Analyse par blocs
    for (size_t pos = 0; pos + BUFFER_SIZE < samples.size(); pos += BUFFER_SIZE) {
        for (int i = 0; i < BUFFER_SIZE; i++) buffer[i] = samples[pos + i];
        fftw_execute(plan);

        std::vector<std::string> notes_detected;
        for (int i = 0; i < BUFFER_SIZE/2; i++) {
            double amp = sqrt(out[i][0]*out[i][0] + out[i][1]*out[i][1]);
            if (amp > 1000.0) {
                double freq = (double)i * SAMPLE_RATE / BUFFER_SIZE;
                std::string note = map_to_note(freq);
                if (note != "Note inconnue") notes_detected.push_back(note);
            }
        }

        if (!notes_detected.empty()) {
            std::cout << "Notes détectées : ";
            for (auto& n : notes_detected) std::cout << n << " ";
            std::cout << "\n";

            std::string chord = detect_chord(notes_detected);
            std::cout << "Accord détecté : " << chord << "\n";
        }
    }

    fftw_destroy_plan(plan);
    fftw_free(out);
    return 0;
}
