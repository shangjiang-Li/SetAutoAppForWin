#include <QProcess>
#include <QTimer>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QClipboard>
#include <QRandomGenerator>
#include <securitybaseapi.h>
#include <synchapi.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
/*
 * windows setting something
 * app from move windows MFC
 * 1.Power option setting                                                                                Every Option set never
 * 2.Power button setting                                                                               ShowHibernateOption
 * 3.Power button action setting                                                                    Do Nothing
 * 4.User Account Control Settings (UAC) setting                                       Very low
 * 5.Clean System and  APP log                                                                       Clean
 * 6.Firewall setting                                                                                        All Disable
 * 7.Check Device managerment                                                                     Yellow Bang Check
 * 8.Setting Dump File                                                                                    Complmet Memory Dump File
 * 9.Check ME and MPM check                                                                       MPM & ME Locked
 * 10.RW.exe envirmemt setting                                                                     OFF Memory Integrity  Disable DriverBlocklist
 * 11. Test App auto install                                                                              check with configure file
*/
#define RUN_OTHERAPP  ":/HPSMC.exe"       /*"SMC Test Tool 4.0/HPSMC.exe"*/
#define BIOS_CONFIG_FILE  "hwqa_bios.log"
#define OLDBIOS_CONFIG_FILE  "back_hwqa_bios.log"
#define AUTO_CMD_CONF   "./toolconf.txt"
#define AUTO_CLICK_CONF  "./appconfig.txt"
#define BNBAUTO_CMD_CONF   "./confgure/BNBtoolconf.txt"
#define BNBAUTO_CLICK_CONF  "./confgure/BNBappconfig.txt"
#define CNBAUTO_CMD_CONF   "./confgure/CNBtoolconf.txt"
#define CNBAUTO_CLICK_CONF  "./confgure/CNBappconfig.txt"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), set(new setting_corl(this)),
      another(new Other_MainWindow()),Find_clickappandnamelist(new QStringList),Notoolsfind(new QStringList),
      ToolsWinlist(new QStringList),Nosetupsstrlist(new QList <QStringList>)
{
    ui->setupUi(this);
    ui->textEdit->setStyleSheet("background-color:#E0E0E0");
    ui->textEdit->setLineWrapMode(QTextEdit::NoWrap);
    ui->AutoApp->setStyleSheet("QPushButton{text-align : left;background-color: rgba(0, 0, 0, 0)}");     //透明显示LCD
    ui->dock_information->setHidden(true);
    ui->ConfigBox->setFlat(true);
    ui->Command_tools->setStyleSheet("background-color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
    ui->Mpm_Me->setStyleSheet("background-color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
    ui->Manual_Set->setStyleSheet("background-color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
    ui->Other_App->setStyleSheet("background-color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
    ui->Del_reg->setStyleSheet("background-color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
    ui->Clean_Edit->setStyleSheet("background-color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
    ui->lcdNumber->setStyleSheet("color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);//you can not move window size
    //ui->textEdit->setStyleSheet("border-image:url(:/images/login.bmp)");
    QDate usedate = QDate::currentDate();
    int date_Y,date_M,date_D;
    usedate.getDate(&date_Y,&date_M,&date_D);
    if ( date_Y >= 2025 || date_Y<=2020) return;
    QTimer *Lcdtime=new QTimer(this);
    Lcdtime->start(1000);
    //Clicke Set MPM Configure file
    void (MainWindow::* pSetInFo2Bios)() = &MainWindow::SetInFo2Bios;
    connect(ui->Old_setting,&QPushButton::clicked,this,pSetInFo2Bios);
    //Clicke Set MPM Configure file
    void (MainWindow::* pCmdAppInstall)() = &MainWindow::CmdAppInstall;
    void (MainWindow::* pClickAppinstall)() = &MainWindow::ClickAppinstall;
    void (MainWindow::* pMpmMe)() = &MainWindow::Mpm_Me_function;
    void (MainWindow::* ptime_function)() = &MainWindow::time_function;

    connect(ui->Command_tools,&QPushButton::clicked,this,pCmdAppInstall);
    connect(ui->AutoApp,&QPushButton::clicked,this,pClickAppinstall);
    connect(ui->Mpm_Me,&QPushButton::clicked,this,pMpmMe);
    connect(ui->Manual_Set,&QPushButton::clicked,this,[=](){ emit set->setting_start(true);});
    connect(set,&setting_corl::setting_done,this,[=](QList <QString >*info){
        foreach(QString str,*info){
            ui->textEdit->append(str);
        }
        info->clear();
    });
    connect(set,&setting_corl::setting_Me_status,this,[=](bool Mestatus,QString err_info){
        if(Mestatus) {
            ui->textEdit->append("<h3><font color=green>Me and MPM are Locked,Please Check MEINFO.LOG  "+err_info+"</font></h3><h3>");
        }else{
           ui->textEdit->append("<h3><font color=red>Me was Unlocked And MPM was Locked, Please Check MEINFO.LOG and Lock Me  "+err_info+"</font></h3><h3>");
           if(err_info.contains("Reboot")) MainWindow::setDisabled(true);
              else   emit   ui->Other_App->clicked(true);
        }
    });
    connect(Lcdtime,&QTimer::timeout,this,ptime_function);
    connect(ui->Other_App,&QPushButton::pressed,ui->Other_App,[=](){
        ui->dock_information->setWindowTitle(BIOS_CONFIG_FILE);
        ui->dock_information->setHidden(false);
        ui->dock_information->show();
        ui->dock_information->setMinimumSize(QSize(400,600));
    });
    //Smc tool for Unlock MPM Settings
    void (MainWindow::* pSmcAppClicked)(bool Humancheck) = &MainWindow::SmcAppClicked;
    connect(ui->Other_App,&QPushButton::clicked,this,pSmcAppClicked );                                               //mecheck true --- No
    //Smc tool for Unlock MPM Settings
    connect(ui->Del_reg,&QPushButton::clicked,ui->Del_reg,[=]() {
        if (!system("%windir%\\system32\\reg.exe DELETE HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\Explorer /V ShowHibernateOption  /F"))  {
                ui->textEdit->append("<h3><font color=green>DELETE  REG ShowHibernateOption PASS</font></h3>");
         } else {
                ui->textEdit->append("<h3><font color=red>DELETE  REG ShowHibernateOption  Failure</font></h3>");
                ui->textEdit->append("<h3><font color=red>Check REG value  exist or not</font></h3>");
                ui->textEdit->append("<h3><font color=red>Computer\\HKEY_LOCAL_MACHINE\\SOFTWARE\\Policies\\Microsoft\\Windows\\Explorer</font></h3>");
                system("%windir%\\system32\\regedt32.exe");
           }
    });
    connect(ui->Clean_Edit,&QPushButton::clicked,ui->Clean_Edit,[=](){ui->textEdit->clear();});
/*
 * *******************************************************************************************************
 *
 * 1.Find tools user select and use app configure file.
 * 2.Thread to thread work.
 * 3.Work to work.
 * 4.Process any functions.
 * 5.Thread have two work
 *
 * *******************************************************************************************************
*/
  /*----------------------------------------------this have ingore two singles----------------------------------------------*/
    //have two work msg for main (app use cmd installed and app use human clicked installed)
 /*----------------------------------------------this have ingore two singles----------------------------------------------*/
    void (setting_corl::* pfindcomplete)(const QStringList& Oabspath,bool human) = &setting_corl::findcomplete;
    void (setting_corl::* pautoinsexeprocess)(int appcount,bool human) = &setting_corl::autoinsexeprocess;
    void (setting_corl::* pNoToolReturn)(const QStringList& Oabspath,bool human) = &setting_corl::NoToolReturn;
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void (setting_corl::* pFindOnlyFile_complete)(const QString& Onlyoabspath) = &setting_corl::FindOnlyFile_complete;
    void (setting_corl::* pFindOnlyFile_completeList)(const QStringList& Onlyoabspath) = &setting_corl::FindOnlyFile_complete;
    void (MainWindow::* pSetFindapplist)(const QString oabspath) = &MainWindow::SetFindapplist;
    void (MainWindow::* pSetFindapplist_L)(const QStringList oabspath) = &MainWindow::SetFindapplist;
    connect(set,pFindOnlyFile_complete,this,pSetFindapplist);
    connect(set,pFindOnlyFile_completeList,this,pSetFindapplist_L);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    connect(set,pfindcomplete,this,[=](QStringList Oabspath,bool human){
        QString str;
        human ? str = "cmd" : str = "clicked";
        QString tempout = "<h2><font color=green>---(%1)Finded Tools And Will For Install---</font></h2>";
        if(!Find_clickappandnamelist->isEmpty())Find_clickappandnamelist->clear();                                       //user select dir find tools and clicked windows names
        if (!Oabspath.isEmpty()) {   
            ui->textEdit->append(tempout.arg(str));
            foreach (QString var, Oabspath) {
                if(var.isEmpty())continue;
                ui->textEdit->append(var.simplified());
            }
            ui->textEdit->append(tempout.arg(str));
        }
    });
    connect(set,pNoToolReturn,this,[=](QStringList Notoolpath,bool human){
        QString str;
        human ? str = "cmd" : str = "clicked";
        QString tempout = "<h2><font color=red>---(%1)Have No Tools Finded It---</font></h2>";
        if (!Notoolpath.isEmpty()) {
            ui->textEdit->append(tempout.arg(str));
            foreach (QString var, Notoolpath) {
                if(var.isEmpty())continue;
                ui->textEdit->append(var.simplified());
            }
            ui->textEdit->append(tempout.arg(str));
        }else {
                 ui->textEdit->append("<h1><font color=red>Cmd &  Clicked Have No Tools Find It</font></h1>");
                 QString config_namestrclick = AUTO_CLICK_CONF;
                 if (ui->Config_bnb->isChecked()) config_namestrclick = BNBAUTO_CLICK_CONF;
                 if (ui->Config_cnb->isChecked()) config_namestrclick = CNBAUTO_CLICK_CONF;
                 QString config_namestrcmd = AUTO_CMD_CONF;
                 if (ui->Config_bnb->isChecked()) config_namestrcmd = BNBAUTO_CMD_CONF;
                 if (ui->Config_cnb->isChecked()) config_namestrcmd = CNBAUTO_CMD_CONF;
                 ui->textEdit->append(config_namestrclick);
                 ui->textEdit->append(config_namestrcmd);
                 ui->AutoApp->setEnabled(true);
                 ui->Command_tools->setEnabled(true);
        }
    });
    connect(set,pautoinsexeprocess,this,[=](){if(!set->getthredmsg()->isEmpty())ui->textEdit->append(*set->getthredmsg());});
/*----------------------------------------------this have ingore two singles----------------------------------------------*/
}
MainWindow::~MainWindow(){
    delete ui;
    delete set;
    delete another;
    delete Find_clickappandnamelist;
    delete Notoolsfind;
    delete ToolsWinlist;
    delete Nosetupsstrlist;
}
void MainWindow::Mpm_Me_function()
{
    QString filename;
    if (Bcu_ToolPath.isEmpty()) {
        filename = QFileDialog::getOpenFileName(this, tr("Open File"),"C:\\", tr("EXE (*.exe)"));
    }else  filename = Bcu_ToolPath;
    if(filename.isEmpty()==true) {
       QMessageBox::critical(this,"Error","Please input BiosConfigUtility/BCU EXE File");
       return ;
    }
    if(!filename.contains("BiosConfigUtility")){
        QMessageBox::critical(this,"Error","Please check file name is contains BiosConfigUtility");
        return ;
    }
    if (!system(filename.toUtf8()+"   -getconfig:"+BIOS_CONFIG_FILE)){
        Bcu_ToolPath=filename.toUtf8();
        emit set->setting_startMpm(BIOS_CONFIG_FILE,filename);
    }else return ;
}
void MainWindow::time_function(){
    static int msc=1;
    static int msc_tmp=msc;
    if (!RunAsAdmin())ui->textEdit->append("<h1><font color=red>App Can Not Get Administrator</font></h1>");
    connect(ui->AutoRun,&QRadioButton::clicked,this,[=](bool statu){ if (statu) msc = 1;});
    ui->lcdNumber->display(++msc);
    if (!ui->AutoRun->underMouse()) {
        ui->AutoRun->move(QRandomGenerator::global()->bounded(ui->textEdit->width()-ui->AutoRun->width()),QRandomGenerator::global()->bounded(ui->textEdit->height()-ui->AutoRun->height()));
    }
    if (ui->Other_App->underMouse()) emit ui->Other_App->pressed();
    if(10==msc && ui->AutoRun->isChecked()) {
        ui->lcdNumber->display(msc);
        emit set->setting_start(true);
    }else{
      ui->lcdNumber->display(msc);
    }
     ui->AutoRun->setStyleSheet("background-color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
    if( ++msc_tmp>9){
        ui->Mpm_Me->setStyleSheet("background-color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
        ui->Manual_Set->setStyleSheet("background-color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
        ui->Other_App->setStyleSheet("background-color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
        ui->Del_reg->setStyleSheet("background-color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
        ui->Clean_Edit->setStyleSheet("background-color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
        ui->lcdNumber->setStyleSheet("color:"+QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
        msc_tmp=1;
    }
}
void MainWindow::CmdAppInstall()
{
    ui->AutoApp->setEnabled(false);
    ui->Command_tools->setEnabled(false);
    QString config_namestrcmd = AUTO_CMD_CONF;
    if (ui->Config_bnb->isChecked()) config_namestrcmd = BNBAUTO_CMD_CONF;
    if (ui->Config_cnb->isChecked()) config_namestrcmd = CNBAUTO_CMD_CONF;
    static int cmdappcount = 0;
    static int userselectappnum = 0;
    QStringList toolapplist;
    QString readline;
    QFile ConfFile(config_namestrcmd);
    if (!ConfFile.exists()) QMessageBox::critical(this,"Error","toolconf.txt can not find it");
    if(!ui->FindFileBox->isChecked()){
        if (ConfFile.open(QFile::ReadOnly))
        while ((readline = ConfFile.readLine()) != nullptr) {
            if (readline.split('#').first().contains(".exe") || readline.split('#').first().contains(".msi")) {
                QFile Tool_exe(readline.split('#').first().simplified());
                if (Tool_exe.exists()) {
                    toolapplist.append(readline);
                }
            }
         }
        if (toolapplist.isEmpty()) {
                   ui->AutoApp->setEnabled(true);
                   ui->Command_tools->setEnabled(true);
                   QMessageBox::critical(this,"Error","Tools can not find it");
                   return;
               }
        emit set->autoexe_setupstart(toolapplist,true);                                                                                             //emit app install setup for command is true
        ConfFile.close();
    }
    if(ui->FindFileBox->isChecked()){
        QString tooldir = QFileDialog::getExistingDirectory(this, tr("Open Tools Dir"),NULL, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if ( tooldir== nullptr) {
           QMessageBox::critical(this,"Tool Path","Please Select Tools path");
           ui->AutoApp->setEnabled(true);
           ui->Command_tools->setEnabled(true);
           return;
        }else {
             UserSelectDir = tooldir;
             emit set->findtools_start(tooldir,config_namestrcmd,true);                                                             //emit set->findtools_start("C:\\Users\\sku5\\Desktop",AOTO_CMD_CONF,true);
        }
    }
/////////////////// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    connect(set,&setting_corl::findcomplete,this,[=](QStringList Oabspath){  userselectappnum = Oabspath.count();});  //connect(set,&set::findcomplete,this,[](){})   findcomplete(QStringList& Oabspath,true)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    connect(set,&setting_corl::autoinsexeprocess,this,[=](int toolcount,bool cmd_or_man){                                            //true is cmd exe run,false is Human clicked run install exe
        if (!cmd_or_man) return; // false is human clicked app installed                                                                             //skip this singls
        ui->CountAppLcd->display(toolcount+1);
            cmdappcount++;                                                                                                                                              //qDebug()<<"count_apps: "<<cmdappcount<<"toolcount:"<<toolcount<<"toolapplist.count():"<<toolapplist.count();
        if (toolapplist.count()==cmdappcount || userselectappnum==cmdappcount) {
            if (!ui->AutoApp->isEnabled()) {
                ui->AutoApp->setEnabled(true);
                ui->Command_tools->setEnabled(true);
                cmdappcount = 0;
                userselectappnum = 0;
                emit ui->AutoApp->clicked();
            }
        }
    });
}
void MainWindow::ClickAppinstall()
{
    ui->AutoApp->setEnabled(false);
    ui->Command_tools->setEnabled(false);
    QString tooldir,config_namestrclick = AUTO_CLICK_CONF;
    if (ui->Config_bnb->isChecked()) config_namestrclick = BNBAUTO_CLICK_CONF;
    if (ui->Config_cnb->isChecked()) config_namestrclick = CNBAUTO_CLICK_CONF;
    QStringList Toolapplist,argslist,AppWinnamelist;
    QList <QStringList> setuparg_strlist;
    QByteArray readline;
    QFile ConfFile(config_namestrclick);
    if (!UserSelectDir.isEmpty()) tooldir = UserSelectDir;                                  //user make sure tools dir exists
    if (!ConfFile.exists()) QMessageBox::critical(this,"Error","appconfig.txt/xxxappconfig.txt can not find it");
    if (ConfFile.open(QFile::ReadOnly)){
        if(!Find_clickappandnamelist->isEmpty() || !Notoolsfind->isEmpty() || !ToolsWinlist->isEmpty() || !Nosetupsstrlist->isEmpty()){
                Find_clickappandnamelist->clear();
                Notoolsfind->clear();
                ToolsWinlist->clear();
                Nosetupsstrlist->clear();
          }
        while ((readline = ConfFile.readLine()) != nullptr) {
            if (readline.split('#').first().contains(".exe")) {
                QFile Tool_exe(readline.split('#').first().simplified());
                if (Tool_exe.exists()) {
                    Toolapplist.append(readline.simplified());
                    AppWinnamelist.append((readline.split('#').last().simplified()));
                    readline = ConfFile.readLine();
                    if (readline != nullptr && readline.contains("#") ) {
                        if(!argslist.isEmpty()) argslist.clear();
                         foreach (QByteArray var, readline.split('#')) {
                            argslist.append(var.simplified());
                         }
                         setuparg_strlist.append(argslist);
                    }
                }else{
                      Notoolsfind->append(Tool_exe.fileName());
                      ToolsWinlist->append((readline.split('#').last().simplified()));
                      readline = ConfFile.readLine();
                      if (readline != nullptr && readline.contains("#") ) {
                          if(!argslist.isEmpty()) argslist.clear();
                           foreach (QByteArray var, readline.split('#')) {
                               argslist.append(var.simplified());
                           }
                           Nosetupsstrlist->append(argslist);
                      }
                }
            }
        }
        ConfFile.close();
    }

    if (!Toolapplist.isEmpty() && !AppWinnamelist.isEmpty() && !setuparg_strlist.isEmpty()) {
        AutoAppInstalls(Toolapplist,AppWinnamelist,setuparg_strlist);                                                               //Install Oabspath apps
        ui->textEdit->append("<h1><font color=green>Find Oabspath apps </font></h1>");
        foreach (QString toolstrname, Toolapplist) {
            if(toolstrname.isEmpty())continue;
            ui->textEdit->append(toolstrname);
        }
        ui->textEdit->append("<h1><font color=green>Find Oabspath apps </font></h1>");
    }
    if (tooldir.isEmpty()) {
        QMessageBox::information(this,"Tool Find Dir Select","Please Select Tools path");
         tooldir = QFileDialog::getExistingDirectory(this, tr("Open FindTools Dir"),NULL, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (tooldir.isEmpty()) {
            QMessageBox::critical(this,"Tool Path","Please Select Tools path");
            ui->textEdit->append("<h2><font color=red>---Tools can not find It---</font></h2>");
            foreach (QString var, *Notoolsfind) {
                if(var.isEmpty())continue;
                ui->textEdit->append(var);
            }
            ui->textEdit->append("<h2><font color=red>---Tools can not find It---</font></h2>");
            ui->AutoApp->setEnabled(true);
            ui->Command_tools->setEnabled(true);
            return;
         }
    }
    QTimer* findapptoolstime = new QTimer(this);
 //tool only one done
//    foreach (QString var, *Notoolsfind) {
//        if(var.isEmpty())continue;
//        emit set->FindOnlyFile_start(tooldir,var); //Will set Find_clickappandnamelist value
//    }
 //tool only one done
   emit set->FindOnlyFile_start(tooldir,*Notoolsfind);                          //This can have find more tools
    if (!Toolapplist.isEmpty() || !Notoolsfind->isEmpty()) {
        if(!Toolapplist.isEmpty())
          findapptoolstime->start(Toolapplist.count()*10000);
        if( !Notoolsfind->isEmpty())
          findapptoolstime->start(Notoolsfind->count()*10000);
    }else {
        findapptoolstime->start(1000);
    }
    connect(findapptoolstime,&QTimer::timeout,this,[=](){
            if (Find_clickappandnamelist){
                if (!Find_clickappandnamelist->isEmpty()){
                    if (Find_clickappandnamelist) {
                        if (Find_clickappandnamelist->count() == Notoolsfind->count() && Find_clickappandnamelist->count() !=0) {
//                             qDebug()<<"Nomal: Find_clickappandnamelist: "<<*Find_clickappandnamelist;
//                             qDebug()<<"Nosetupsstrlist: "<<*Nosetupsstrlist;
                             ui->textEdit->append("<h1><font color=green>(Find stress)Clicked Tools Find It</font></h1>");
                             foreach (QString toolname, *Find_clickappandnamelist) {
                                 if(toolname.isEmpty())continue;
                                 ui->textEdit->append(toolname);
                             }
                             ui->textEdit->append("<h1><font color=green>(Find stress)Clicked Tools Find It</font></h1>");
                              AutoAppInstalls(*Find_clickappandnamelist,*ToolsWinlist,*Nosetupsstrlist);
                        }else if(Find_clickappandnamelist->count() !=0){
                            QStringList newtoolnamelist,appWinnamelist,newfind_No_tools;
                            bool findit = false;
                            QList <QStringList> newsetupsstrlist;
                            for (int var = 0; var < Notoolsfind->count(); ++var) {
                                for (int j = 0; j < Find_clickappandnamelist->count(); ++j) {
                                    if ((*Find_clickappandnamelist)[j].contains((*Notoolsfind)[var])) {
                                        newtoolnamelist.append((*Find_clickappandnamelist)[j]);
                                        appWinnamelist.append((*ToolsWinlist)[var]);
                                        newsetupsstrlist.append((*Nosetupsstrlist)[var]);
                                        findit = true;
                                        break;
                                    }else{
                                              findit = false;
                                              continue;
                                    }
                                    if (!findit) {
                                        newfind_No_tools.append((*Notoolsfind)[var]);
                                    }
                                }
                            }
                            if (!newfind_No_tools.isEmpty()) {
                                ui->textEdit->append("<h2><font color=red>---(Find stress)Tools can not find It---</font></h2>");
                                foreach (QString toolname, newfind_No_tools) {
                                    if(toolname.isEmpty())continue;
                                    ui->textEdit->append(toolname);
                                }
                                ui->textEdit->append("<h2><font color=red>---(Find stress)Tools can not find It---</font></h2>");
                              }
                            ui->textEdit->append("<h1><font color=green>(Find stress)Clicked Tools Find It</font></h1>");
                            foreach (QString toolname, newtoolnamelist) {
                                if(toolname.isEmpty())continue;
                                ui->textEdit->append(toolname);
                            }
                            ui->textEdit->append("<h1><font color=green>(Find stress)Clicked Tools Find It</font></h1>");
                            //qDebug()<<"Find_clickappandnamelist: "<<*Find_clickappandnamelist;
                            AutoAppInstalls(newtoolnamelist,appWinnamelist,newsetupsstrlist);
                        }
                       findapptoolstime->stop();
                    }
                }else {
                    ui->textEdit->append("<h1><font color=red>Find Process Will Delay Many Times,  Please Waiting........</font></h1>");
                    static int count_times = 1;
                    if(count_times>=10){
                        ui->textEdit->append("<h2><font color=red>---(Find stress)Tools can not find It---</font></h2>");
                        foreach (QString var, *Notoolsfind) {
                            if(var.isEmpty())continue;
                            ui->textEdit->append(var);
                        }
                         ui->textEdit->append("<h2><font color=red>---(Find stress)Tools can not find It---</font></h2>");
                         ui->AutoApp->setEnabled(true);
                         ui->Command_tools->setEnabled(true);
                         findapptoolstime->stop();
                         count_times = 1;
                      }
                        count_times++;
                }
            }else   findapptoolstime->stop();
    });
}
/*
*Arguments:
* SetupWinnamelist: Is App chlick install windows name with spy++ finded name.
*ToolSetupName: The name is tools name.(exp:HWQA.exe,xxx.exe....)
*SetupArgslist: The chlicked with buttons name.(exp:next,next,Yes....)
*/
void MainWindow::AutoAppInstalls(const QStringList& ToolSetupName,const QStringList& SetupWinnamelist,const QList <QStringList>& SetupArgslist)
{
        static int delayinstalltime = 0;
        static int count_apps = 0;
       QTimer *appinstalltimer=new QTimer(this);
       if (!ToolSetupName.isEmpty()) {
           appinstalltimer->start(1000);
           emit set->autoexe_setupstart(ToolSetupName,false);                                                                            //emit app install setup  first
       }
connect(set,&setting_corl::autoinsexeprocess,this,[=](int toolcount,bool cmd_or_man){                     //true is cmd exe run,false is man clicked run install exe
           if (cmd_or_man) return;                                                                                                                           //skip this singls
           ui->CountAppLcd->display(toolcount+1);                                                                                         //qDebug()<<"count_apps: "<<count_apps<<"toolcount:"<<toolcount<<"toolapplist.count():"<<toolapplist.count();
           if (ToolSetupName.count()==++count_apps) {
               appinstalltimer->stop();
               ui->AutoApp->setEnabled(true);
               ui->Command_tools->setEnabled(true);
               count_apps = 0;
               ui->AppLcd->display(0);
           }
       });
connect(appinstalltimer,&QTimer::timeout,this,[=](){
           if (delayinstalltime == 10) {
               delayinstalltime = 0;                                                                                                                //qDebug()<<"count_apps:  "<<count_apps<<"ToolSetupName.count():  "<<ToolSetupName.count();
               if(count_apps >= SetupWinnamelist.count())
               {
                   appinstalltimer->stop();
                   return;
               }
               emit set->autoinstallapp_start(SetupWinnamelist[count_apps],SetupArgslist[count_apps]); //installed send window's singals
        }
           ui->AppLcd->display(delayinstalltime++);
       });
}
bool MainWindow::RunAsAdmin()
{
    BOOL Admistatus;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;
    Admistatus = AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &AdministratorsGroup);
    if(Admistatus)
    {
        if (!CheckTokenMembership( NULL, AdministratorsGroup, &Admistatus))
        {
             Admistatus = false;
        }
        FreeSid(AdministratorsGroup);
    }
    return(Admistatus);
}
void MainWindow::SetInFo2Bios()
{
  QFileInfo oldfilepath(OLDBIOS_CONFIG_FILE);
  QString filename;
  if(oldfilepath.exists()){
      QMessageBox msgBox(this);
      msgBox.setWindowTitle("Set Bios Infomation ");
      msgBox.setText("Use Default File:    "+oldfilepath.absoluteFilePath());
      msgBox.setInformativeText("Do you want to Setting?");
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      msgBox.setDefaultButton(QMessageBox::Yes);
      int ret = msgBox.exec();
      switch (ret) {
         case QMessageBox::Yes:    break;
         case QMessageBox::No:{
            oldfilepath.setFile(QFileDialog::getOpenFileName(this, tr("Open File(*.log/.txt)"),"C:\\", tr("LOG(*.log *.txt)")));
            if(!oldfilepath.exists()) return;
          }break;
        default:
          break;
        }
    }else {
           QMessageBox::information(this,"BCU Set Config File","Open Config File Must be *.log Or *.txt");
           oldfilepath.setFile(QFileDialog::getOpenFileName(this, tr("Open File(*.log/.txt)"),"C:\\", tr("LOG(*.log *.txt)")));
           if(!oldfilepath.exists()) return;
  }
    if (Bcu_ToolPath.isEmpty()) {
            QMessageBox::information(this,"BCU Set Config File","Open Setting Tool Must be BIOS Pakge (BiosConfigUtility) *.exe");
            filename = QFileDialog::getOpenFileName(this, tr("Open File(BiosConfigUtility)"),"C:\\", tr("EXE (*.exe)"));
            Bcu_ToolPath=filename;
    }
    if(Bcu_ToolPath.isEmpty() || !Bcu_ToolPath.contains("BiosConfigUtility")) {
       QMessageBox::critical(this,"Error","Please input BiosConfigUtility/BCU EXE File");
       return ;
    }
  //User Select MPM Lock Or UnLock
 /*
        Manufacturing Programming Mode
            *Unlock
            Lock
        Manufacturing Programming Mode
                Unlock
                *Lock
 */
 {
      QMessageBox msgBox(this);
      msgBox.setWindowTitle("MPM");
      msgBox.setText("Lock Or Unlock");
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      msgBox.setDefaultButton(QMessageBox::Yes);
      msgBox.setButtonText(QMessageBox::Yes,QString("Lock"));
      msgBox.setButtonText(QMessageBox::No,QString("Unlock"));
      int ret = msgBox.exec();
      switch (ret) {
         case QMessageBox::Yes:{
                  QFile Infofile(oldfilepath.absoluteFilePath());
                  QByteArray Lineinfo,Rtmplineinfo,Wtmplineinfo;
                  qint64 fileposstion;
                  if(Infofile.open(QFile::ReadWrite)){
                      while ( !(Lineinfo = Infofile.readLine()).isEmpty() ) {
                            if (Lineinfo.contains("Manufacturing Programming Mode")) {
                                fileposstion = Infofile.pos();
                                Rtmplineinfo = Infofile.readLine();
                                Rtmplineinfo += Infofile.readLine();
                                if(Rtmplineinfo=="\t*Unlock\r\n\tLock\r\n")
                                      Wtmplineinfo = "\tUnlock\r\n\t*Lock\r\n";
                                else if(Rtmplineinfo=="\tLock\r\n\t*Unlock\r\n")
                                           Wtmplineinfo = "\t*Lock\r\n\tUnlock\r\n";
                                        else break;
                                if(Wtmplineinfo.size()==Rtmplineinfo.size()){
                                    Infofile.seek(fileposstion);
                                    Infofile.write(Wtmplineinfo);
                                  }
                                break;
                            }
                     }
                }
           Infofile.close();
          }break;
         case QMessageBox::No:{
                  QFile Infofile(oldfilepath.absoluteFilePath());
                  QByteArray Lineinfo,Rtmplineinfo,Wtmplineinfo;
                  qint64 fileposstion;
                  if(Infofile.open(QFile::ReadWrite)){
                      while ( !(Lineinfo = Infofile.readLine()).isEmpty() ) {
                            if (Lineinfo.contains("Manufacturing Programming Mode")) {
                                fileposstion = Infofile.pos();
                                Rtmplineinfo = Infofile.readLine();
                                Rtmplineinfo += Infofile.readLine();
                                if(Rtmplineinfo=="\tUnlock\r\n\t*Lock\r\n")
                                      Wtmplineinfo = "\t*Unlock\r\n\tLock\r\n";
                                else if (Rtmplineinfo=="\t*Lock\r\n\tUnlock\r\n")
                                           Wtmplineinfo = "\tLock\r\n\t*Unlock\r\n";
                                        else break;
                                if(Wtmplineinfo.size()==Rtmplineinfo.size()){
                                    Infofile.seek(fileposstion);
                                    Infofile.write(Wtmplineinfo);
                                  }
                                 break;
                            }
                     }
                }
           Infofile.close();
          }break;
        default:
          break;
        }
 }
     QString outinfo = "Command Execue Succfully! return value:  %1   File Use: "+oldfilepath.absoluteFilePath().toUtf8();
     QMessageBox::information( this,"Set Bios Informations",outinfo.arg( system(Bcu_ToolPath.toUtf8()+"   -setconfig:"+oldfilepath.absoluteFilePath().toUtf8()) ) );     //Return value 11/13    file can not exsite /MPM locked
     QMessageBox msgBox(this);
     msgBox.setWindowTitle("Reboot System ? ");
     msgBox.setText("Do you want to Reboot?");
     msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
     msgBox.setDefaultButton(QMessageBox::Yes);
     msgBox.setButtonText(QMessageBox::Yes,QString("Reboot"));
     msgBox.setButtonText(QMessageBox::No,QString("Cancel"));
     int ret = msgBox.exec();
     switch (ret) {
        case QMessageBox::Yes: system("shutdown -r -t 0");    break;
        case QMessageBox::No:  break;
       default:
         break;
       }
}
void MainWindow::SmcAppClicked(bool Humancheck)
{
      ui->dock_information->setWindowIconText(BIOS_CONFIG_FILE);
      QString backupname;
      QFile mpmfile(BIOS_CONFIG_FILE),Backupfile(OLDBIOS_CONFIG_FILE);
      QByteArray mpminfo;
      qDebug()<<Humancheck;
      if (!Humancheck) {
          QString filename;
          if (Bcu_ToolPath.isEmpty()) {
                  filename = QFileDialog::getOpenFileName(this, tr("Open File"),"C:\\", tr("EXE (*.exe)"));
                  Bcu_ToolPath=filename;
          }
          if(Bcu_ToolPath.isEmpty()) {
             QMessageBox::critical(this,"Error","Please input BiosConfigUtility/BCU EXE File");
             return ;
          }
          if(!Bcu_ToolPath.contains("BiosConfigUtility")){
              QMessageBox::critical(this,"Error","Please  check file name is contains BiosConfigUtility");
              return ;
          }
          if (mpmfile.exists())
              mpmfile.remove();
          if (!system(Bcu_ToolPath.toUtf8()+"   -getconfig:"+BIOS_CONFIG_FILE)){
              if (!mpmfile.exists()) {
                 QMessageBox::critical(this,"Error","Can find file: "+QString::fromUtf8(BIOS_CONFIG_FILE));
                 return ;
              }
          }
          QMessageBox msgBox(this);
          msgBox.setWindowTitle("Open App ");
          msgBox.setText("HPSMC.exe");
          msgBox.setInformativeText("Cancel Will Only Read  Bios Informaion");
          msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
          msgBox.setDefaultButton(QMessageBox::Yes);
          msgBox.setButtonText(QMessageBox::Yes,QString("Open"));
          msgBox.setButtonText(QMessageBox::No,QString("Cancel"));
          int ret = msgBox.exec();
          switch (ret) {
             case QMessageBox::Yes: {
                   emit set->setting_exe_run(RUN_OTHERAPP);
                  }break;
             case QMessageBox::No:break;
              default:
              break;
            }
      }else {
                  emit set->setting_exe_run(RUN_OTHERAPP);
                  ui->dock_information->setWindowTitle(BIOS_CONFIG_FILE);
                  ui->dock_information->setHidden(false);
                  ui->dock_information->show();
                  ui->dock_information->setMinimumSize(QSize(400,600));
           }
    if(mpmfile.open(QFile::ReadOnly)){
          if( mpmfile.copy(OLDBIOS_CONFIG_FILE) )
            ui->textEdit->append("<h2><font color=green>Backup Sucessfully</font></h2>");
          else{
                 mpmfile.remove(OLDBIOS_CONFIG_FILE);
                 backupname = QFileDialog::getSaveFileName(this,tr("Save backup File"),OLDBIOS_CONFIG_FILE,tr("txt (*.txt *.log)"));
                 QFile saveother(backupname);
                 if(saveother.exists())
                     saveother.moveToTrash();
                 if(!backupname.isEmpty())
                   if(!mpmfile.copy(backupname))
                     ui->textEdit->append("<h2><font color=red>Save File:\t"+backupname+"\tFailed</font></h2>");
            }
          ui->Unit_info->clearHistory();
          ui->Unit_info->clear();
          ui->Unit_info->setTextColor(QColor::fromRgb(QRandomGenerator::global()->generate()).name(QColor::HexArgb));
          if(!backupname.isEmpty()) Backupfile.setFileName(backupname);
          if(Backupfile.open(QFile::ReadOnly)){
            QClipboard *clipboard = QApplication::clipboard();                                                 //Clipboard copy and paste
            while ( !(mpminfo = Backupfile.readLine()).isEmpty() ) {
              if ( mpminfo.contains("Integrated MAC Address")||mpminfo.contains("ME Firmware Version")||mpminfo.contains("System Board ID")||
                    mpminfo.contains("System BIOS Version")||mpminfo.contains("Product Name")||mpminfo.contains("SKU Number")|| mpminfo.contains("Universally Unique Identifier (UUID)")) {
                    ui->Unit_info->append(mpminfo);
                    if (mpminfo.contains("Universally Unique Identifier (UUID)")) {
                        clipboard->setText(Backupfile.readLine());
                        ui->Unit_info->append(clipboard->text());
                      }
                     ui->Unit_info->append(Backupfile.readLine());
                }
              if (mpminfo.contains("Manufacturing Programming Mode")) {
                  ui->Unit_info->append(mpminfo);
                  ui->Unit_info->append(Backupfile.readLine());
                  ui->Unit_info->append(Backupfile.readLine());
              }
           }
       } else ui->textEdit->append("<h3><font color=red>Backup_hwqa_bios.log Backup File Or Can't Open Backupack_hwqa_bios.log</font></h3>");
    }
    mpmfile.close();
    Backupfile.close();
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
   //qDebug()<<event;
    switch (event->key()) {
    case Qt::Key_A:
    {
        bool check = true;
        ui->AutoRun->isChecked() ? check = false : true;
        ui->AutoRun->setChecked(check);
        emit ui->AutoRun->clicked(check);
    }break;
    case Qt::Key_B:
    {
        bool check = true;
        ui->Config_bnb->isChecked()? check = false : true;
        ui->Config_bnb->setChecked(check);
    }break;
    case Qt::Key_C:
    {
        bool check = true;
        ui->Config_cnb->isChecked()? check = false : true;
        ui->Config_cnb->setChecked(check);
    }break;
    case Qt::Key_D:
    {
        bool check = true;
        ui->Config_def->isChecked()? check = false : true;
        ui->Config_def->setChecked(check);
    }break;
    case Qt::Key_F:
    {
        bool check = true;
        ui->FindFileBox->isChecked()? check = false : true;
        ui->FindFileBox->setChecked(check);
    }break;
    case Qt::Key_T:
    {
        emit ui->Command_tools->clicked();
    }break;
    case Qt::Key_O:
    {
        if (!another->isActiveWindow()){
            another->setWindowTitle("Other Function Will Development");
            another->show(); 
            another->setFocus();
        }
    }break;
    case Qt::Key_Q:
    case Qt::Key_E:
        close();
        break;
    default:
        break;
    }
   return QWidget::keyPressEvent(event);
}
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
    {
    }break;
    case Qt::Key_B:
    {
    }break;
    case Qt::Key_C:
    {
    }break;
    case Qt::Key_D:
    {
    }break;
    case Qt::Key_E:
    {
    }break;
    case Qt::Key_F:
    {
    }break;
    case Qt::Key_G:
    {
    }break;
    case Qt::Key_H:
    {
    }break;
    default:
        break;
    }
    return QWidget::keyReleaseEvent(event);
}
