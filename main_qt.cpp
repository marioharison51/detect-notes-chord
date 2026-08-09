#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include "detect_notes.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Detect Notes & Chords");

    QPushButton button("Choisir un fichier audio", &window);
    QTextEdit output(&window);

    QObject::connect(&button, &QPushButton::clicked, [&]() {
        QString file = QFileDialog::getOpenFileName(&window, "Ouvrir un fichier audio", "", "Audio Files (*.wav *.mp3)");
        if (!file.isEmpty()) {
            std::string result = detect_notes(file.toStdString());
            output.setText(QString::fromStdString(result));
        }
    });

    QVBoxLayout layout(&window);
    layout.addWidget(&button);
    layout.addWidget(&output);
    window.setLayout(&layout);

    window.show();
    return app.exec();
}
