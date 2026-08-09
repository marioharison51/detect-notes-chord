#include <fftw3.h>
#include <cmath>

std::string detect_notes(const std::string& filename) {
    SF_INFO sfinfo;
    SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sfinfo);
    if (!file) {
        std::cerr << "Erreur ouverture fichier audio\n";
        return "Erreur ouverture fichier audio";
    }

    std::vector<double> samples(sfinfo.frames);
    sf_read_double(file, samples.data(), sfinfo.frames);
    sf_close(file);

    // --- Analyse FFT ---
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * sfinfo.frames);
    fftw_plan plan = fftw_plan_dft_r2c_1d(sfinfo.frames, samples.data(), out, FFTW_ESTIMATE);
    fftw_execute(plan);

    // Exemple : détecter la fréquence dominante
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

    return "Fréquence dominante : " + std::to_string(freqDominante) + " Hz";
}
