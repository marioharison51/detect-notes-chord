#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sndfile.h>
#include <fftw3.h>

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

// Détection d’accords simples
std::string detect_chord(const std::vector<std::string>& notes) {
    if (notes.size() >= 3) {
        if (notes[0] == "C" && notes[1] == "E" && notes[2] == "G") return "C majeur";
        if (notes[0] == "A" && notes[1] == "C" && notes[2] == "E") return "A mineur";
        if (notes[0] == "G" && notes[1] == "B" && notes[2] == "D") return "G majeur";
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
