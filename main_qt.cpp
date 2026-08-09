#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QTextEdit>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMediaPlayer>
#include <QString>
#include <string>

// Déclaration de ta fonction d’analyse
std::string detect_notes(const std::string& filename);

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Detect Notes & Chords");

    QVBoxLayout *layout = new QVBoxLayout(&window);

    QPushButton *buttonImport = new QPushButton("Importer un fichier audio");
    QPushButton *buttonPlay = new QPushButton("Lire");
    QTextEdit *output = new QTextEdit();
    output->setReadOnly(true);

    // Zone graphique pour la tablature
    QGraphicsView *tabView = new QGraphicsView();
    QGraphicsScene *scene = new QGraphicsScene();
    tabView->setScene(scene);

    layout->addWidget(buttonImport);
    layout->addWidget(buttonPlay);
    layout->addWidget(output);
    layout->addWidget(tabView);

    QMediaPlayer *player = new QMediaPlayer;
    QString fileName;

    QObject::connect(buttonImport, &QPushButton::clicked, [&]() {
        fileName = QFileDialog::getOpenFileName(&window, "Choisir un fichier audio", "", "Audio Files (*.wav *.mp3)");
        if (!fileName.isEmpty()) {
            std::string result = detect_notes(fileName.toStdString());
            output->setPlainText(QString::fromStdString(result));

            // Exemple simplifié : afficher les notes sur une ligne horizontale
            scene->clear();
            int x = 0;
            for (const auto &line : QString::fromStdString(result).split("\n")) {
                if (line.contains("Temps")) {
                    scene->addText(line)->setPos(x, 0);
                    x += 120; // espacement horizontal
                }
            }
        }
    });

    QObject::connect(buttonPlay, &QPushButton::clicked, [&]() {
        if (!fileName.isEmpty()) {
            player->setSource(QUrl::fromLocalFile(fileName));
            player->play();
        }
    });

    window.show();
    return app.exec();
}
