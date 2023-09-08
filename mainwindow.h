#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "other_mainwindow.h"
#include "setting_corl.h"
#include "QtEvents"
#include "QDebug"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    void Mpm_Me_function();
    void SetInFo2Bios();
    void SmcAppClicked(bool Humancheck);
    void time_function();
    void ClickAppinstall();
    void SetFindapplist(const QString oabspath){if(!oabspath.isEmpty())Find_clickappandnamelist->append(oabspath);};
    void SetFindapplist(const QStringList oabspathList){if(!oabspathList.isEmpty()) *Find_clickappandnamelist= oabspathList;};
    void AutoAppInstalls(const QStringList& ToolSetupName,const QStringList& SetupWinnamelist,const QList <QStringList>& SetupArgslist);
    void CmdAppInstall();
    bool RunAsAdmin();
    ~MainWindow();
protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
private:
    Ui::MainWindow* ui;
    setting_corl* set;
    Other_MainWindow* another;
    QString UserSelectDir;
    QString Bcu_ToolPath;
   QStringList*  Find_clickappandnamelist,*Notoolsfind,*ToolsWinlist;//*Notoolsfind,*ToolsWinlist
   QList <QStringList>* Nosetupsstrlist;
};
#endif // MAINWINDOW_H
