#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <cstdint>
#include <vector>
#include <QMenuBar>
#include "networklistener.h"
#include <QPushButton>

//extern uint8_t tileSize;

class RenderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RenderWidget(QWidget *parent = nullptr);
    QColor generateRandomColor();

    void closeEvent(QCloseEvent *event) override;

    std::vector <uint16_t> topPixels;
    std::vector <uint16_t> botPixels;
    std::vector <uint16_t>* pointerPixels = nullptr;
    uint16_t position = 0;

    QImage bgImage;
    QImage advertImage;
    NetworkListener networkListener;

    void clearScreen();
    QString displayText = "";
    QPushButton btnConnect;

    bool eventFilter(QObject *obj, QEvent *event);
    void mouseMoveEvent(QMouseEvent* event) override;

public slots:
    void handleDataReceived(const QByteArray &data);
    void onConnect();
    void onConnectionSucceeded();
    void onConnectionLost();
    void onResized();
    void showButton();
    void onToggleMouseVisibility();

signals:
    void signalConnect(QString ipAddress, QString port);
    void resized();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override
    {
        // Emit the resized signal when the widget is resized
        QWidget::resizeEvent(event);
        emit resized();
    }

private:
    void onTouchEvent();
    uint32_t kHeld=0;
    uint16_t touchX=0, touchY=0;
    bool showMouse = false;
};

inline void RenderWidget::onToggleMouseVisibility()
{
    showMouse = !showMouse;
    update();
}

#define BIT(n) (1U << (n))

/// Key values.
enum
{
    KEY_A       = BIT(0),       ///< A
    KEY_B       = BIT(1),       ///< B
    KEY_SELECT  = BIT(2),       ///< Select
    KEY_START   = BIT(3),       ///< Start
    KEY_DRIGHT  = BIT(4),       ///< D-Pad Right
    KEY_DLEFT   = BIT(5),       ///< D-Pad Left
    KEY_DUP     = BIT(6),       ///< D-Pad Up
    KEY_DDOWN   = BIT(7),       ///< D-Pad Down
    KEY_R       = BIT(8),       ///< R
    KEY_L       = BIT(9),       ///< L
    KEY_X       = BIT(10),      ///< X
    KEY_Y       = BIT(11),      ///< Y
    KEY_ZL      = BIT(14),      ///< ZL (New 3DS only)
    KEY_ZR      = BIT(15),      ///< ZR (New 3DS only)
    KEY_TOUCH   = BIT(20),      ///< Touch (Not actually provided by HID)
    KEY_CSTICK_RIGHT = BIT(24), ///< C-Stick Right (New 3DS only)
    KEY_CSTICK_LEFT  = BIT(25), ///< C-Stick Left (New 3DS only)
    KEY_CSTICK_UP    = BIT(26), ///< C-Stick Up (New 3DS only)
    KEY_CSTICK_DOWN  = BIT(27), ///< C-Stick Down (New 3DS only)
    KEY_CPAD_RIGHT = BIT(28),   ///< Circle Pad Right
    KEY_CPAD_LEFT  = BIT(29),   ///< Circle Pad Left
    KEY_CPAD_UP    = BIT(30),   ///< Circle Pad Up
    KEY_CPAD_DOWN  = BIT(31),   ///< Circle Pad Down
};


#endif // RENDERWIDGET_H
