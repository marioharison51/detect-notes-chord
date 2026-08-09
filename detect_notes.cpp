#include <sndfile.h>
#include <fftw3.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>

// Fonction pour convertir une fréquence en note musicale
std::string freqToNote(double freq) {
    if (freq <= 0) return "Fréquence invalide";

    // Référence : La4 = 440 Hz
    static const std::vector<std::string> notes = {
        "Do", "Do#", "Ré", "Ré#", "Mi", "Fa", "Fa#", "Sol", "Sol#", "La", "La#", "Si"
    };

    // Calcul du nombre de demi-tons par rapport à La4
    int n = std::round(12 * std::log2(freq / 440.0));
    int octave = 4 + (n / 12);
    int noteIndex = (n % 12 + 12) % 12; // éviter les indices négatifs

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

    // Extraire uniquement le premier canal si multicanal
    std::vector<double> samples(sfinfo.frames);
    for (int i = 0; i < sfinfo.frames; ++i) {
        samples[i] = buffer[i * sfinfo.channels];
    }

    // --- Analyse FFT ---
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * sfinfo.frames);
    fftw_plan plan = fftw_plan_dft_r2c_1d(sfinfo.frames, samples.data(), out, FFTW_ESTIMATE);
    fftw_execute(plan);

    // Détecter la fréquence dominante
    double maxMag = 0.0;
    int maxIndex = 0;
    for (int i = 0; i < sfinfo.frames / 2; ++i) {
        double mag = std::sqrt(out[i][0]*out[i][0] + out[i][1]*out[i][1]);
        if (mag > maxMag) {
            maxMag = mag;
            maxIndex = i;
        }
    }

    double freqDominante = maxIndex * sfinfo.samplerate / sfinfo.frames;

    fftw_destroy_plan(plan);
    fftw_free(out);

    // Conversion en note musicale
    std::string note = freqToNote(freqDominante);

    return "Fréquence dominante : " + std::to_string(freqDominante) + " Hz (" + note + ")";
}
