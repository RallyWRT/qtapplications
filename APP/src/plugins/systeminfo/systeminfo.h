#ifndef SYSINFODLG_H
#define SYSINFODLG_H

#include <QMainWindow>
#include <QProcess>
#include <QProgressBar>



#include "ui_systeminfo.h"

#include "HHSharedGUI/hdatabaseconnecter.h"

namespace HEHUI {


class SystemInfo: public QMainWindow {
    Q_OBJECT

public:

    SystemInfo(bool isYDAdmin = false, QWidget *parent = 0);
    ~SystemInfo();

    static bool isRunning(){return running;}

    void retranslateUi();

protected:
    //bool event ( QEvent * event );
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *obj, QEvent *event);
    void languageChange();


public slots:



private slots:
    void slotResetStatusBar(bool show);

    void slotScanSystem();
    void slotScannerExit( int exitCode, QProcess::ExitStatus exitStatus);
    void slotReadReport();


    void slotUploadSystemInfo();

//    void slotUploadSystemInfoToSitoyDBServer();

    void slotQuerySystemInfo();


    //重新设置界面信息
    void slotResetAllInfo();
    //重新从界面上获取信息
    void slotGetAllInfo();



    void on_toolButtonQuerySystemInfo_clicked();
    void on_toolButtonUpload_clicked();
    void on_toolButtonScan_clicked();



private:
    void initStatusBar();


private:
    Ui::SystemInfoMainWindowUI ui;

    static bool running;
    static qint64 failedFilesTotalSize;

    bool isScanning;
    //bool isUploaded;
    //bool isUploadedToSitoyDB;

    QWidget *m_progressWidget;
    QLayout* hlayout;
    QLabel *label;
    QProgressBar *progressBar;


    QProcess *process;
    QString m_systemInfoFilePath;

    //OS Info
    QString os;
    QString installationDate;
    QString workgroup;
    QString computerName;
    QString windowsDir;

    QStringList drivesInfo;

    QStringList usersInfo;


    //User Info
    QString userName;
    bool isAdmin;
    QString myDocuments;

    QString emailStoreRoot;
    int emailFolderSize;
    QString biggestDBXFile;

    int tempDirSize;
    int ieTempDirSize;

    //Devices Info
    QString cpu;
    QString dmiUUID;
    QString motherboardName;
    QString chipset;
    QString memory;

    QString video;
    QString monitor;
    QString audio;
    QString partitionsTotalSize;

    QStringList network1Info;
    QStringList network2Info;

    //Other Info
    QString pNo;
    QString remark;
    QString dept;
    QString area;

    QDate motherboardMDate;
    QDate monitorMDate;

    QString serviceNumber;

    bool recordExists;

    QRegExpValidator *validator;


    DatabaseConnecter *dc;
    QSqlQueryModel *queryModel;

    bool isYDAdmin;

    static QMap<QString/*Short Name*/, QString/*Department*/>departments;



};

}

#endif // SYSINFODLG_H

