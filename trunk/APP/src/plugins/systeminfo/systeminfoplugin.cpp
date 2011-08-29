/*
 ****************************************************************************
 * systeminfoplugin.cpp
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




#include "systeminfoplugin.h"


//#include "../../shared/core/user.h"
//#include "../../shared/gui/login/loginbase.h"

#include "HHSharedCore/huser.h"
#include "HHSharedGUI/hloginbase.h"


namespace HEHUI {

SystemInfoPlugin::SystemInfoPlugin() {

        systeminfoWidgetList = QList<SystemInfo *> ();

}

SystemInfoPlugin::~SystemInfoPlugin() {
        unload();
}


/*
bool SystemInfoPlugin::init( QWidget * parentWidget, QMenu *menu, QToolBar *toolBar, QSystemTrayIcon *systemTrayIcon, const QString& pName, const QString& pVersion  ){
	setParent(parentWidget);
	this->parentWidget = parentWidget;
	this->systemTrayIcon = systemTrayIcon;
	QAction *action = new QAction(icon(), name(), parentWidget);
	action->setToolTip(toolTip());
	action->setStatusTip(toolTip());
	action->setWhatsThis(whatsThis());
	connect(action, SIGNAL(triggered()), this, SLOT(slotRun()));

	if(menu){
		menu->addAction(action);
	}

	if(toolBar){
		toolBar->addAction(action);
	}

	if(systemTrayIcon){
		//TODO:
		//systemTrayMenu->addAction(action);
	}

	return true;

}


QWidget * SystemInfoPlugin::parentWidgetOfPlugin(){

	return parentWidget;
}

*/

bool SystemInfoPlugin::isSingle(){

	return true;

}

QString SystemInfoPlugin::name () const{
	return QString(tr("System Information"));
}

QString SystemInfoPlugin::version() const{
    return "2010.05.10";
}

QString SystemInfoPlugin::description() const{
    return QString(tr("System Information Plugin"));
}

QIcon SystemInfoPlugin::icon () const{
	return QIcon(":/resources/images/systeminfo.png");

}

QString SystemInfoPlugin::whatsThis () const{
	return QString(tr("System Information"));
}

QString SystemInfoPlugin::toolTip () const{
	return QString(tr("System Information"));
}

bool SystemInfoPlugin::unload(){
    qDebug("----SystemInfoPlugin::unload()");

    emit signalPluginToBeUnloaded();

    if(systeminfoWidgetList.isEmpty()){
        return true;
    }

    foreach(SystemInfo *systemInfo, systeminfoWidgetList){
        if(!systemInfo){break;}
        if(systemInfo->close()){
            systeminfoWidgetList.removeAll(systemInfo);
            delete systemInfo;
            systemInfo = 0;
        }
    }

    return systeminfoWidgetList.isEmpty();

}

void SystemInfoPlugin::slotMainActionForMenuTriggered(){
	if(isSingle() && SystemInfo::isRunning()){
		//TODO: Activate the widget
		return;
	}

        QWidget *parentWidget = qobject_cast<QWidget *> (parent());

        HEHUI::User user;
        HEHUI::LoginBase login(&user, name(), parentWidget);
        if (!login.isVerified()) {
                return ;
        }

        bool isYDAdmin = false;
        if(user.getBusinessAddress() == "DG"){
            isYDAdmin = false;
        }else if(user.getBusinessAddress() == "YD"){
            isYDAdmin = true;
        }

        SystemInfo *systemInfo = new SystemInfo(isYDAdmin, parentWidget);
        connect(systemInfo, SIGNAL(destroyed(QObject *)), SLOT(slotSysteminfoWidgetDestoryed(QObject *)));

	if(parentWidget){
		if(QMdiArea *mdiArea = qobject_cast<QMdiArea *>(parentWidget)){
                    QMdiSubWindow *subWindow = new QMdiSubWindow;
                         subWindow->setWidget(systemInfo);
                         subWindow->setAttribute(Qt::WA_DeleteOnClose);
                         mdiArea->addSubWindow(subWindow);
                         connect(this, SIGNAL(signalPluginToBeUnloaded()), subWindow, SLOT(close()));

                        //mdiArea->addSubWindow(sqlExplorer, Qt::Dialog);

		}
	}

	systemInfo->show();
        systeminfoWidgetList.append(systemInfo);
}

void SystemInfoPlugin::slotSysteminfoWidgetDestoryed(QObject * obj){
    qDebug("----SystemInfoPlugin::slotSysteminfoWidgetDestoryed(QObject * obj)");

    SystemInfo *systemInfo = static_cast<SystemInfo *> (sender());
    if(systemInfo){
        systeminfoWidgetList.removeAll(systemInfo);
    }

}


Q_EXPORT_PLUGIN2(systemInfoPlugin, SystemInfoPlugin)


} //namespace HEHUI
