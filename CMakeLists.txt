cmake_minimum_required(VERSION 3.10)
project(DetectNotesChord)

# Utiliser C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Trouver Qt6 pour l'interface graphique et multimédia
find_package(Qt6 COMPONENTS Widgets Multimedia REQUIRED)

# --- FFTW3 : détection manuelle ---
find_path(FFTW3_INCLUDE_DIR fftw3.h /usr/include /usr/local/include)
find_library(FFTW3_LIBRARY NAMES fftw3 PATHS /usr/lib /usr/local/lib)

if(FFTW3_INCLUDE_DIR AND FFTW3_LIBRARY)
    message(STATUS "✅ FFTW3 trouvé : ${FFTW3_LIBRARY}")
else()
    message(FATAL_ERROR "❌ FFTW3 non trouvé. Installe-le avec 'sudo apt install libfftw3-dev'")
endif()

# Inclure les bibliothèques audio
find_package(SndFile REQUIRED)

# Définir les fichiers sources
add_executable(DetectNotesChord
    main_qt.cpp
    detection_notes.cpp
)

# Lier les bibliothèques nécessaires
target_link_libraries(DetectNotesChord
    Qt6::Widgets
    Qt6::Multimedia
    ${FFTW3_LIBRARY}
    sndfile
)

# --- Configuration Android ---
if(ANDROID)
    message(STATUS "Compilation pour Android détectée")

    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    set(ANDROID_ABI "arm64-v8a")
    set(ANDROID_PLATFORM 33)

    set_property(TARGET DetectNotesChord PROPERTY ANDROID_PERMISSIONS
        "android.permission.READ_EXTERNAL_STORAGE"
        "android.permission.WRITE_EXTERNAL_STORAGE"
        "android.permission.RECORD_AUDIO"
    )

    set_property(TARGET DetectNotesChord PROPERTY ANDROID_APP_NAME "Detect Notes & Chords")
    set_property(TARGET DetectNotesChord PROPERTY ANDROID_APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.png")
endif()
