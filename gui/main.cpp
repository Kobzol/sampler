#include <QApplication>
#include <memory>

#include "components/MainWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    // create services
    auto samplerManager = std::make_unique<SamplerManager>();

    MainWindow window(*samplerManager);
    window.show();

    return app.exec();
}
