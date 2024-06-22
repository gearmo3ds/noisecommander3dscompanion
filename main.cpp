#include "renderwidget.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/img/icon.png"));

    // Register the resource file
    Q_INIT_RESOURCE(resources);

    a.setAttribute(Qt::AA_DisableHighDpiScaling);
//    a.setAttribute(Qt::AA_DisableAntialiasing);

    RenderWidget pixelWidget;

    pixelWidget.resize(700, 600);
//    pixelWidget.showMaximized();
//    pixelWidget.showFullScreen();
    pixelWidget.show();

    int result = a.exec();
    return result;
}
