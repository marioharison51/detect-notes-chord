#include <sndfile.h>
#include <iostream>
#include <vector>

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

    // Ici tu peux appliquer ton analyse FFT
    return "Analyse terminée : " + filename;
}
