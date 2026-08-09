#include <sndfile.h>
#include <fftw3.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>

// Conversion fréquence → note musicale
std::string freqToNote(double freq) {
    if (freq <= 0) return "Fréquence invalide";

    static const std::vector<std::string> notes = {
        "Do", "Do#", "Ré", "Ré#", "Mi", "Fa", "Fa#", "Sol", "Sol#", "La", "La#", "Si"
    };

    int n = std::round(12 * std::log2(freq / 440.0));
    int octave = 4 + (n / 12);
    int noteIndex = (n % 12 + 12) % 12;

    return notes[noteIndex] + std::to_string(octave);
}

std::string detect_notes(const std::string& filename) {
    SF_INFO sfinfo;
    SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sfinfo);
    if (!file) {
        std::cerr << "Erreur ouverture fichier audio\n";
        return "Erreur ouverture fichier audio";
    }

    // Lire tous les canaux
    std::vector<double> buffer(sfinfo.frames * sfinfo.channels);
    sf_read_double(file, buffer.data(), sfinfo.frames * sfinfo.channels);
    sf_close(file);

    // Extraire uniquement le premier canal
    std::vector<double> samples(sfinfo.frames);
    for (int i = 0; i < sfinfo.frames; ++i) {
        samples[i] = buffer[i * sfinfo.channels];
    }

    // --- Segmentation en fenêtres ---
    int windowSize = 2048; // taille de fenêtre (≈ 46 ms à 44.1 kHz)
    int hopSize = 1024;    // pas de décalage (50 % overlap)
    std::string timeline = "Analyse par segments :\n";

    for (int start = 0; start + windowSize <= sfinfo.frames; start += hopSize) {
        // Extraire un segment
        std::vector<double> segment(windowSize);
        for (int i = 0; i < windowSize; i++) {
            segment[i] = samples[start + i];
        }

        // FFT sur le segment
        fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * windowSize);
        fftw_plan plan = fftw_plan_dft_r2c_1d(windowSize, segment.data(), out, FFTW_ESTIMATE);
        fftw_execute(plan);

        // Détecter fréquence dominante
        double maxMag = 0.0;
        int maxIndex = 0;
        for (int i = 0; i < windowSize / 2; ++i) {
            double mag = std::sqrt(out[i][0]*out[i][0] + out[i][1]*out[i][1]);
            if (mag > maxMag) {
                maxMag = mag;
                maxIndex = i;
            }
        }

        double freqDominante = maxIndex * sfinfo.samplerate / windowSize;
        std::string note = freqToNote(freqDominante);

        double timeSec = (double)start / sfinfo.samplerate;
        timeline += "Temps " + std::to_string(timeSec) + "s : " + note + " (" + std::to_string(freqDominante) + " Hz)\n";

        fftw_destroy_plan(plan);
        fftw_free(out);
    }

    return timeline;
}
