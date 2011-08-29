/*
 ****************************************************************************
 * screenshot.h
 *
 * Created on: 2011-7-27
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified on: 2011-7-27
 * Last Modified by: 贺辉
 ***************************************************************************
 */



#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QtGui/QWidget>
#include <QEvent>


#include "../ui/ui_screenshot.h"

#include "selecttargetimagewidget.h"

#include "../../core/mysharedlib_global.h"


namespace HEHUI {


class MYSHAREDLIB_API Screenshot : public QWidget
{
    Q_OBJECT

public:
    //Screenshot(QWidget *parent = 0, Qt::WindowFlags fl = Qt::Popup | Qt::FramelessWindowHint | Qt::SplashScreen);
    Screenshot(QWidget *parent = 0, Qt::WindowFlags fl = Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    ~Screenshot();


protected:
    bool event(QEvent *e);


signals:
    void imageSelected(const QImage &image);


public slots:


private:
    void drawBackground();

    void showSelectedRect();
    void updateSelectedRect();

private:
    Ui::ScreenshotClass ui;

    SelectTargetImageWidget * m_targetPixmapWidget;

    QPixmap originalPixmap;

    QPixmap resultPixmap;

    //    QImage sourceImage;
    //    QImage destinationImage;
    QImage backgroundImage;

    QPoint topLeft;
    QPoint bottomRight;





};

} //namespace HEHUI

#endif // SCREENSHOT_H
