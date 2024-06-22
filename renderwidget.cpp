#include "renderwidget.h"
#include <QRandomGenerator>
#include <iostream>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QPixmap>
#include "ipaddressdialog.h"
#include <default_font_bin2.h>
#include <QEvent>
#include <QKeyEvent>
#include <QShortcut>
#include <QKeySequence>

struct UDPixel
{
    uint8_t x, y;
    uint16_t chr;
};


void unpackCharAndColors(uint16_t packedData, char &c, uint8_t &fg, uint8_t &bg) {
    // Unpack the character from bits 7-0
    c = static_cast<char>(packedData & 0xFF);

    // Unpack the foreground color from bits 10-8
    fg = static_cast<uint8_t>((packedData >> 8) & 0x07);

    // Unpack the background color from bits 6-4
    bg = static_cast<uint8_t>((packedData >> 11) & 0x07);
}


uint16_t packCharAndColors(char c, uint8_t fg, uint8_t bg)
{
    uint16_t packedData = 0;

    // Pack the character into bits 7-0
    packedData |= static_cast<uint16_t>(c);

    // Pack the foreground color into bits 8-10
    packedData |= (fg & 0x07) << 8;

    // Pack the background color into bits 11-13
    packedData |= (bg & 0x07) << 11;

    return packedData;
}


QImage combineWithAlpha(const QImage &image, const QImage &alphaImage, qreal opacity) {
    // Ensure both images are of the same size
    if (image.size() != alphaImage.size()) {
        qWarning("Image and alphaImage must have the same size");
        return QImage();
    }

    // Create a copy of the JPEG image with an alpha channel
    QImage resultImage(image.size(), QImage::Format_ARGB32);
    resultImage.fill(Qt::transparent);

    // Iterate over each pixel and set its alpha value based on the corresponding pixel in the alpha image
    for (int y = 0; y < resultImage.height(); ++y) {
        for (int x = 0; x < resultImage.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            QRgb alphaPixel = alphaImage.pixel(x, y);

            // Extract alpha value from the grayscale image and apply opacity
            uchar alpha = qBlue(alphaPixel);
            alpha = qBound<uchar>(0, qRound(alpha * opacity), 255);

            // Set the alpha value of the pixel in the result image
            QRgb resultPixel = qRgba(qRed(pixel), qGreen(pixel), qBlue(pixel), alpha);
            resultImage.setPixel(x, y, resultPixel);
        }
    }

    return resultImage;
}

void drawCharacter(QPixmap &buffer, char character, QColor const& color, unsigned char *fontData)
{
    buffer.fill(Qt::transparent);
    QPainter painter(&buffer);

    // Calculate the index of the character in the font data
    int index = static_cast<unsigned char>(character) * 8;
    painter.setPen(color);

    // Loop through each row of the character
    for (int row = 0; row < 8; ++row) {
        // Get the byte representing the current row
        unsigned char rowData = fontData[index + row];

        // Loop through each bit in the byte
        for (int col = 0; col < 8; ++col) {
            // Check if the current bit is set (pixel is on)
            bool pixelOn = rowData & (1 << (7 - col));
            if (!pixelOn)
                continue;

            // Draw the pixel
            painter.drawPoint(col, row);
        }
    }
}

void RenderWidget::clearScreen()
{
    std::fill(topPixels.begin(), topPixels.end(), packCharAndColors(' ', 0, 0));
    std::fill(botPixels.begin(), botPixels.end(), packCharAndColors(' ', 0, 0));
}

bool RenderWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_F7) {
            // Perform action associated with Ctrl+S shortcut
            onConnect();
            return true; // Event handled
        }
    }
    // Let the base class handle the event
    return QWidget::eventFilter(obj, event);
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    // Re-paint to see the mouse cursor
    update();
}

RenderWidget::RenderWidget(QWidget *parent)
    : QWidget{parent}
{
    setWindowTitle("Noise Commander 3DS - Network screencast client - PATREON LINK: patreon.com/NoiseCommander3DS");

    // Connect the signal from NetworkListeningThread to a slot in RenderWidget
    QObject::connect(&networkListener, &NetworkListener::dataReceived,
                     this, &RenderWidget::handleDataReceived);

    QObject::connect(&networkListener, &NetworkListener::dataReceived,
                     this, &RenderWidget::handleDataReceived);

    QObject::connect(&networkListener, &NetworkListener::signalConnectionLost,
                     this, &RenderWidget::onConnectionLost);

    // Connect the resize event to a slot
    connect(this, &RenderWidget::resized, this, &RenderWidget::onResized);

    // Create a menu bar
//    menuBar = /*n*/ew QMenuBar(this);

    // Add menus and actions to the menu bar
//    QMenu *fileMenu = menuBar->addMenu("&Menu"); // Example menu

    QAction *action = new QAction("Connect", this);
    action->setShortcut(QKeySequence(Qt::Key_F7));
    action->setStatusTip("This is my action");
    connect(action, &QAction::triggered, this, &RenderWidget::onConnect);
//    fileMenu->addAction(action);


    QShortcut *shortcut = new QShortcut(QKeySequence("F7"), this);
    connect(shortcut, &QShortcut::activated, this, &RenderWidget::onConnect);

    QShortcut* cursorShortcut = new QShortcut(QKeySequence("F4"), this);
    connect(cursorShortcut, &QShortcut::activated, this, &RenderWidget::onToggleMouseVisibility);

    topPixels.resize(50 * 30);
    botPixels.resize(40 * 30);

    clearScreen();

    QImage color;
    QImage alpha;
    alpha.load(":/img/thebluealpha.jpg");
    color.load(":/img/theblue.jpg");
    bgImage = combineWithAlpha(color, alpha, 1.0);

    advertImage.load(":/img/PatreonBottomScreenLogo.jpg");


//    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setParent(0); // Create TopLevel-Widget
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    btnConnect.setText("Connect (F7)");
    btnConnect.setParent(this);
    btnConnect.move(100, 100);
    QObject::connect(&btnConnect, &QPushButton::clicked, this, &RenderWidget::onConnect);
    btnConnect.show();

    // installEventFilter(this);
    setMouseTracking(true);
}

// Utility to reduce boilerplate of setting a QPainter up
void makePainter(QPainter& painter, uint8_t const& screenNr, qreal const& bgw, float const& xpos)
{
    painter.setPen(Qt::black);  // Set the pixel color

    painter.translate(xpos, 0); // Center horizontally
    painter.scale(bgw, bgw); // Scale to fit screen
    painter.translate(screenNr == 0 ? 159 : 195,
                      screenNr == 0 ? 74: 321); // Move upper left corner to match the background position
    float facktohr = 320.0 / 354;
    painter.scale(facktohr, facktohr); // Scale width to match screen witdh in background picture

    uint8_t const sqsiz = 8;

        // Set the font size to fit into the 8x8 tile
    QFont font("Arial", sqsiz - 1); // Adjust the font and size as needed
    font.setBold(true); // Set the font weight to bold
    painter.setFont(font);

    //           QFont myFont{"Times New Roman", 16};
    font.setStyleStrategy(QFont::NoAntialias);

    // THIS is the key line:
    font.setHintingPreference(QFont::HintingPreference::PreferFullHinting);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

}

void drawFilledCircle(QPainter &painter, const QPoint &center, int radius) {
    // Calculate the top-left point of the bounding rectangle
    QPoint topLeft(center.x() - radius, center.y() - radius);

    // Create a QRect that represents the bounding rectangle of the circle
    QRect rect(topLeft, QSize(radius * 2, radius * 2));

    // Set the fill color with 50% transparency
    QColor fillColor(255, 255, 255, 128); // Black color with alpha value of 128 (50% transparency)

    // Set the brush color (fill color)
    painter.setBrush(fillColor);

    // Set the pen color to white with a width of 0 (no border)
    painter.setPen(QPen(Qt::white, 0));

    // Draw a filled ellipse (circle) within the bounding rectangle
//    painter.setBrush(Qt::white); // Set the brush color (fill color)
    painter.drawEllipse(rect);
}

void RenderWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    // Compute scaling and x-offset so that the image is always centered.
    const qreal bgw = static_cast<qreal>(height()) / (bgImage.height());
    const float xpos =(width() - bgImage.width() * bgw) * 0.5f;

    // Paint background image
    QPainter bgp(this);
    bgp.translate(xpos, 0);
    bgp.scale(bgw, bgw);
    bgp.drawImage(0, 0, bgImage);
    bgp.setPen(Qt::white);
//    bgp.drawText(rect(), Qt::AlignLeft,  QString::number((bgw)));
//    bgp.drawText(QRect(50, 450, 100, 100), Qt::AlignLeft, displayText);

    // Show button presses
    if (kHeld & KEY_X)
        drawFilledCircle(bgp, QPoint(604, 181), 16);
    if (kHeld & KEY_Y)
        drawFilledCircle(bgp, QPoint(569, 217), 16);
    if (kHeld & KEY_A)
        drawFilledCircle(bgp, QPoint(640, 217), 16);
    if (kHeld & KEY_B)
        drawFilledCircle(bgp, QPoint(604, 252), 16);

    if (kHeld & KEY_START)
        drawFilledCircle(bgp, QPoint(571, 352), 12);
    if (kHeld & KEY_SELECT)
        drawFilledCircle(bgp, QPoint(571, 399), 12);

    if (kHeld & KEY_L)
        drawFilledCircle(bgp, QPoint(37, 37), 30);
    if (kHeld & KEY_R)
        drawFilledCircle(bgp, QPoint(640, 37), 30);

    if (kHeld & KEY_DUP)
        drawFilledCircle(bgp, QPoint(74, 307), 14);
    if (kHeld & KEY_DRIGHT)
        drawFilledCircle(bgp, QPoint(100, 333), 14);
    if (kHeld & KEY_DDOWN)
        drawFilledCircle(bgp, QPoint(74, 357), 14);
    if (kHeld & KEY_DLEFT)
        drawFilledCircle(bgp, QPoint(48, 333), 14);

    if (kHeld & KEY_CPAD_UP)
        drawFilledCircle(bgp, QPoint(74, 215-30), 14);
    if (kHeld & KEY_CPAD_RIGHT)
        drawFilledCircle(bgp, QPoint(74+30, 215), 14);
    if (kHeld & KEY_CPAD_DOWN)
        drawFilledCircle(bgp, QPoint(74, 215+30), 14);
    if (kHeld & KEY_CPAD_LEFT)
        drawFilledCircle(bgp, QPoint(74-30, 215), 14);

    bgp.end();

    uint8_t const sqsiz = 8;

    QColor palette[8] = {
        QColor(0, 0, 0), // black
        QColor(128, 0, 0), // red
        QColor(0, 128, 0),  // green
        QColor(128, 128, 0), // yellow
        QColor(0, 0, 128), // blue
        QColor(128, 0, 128), // magenta
        QColor(0, 128, 128), // cyan
        QColor(192, 192, 192), // white
    };


    std::vector <uint16_t>* pPixels = nullptr;

    QPixmap charBuffer(8, 8);

    // Paint both screens.
    for (uint8_t screenIndex=0; screenIndex < 2; screenIndex++)
    {
        QPainter painter(this);
        makePainter(painter, screenIndex, bgw, xpos);
        pPixels = screenIndex == 0 ? &topPixels : & botPixels;

        uint8_t width = screenIndex == 0 ? 50 : 40;
        painter.fillRect(QRect(0, 0, width * 8, 30*8), Qt::black);

        for (uint i=0; i<pPixels->size(); i++)
        {
            char c;
            uint8_t fg, bg;
            // Note the pointer dereferencing syntax
            unpackCharAndColors((*pPixels)[i], c, fg, bg);

            int x = i % width;
            int y = i / width;
            QColor color = palette[bg];
            QRect rect(x * sqsiz, y * sqsiz,  sqsiz, sqsiz);
            painter.setPen(color);
            painter.fillRect(rect, color);

            if (c != 0 && c != ' ') {
                color = palette[fg];
                painter.setPen(color);
//                painter.drawText(rect, Qt::AlignCenter, QString(c)); // Change the text as needed
                drawCharacter(charBuffer, c, color, &default_font_bin2[0]);
                painter.drawPixmap(x*sqsiz, y*sqsiz, charBuffer);
            }
        }

        if (screenIndex == 1 && touchX != 0 && touchY != 0) {
            drawFilledCircle(painter, QPoint(touchX, touchY), 25);
        }

        if (btnConnect.isVisible() && screenIndex == 0)
            painter.drawImage(QRect(0, 0, 400, 240), advertImage);

        painter.end();
    }

    if (showMouse) {
        QPainter painter(this);
        QPoint globalMousePos = QCursor::pos();
        // Map the global position to the widget's coordinate space
        QPoint widgetMousePos = mapFromGlobal(globalMousePos);
        drawFilledCircle(painter, widgetMousePos, 50);
    }

}

void RenderWidget::onTouchEvent()
{
    displayText = QString("Touch x: %1 y: %2").arg(touchX).arg(touchY);

}

QColor RenderWidget::generateRandomColor()
{
    int red = QRandomGenerator::global()->bounded(256);
    int green = QRandomGenerator::global()->bounded(256);
    int blue = QRandomGenerator::global()->bounded(256);
    return QColor(red, green, blue);
}

void RenderWidget::closeEvent(QCloseEvent *event)
{
    // Stop the NetworkListener thread when the application is closed
    if (networkListener.isRunning()) {
        networkListener.close();
        networkListener.quit();
        networkListener.wait(2000);
    }
    // Call the base class implementation
    QWidget::closeEvent(event);
}

std::vector<uint16_t> convertByteArrayToUint16(const QByteArray& byteArray, bool littleEndian) {
    std::vector<uint16_t> result;

    // Ensure the size of the byte array is even
    int size = byteArray.size();
    if (size % 2 != 0) {
        // Handle odd size if needed
        return result;
    }

    for (int i = 0; i < size; i += 2) {
        uint16_t value = 0;

        if (littleEndian) {
            // Little-endian: Least significant byte comes first
            value |= static_cast<uint8_t>(byteArray[i]);
            value |= static_cast<uint8_t>(byteArray[i + 1]) << 8;
        } else {
            // Big-endian: Most significant byte comes first
            value |= static_cast<uint8_t>(byteArray[i + 1]);
            value |= static_cast<uint8_t>(byteArray[i]) << 8;
        }

        result.push_back(value);
    }

    return result;
}

uint32_t combineInt16(int16_t high, int16_t low) {
    // Shift the high value left by 16 bits and then bitwise OR it with the low value
    return static_cast<uint32_t>(high) << 16 | (static_cast<uint16_t>(low) & 0xFFFF);
}

void RenderWidget::handleDataReceived(const QByteArray &data)
{
    if (!data.length())
        return;

    std::vector<uint16_t> entries = convertByteArrayToUint16(data, true);

    char text[5];
    for (int i = 0; i < 5; ++i) {
        char c;
        uint8_t fg, bg;
        // Note the pointer dereferencing syntax
        unpackCharAndColors(entries[i+1], c, fg, bg);
        text[i] = c;
    }
    position = 0;

    uint16_t currentScreenNr = 0;
    uint16_t topBit = 1 << 15;
//    uint16_t screenBit = topBit | (1 << 14);

    for (int i = 0; i < entries.size(); ++i)
    {
        uint16_t& entry = entries[i];

        // The first entry contains the screen number.
//        if (entries[i] & screenBit){
        if ((entry & 0xC000) == 0xC000){
            uint16_t bitmask = ~(0x03 << 14); // 0x03 represents 11 in binary (bits 14 and 15 set)

            // Perform bitwise AND operation to clear bits 14 and 15
            currentScreenNr = entry & bitmask;
            if (currentScreenNr == 2) {
                kHeld = combineInt16(entries[i+1], entries[i+2]);
                continue;
            }
            else if (currentScreenNr == 3) {
                // Handle touch
                touchX = entries[i+1];
                touchY = entries[i+2];
                onTouchEvent();
                continue;
            }
            position = 0;
        }

        // If the top bit is set, then this is a control entry.
        // Currently it can only contain a positional jump value.
        else if (entries[i] & topBit) {
            position = entries[i] & 0x7FFF;
        }

        // The default case simply contains a pixel.
        else {
            // I am not used to dereferencing pointer like this, so need to remember!
//            (*pointerPixels)[position] = entries[i];
            switch(currentScreenNr) {
            case 0: {
                topPixels[position] = entry;
            } break;
            case 1: {
                botPixels[position] = entry;
            } break;
            }
            // Advance "cursor" position
            position++;
        }
    }

    update();
}

void RenderWidget::onConnect()
{
    IpAddressDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        QString ipAddress = dialog.ipAddress();
        QString portNumber = dialog.portNumber();
        networkListener.onConnect(ipAddress, portNumber);
        btnConnect.hide();
//        qDebug() << "IP Address:" << ipAddress;
//        qDebug() << "Port:" << portNumber;
    }
}

void RenderWidget::onConnectionSucceeded()
{
    QMessageBox msgBox;
    msgBox.setText("Connection successful");
    msgBox.exec();
    btnConnect.hide();
}

void RenderWidget::onConnectionLost()
{
    kHeld = 0;
    clearScreen();
    update();
    btnConnect.show();
}

void RenderWidget::onResized()
{
    // Connect the resize event to a slot
    btnConnect.move(
        (rect().width() / 2) - (btnConnect.width() / 2),
        rect().height() * 0.7);

}

void RenderWidget::showButton()
{
    btnConnect.show();
}
