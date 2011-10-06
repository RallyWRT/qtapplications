/*
 ****************************************************************************
 * usermanagerplugin.h
 *
 * Created on: 2009-8-24
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
 * Last Modified on: 2010-05-19
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#ifndef USERMANAGERPLUGIN_H_
#define USERMANAGERPLUGIN_H_

#include <QObject>

#include "usermanagermainwindow.h"

//#include "../../shared/gui/plugin/guipluginbase.h"
#include "HHSharedGUI/hguipluginbase.h"

namespace HEHUI {

class UserManagerPlugin : public GUIPluginBase  {
    Q_OBJECT
    //Q_INTERFACES(AppInterface)

public:
    UserManagerPlugin();
    virtual ~UserManagerPlugin();


    //bool init( QWidget * parentWidget, QMenu *menu, QToolBar *toolBar, QSystemTrayIcon *systemTrayIcon, const QString& pName, const QString& pVersion  );
    //QWidget *parentWidgetOfPlugin();
    bool isSingle();
    QString name () const;
    QString version() const;
    QString description() const;

    QIcon icon () const;
    QString whatsThis () const;
    QString toolTip () const;

    bool unload();


private slots:
    void slotMainActionForMenuTriggered();
    void slotUserManagerWidgetDestoryed(QObject *obj);



private:
    QList<UserManagerMainWindow *> userManagerWidgetList;




};

} //namespace HEHUI

#endif /* USERMANAGERPLUGIN_H_ */