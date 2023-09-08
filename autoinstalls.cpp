#include "autoinstalls.h"
#include "qdebug.h"
#include "qregularexpression.h"
#define G_COLOR(value)         "<font color=green>" value"</font>"
#define R_COLOR(value)         "<font color=red>" value"</font>"
static BOOL  CALLBACK  EnumChildProc(HWND hwndChild, LPARAM lParam);
namespace  {
  QStringList Save_other;    ///static  QStringList Save_other;
}

Exesetup::Exesetup(QObject *parent)
    : QObject{parent},Installprocess(new QProcess(this)),info(new QString("")),countapp(0)
{
}
Exesetup::~Exesetup()
{
    delete Installprocess;
    delete info;
}
void Exesetup::exeinstall(const QStringList& applist,bool cmd_or_man)
{
    HWND apphwnd;
    int processvalue_return;
    static QRegularExpression reg(".*\\\\");
    if (cmd_or_man) {
        QStringList arguments;
        Installprocess->setWorkingDirectory("./");
        foreach (QString var, applist) {
                arguments = analysis_args(var.split('#').last());                                                                           //targuments<<QString("/s")<<QString("/v /qn REBOOT=Suppress");
                foreach (QString var, arguments) {
                    if (var.isEmpty())  arguments.remove(arguments.indexOf(var));
                }
                 processvalue_return = Installprocess->execute(var.split('#').first().simplified(),arguments);
                 //Wait process exit......  good math   If the process cannot be started, -2 is returned. If the process crashes, -1 is returned. Otherwise, the process' exit code is returned.
                switch (processvalue_return) {
                case 0:{
                                *info = var.split('#').first().replace(reg,"")+" return: %1  "+G_COLOR("\t\tInstallation succeeded");                     //qDebug()<<var.split('#').first().replace(QRegularExpression(".*\\\\"),"");
                                *info=info->arg(processvalue_return);
                            }break;
                case -1:{
                                *info = var.split('#').first().replace(reg,"")+" return: %1  "+R_COLOR("\tInstallation failed: test app crashes!");
                                *info=info->arg(processvalue_return);
                            }break;
                case -2:{
                                 QString temp =var;//msi app will use this way to installed!
                                 QString sys_arg = "\""+var.split("#").first()+"\"    "+var.split("#").last();
                                 int status_sys;
                                 //qDebug()<<sys_arg.toUtf8();
                                 status_sys = system(sys_arg.toUtf8());
                                 switch (status_sys) {
                                     case 0:{
                                                 *info = temp.split('#').first().replace(reg,"")+" return: %1 system: %2\t"+G_COLOR("\tInstallation succeeded");
                                                 *info=info->arg(processvalue_return).arg(status_sys);
                                     }break;
                                     case 1:{
                                                 *info = temp.split('#').first().replace(reg,"")+" return: %1 system: %2\t"+R_COLOR("\tInstallation failed(system call)");
                                                 *info=info->arg(processvalue_return).arg(status_sys);
                                     }break;
                                     default:{
                                                 *info = temp.split('#').first().replace(reg,"")+"return: %1 system: %2\t"+R_COLOR("\tInstallation failed(system call)");
                                                 *info=info->arg(processvalue_return).arg(status_sys);
                                     }break;
                                }
                            } break;
                case 3010:{
                                     *info = var.split('#').first().replace(reg,"")+" return: %1  "+G_COLOR("\t\tInstallation succeeded");
                                     *info=info->arg(processvalue_return);
                                }break;
                default:{                                                                                               // Intel(R)PowerAndThermalAnalysisTool
                                QString temp =var;
                                QString system_str = "\""+var.replace('#',"\"  ");
                                if( !system( system_str.toUtf8().simplified() ) ){
                                    *info = temp.split('#').first().replace(reg,"")+" return: %1  "+G_COLOR("\tInstallation succeeded(system call)");
                                    *info=info->arg(processvalue_return);
                                }
                                else {
                                   *info = temp.split('#').first().replace(reg,"")+" return: %1  "+R_COLOR("\tInstallation failed: test app can not started install!(Unknown Error!! try system call )");
                                    *info=info->arg(processvalue_return);
                                }
                            }break;
                }
                arguments.clear();
                emit execomplte(*info);
                emit execomplte(countapp++,cmd_or_man);
        }
    }else {                                                                                                                                                                     //Only Open clicked exe app Wait AutoInstalls Process send signals
        foreach (QString var, applist) {
            Installprocess->start(var.split('#').first());                                                                                                      //will try execute
            while (Installprocess->state()==QProcess::Running || Installprocess->state()==QProcess::Starting ) {             //locked Main program Run
                Installprocess->waitForFinished();
                apphwnd = FindWindow(NULL,var.split('#').last().toStdWString().c_str());                                              //Wait exe windows exit
                if (apphwnd ==NULL &&  Installprocess->state() == QProcess::NotRunning) {                                          //Debug Process::is start Installprocess->state() == QProcess::NotRunning 2023-02-07 13:20 add
                    *info = var.split('#').first().replace(reg,"")+G_COLOR("\tInstallation succeeded");
                    emit execomplte(*info);
                    emit execomplte(countapp++,cmd_or_man);                                                                                       //emit ocrtool process finished signales for Mian, Tell Main Program  progress schedule
                    break;
                }
            }
        }
    }
}
const QStringList Exesetup::analysis_args(const QString& args)
{
    QString analysis_arg;
    QString temp;
    QStringList arguments;
    QRegularExpressionMatch match;
    static QRegularExpression reg("\".*\"");
    analysis_arg = args;
    if (analysis_arg.contains("\"")) {
        if(analysis_arg.indexOf(reg,0,&match) != 0)
        {
            temp = match.captured().replace('"',"");
            analysis_arg.remove(reg);
        }
        arguments=(analysis_arg.simplified().replace("/","#/").split('#'));
        if (temp.contains(",")){
            foreach (QString var, temp.split(',')) {
                if( var.remove("\"").isEmpty()) continue;
                arguments.append(var);
            }
            return arguments;
        }
        arguments.append(temp);
        return arguments;
    }
    arguments.append(analysis_arg.simplified().replace("/","#/").split('#'));
    return arguments;
}
//Sent Windows Signals for Test App
AutoInstalls::AutoInstalls(QObject *parent)
    : QObject{parent},savelist(new QStringList)
{

}
AutoInstalls::~AutoInstalls()
{
    delete savelist;
}
void AutoInstalls::appautoinstall(const QString& win_name, const QStringList& setuplist)
{
     InstallActions(win_name,setuplist);
}
void AutoInstalls::InstallActions(const QString& namestr,const QStringList& setup)
{
    qDebug()<<"namestr:     "<<namestr<<"Arguments:      "<<setup;
    *savelist = setup;
    if(!savelist->isEmpty()) Save_other = *savelist;
    HWND Appname;
    Appname = FindWindow(NULL,namestr.toStdWString().c_str());
    if (Appname != nullptr) {
            for (int var = 0; var < 4; ++var) {
                EnumChildWindows(Appname,EnumChildProc,256);
            }
            browersetups(Appname,setup);
    }else{
                Appname = FindWindow(NULL,(namestr+" ").toStdWString().c_str());                         ///some tools app Window name have space
                if (Appname != nullptr) {
                    for (int var = 0; var < 4; ++var) {
                        EnumChildWindows(Appname,EnumChildProc,256);
                    }
                    browersetups(Appname,setup);
                }
    }
}
BOOL  CALLBACK   EnumChildProc(HWND hwndChild, LPARAM lParam){
char sBuf[lParam];
QStringList setuplist;
setuplist<<QString("Next")<<QString("&Next")<<QString("&Next >")<<QString("I &accept the agreement")
               <<QString("Install")<<QString("&Install")<<QString("Finish")<<QString("&Finish");
GetWindowTextA(hwndChild,sBuf,256);
if(!Save_other.isEmpty()) setuplist.append(Save_other);
foreach (QString var, setuplist) {
    if(QString::fromUtf8(sBuf)==var){
        WORD setupID = (WORD)GetDlgCtrlID(hwndChild);
        SendMessage(hwndChild,WM_COMMAND,MAKELONG(setupID,BN_CLICKED),LPARAM(hwndChild));
    }
}
return TRUE;
}
void AutoInstalls::browersetups(HWND Appname,const QStringList& setupstrlist)
{
    HWND button;
    foreach (QString var, setupstrlist) {
          if((button=FindWindowEx(Appname,nullptr,nullptr,var.toStdWString().c_str())) != nullptr) {
            SendMessage(Appname,WM_COMMAND,MAKELONG((WORD)GetDlgCtrlID(button),BN_CLICKED),LPARAM(button));
        }
    }
}
