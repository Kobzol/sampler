#include <QApplication>

#include "components/MainWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    // create services
    QSettings settings("cz.beranek", "sampler");

    SamplerManager samplerManager;
    SettingsManager settingsManager(settings);

    MainWindow window(samplerManager, settingsManager);
    window.show();

    return app.exec();
}
