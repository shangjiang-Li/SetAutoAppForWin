#include "setting_corl.h"
#include "windows_function.h"
#include "mpm.h"
#include "findtools.h"
#include "autoinstalls.h"
#include "exeapp.h"

setting_corl::setting_corl(QObject *parent)
 : QObject(parent),
   setting_corlthread (new QThread(this)),
   setting_mpmthread ( new QThread(this)),
   setting_exeappthread (new QThread(this)),
   findtoolsthread (new QThread(this)),
   exesetupthread (new QThread(this)),
   installappthread (new QThread(this)),         //create thread
   thredmsg(new QString)
{
    Windows_function * pwindows_function = new Windows_function;
    pwindows_function->moveToThread(setting_corlthread);
    void (setting_corl::*p_settingstart)(bool start) = &setting_corl::setting_start;
    void (Windows_function::*p_work)() = &Windows_function::setting_work;
    void (setting_corl::*p_settingdone)(QList <QString >* done) = &setting_corl::setting_done;
    void (setting_corl::*p_settingcomplete)(QList <QString >* information) = &setting_corl::setting_complete;                                                                   //

    connect(this,p_settingstart,pwindows_function,p_work,Qt::QueuedConnection/*BlockingQueuedConnection*//*DirectConnection*//*QueuedConnection*/);
    connect(pwindows_function,&Windows_function::setting_complemt,this,p_settingcomplete);
    connect(pwindows_function,&Windows_function::setting_complemt,this,p_settingdone);                                                   //信号向上传递
    setting_corlthread->start();

    MPM *p_mpm = new MPM;
    p_mpm->moveToThread(setting_mpmthread);
    void (setting_corl::*p_Mpm_start)(QString filename,QString bcu_path)=&setting_corl::setting_startMpm;
    void (MPM::*p_mpmwork)(QString filename,QString bcu_path)=&MPM::mpmwork;
    void (setting_corl::*p_Mestaus)(bool me,QString err_info)=&setting_corl::setting_Me_status;

    connect(this,p_Mpm_start,p_mpm,p_mpmwork,Qt::QueuedConnection);
    connect(p_mpm,&MPM::MpmMe_status,this,p_Mestaus);                              //信号向上传递
    setting_mpmthread->start();

    EXEAPP *pexeapp = new EXEAPP;
    pexeapp->moveToThread(setting_exeappthread);
    void (setting_corl::*pexeapprun)(QString runname) = &setting_corl::setting_exe_run;
    void (EXEAPP::*pexeappwork)(QString appname) = &EXEAPP::exeappRun;

    connect(this,pexeapprun,pexeapp,pexeappwork,Qt::QueuedConnection);
    setting_exeappthread->start();
    /*
     *
     * find tools function and auto install all tools
     *
    */
    FindTools* pfindtools = new FindTools;
    pfindtools->moveToThread(findtoolsthread);
    void (setting_corl::* pfindtools_start)(QString userseldir,QString confile,bool human) = &setting_corl::findtools_start;
    void (setting_corl::* pthredfindcomplete)(const QStringList& Oabspath,bool human) = &setting_corl::findcomplete;
    void (setting_corl::* pNoToolReturn)(const QStringList& Oabspath,bool human) = &setting_corl::NoToolReturn;
    void (FindTools::* pfinduserseldir)(QString userseldir,QString confile,bool human) = &FindTools::finduserseldir;
    void (FindTools::* pfindcomplete)(const QStringList& Oabspath,bool human) = &FindTools::findcomplete;
    void (FindTools::* pFindNoTools)(const QStringList& Oabspath,bool human) = &FindTools::FindNoTools;
    connect(this,pfindtools_start,pfindtools,pfinduserseldir,Qt::QueuedConnection);

    void (setting_corl::* pFindOnlyFile_start)(QString userseldir,QString toolname) = &setting_corl::FindOnlyFile_start;
    void (setting_corl::* pFindOnlyFile_startList)(QString userseldir,QStringList ListToolName) = &setting_corl::FindOnlyFile_start;
    void (setting_corl::* pFindOnlyFile_complete)(const QString& Oabspath_file) = &setting_corl::FindOnlyFile_complete;
    void (setting_corl::* pFindOnlyFile_completeList)(const QStringList& Oabspath_file) = &setting_corl::FindOnlyFile_complete;
    void (FindTools::* pOnlyFindFiledone)(const QString& Oabspath_file) = &FindTools::OnlyFindFiledone;
    void (FindTools::* pOnlyFindFiledoneList)(const QStringList& ListOabspath_file) = &FindTools::OnlyFindFiledone;
    void (FindTools::* pOnlyFindFileThread)(QString userseldir,QString toolname) = &FindTools::OnlyFindFileThread;
    void (FindTools::* pOnlyFindFileThreadList)(QString userseldir,QStringList ListToolName) = &FindTools::OnlyFindFileThread;
    connect(this,pFindOnlyFile_start,pfindtools,pOnlyFindFileThread,Qt::QueuedConnection);
    connect(this,pFindOnlyFile_startList,pfindtools,pOnlyFindFileThreadList,Qt::QueuedConnection);

    findtoolsthread->start();

    Exesetup* pexesetup = new Exesetup;
    pexesetup->moveToThread(exesetupthread);
    void (setting_corl::* pautoapp_setupstart)(const QStringList& applist,bool commad_toollist) = &setting_corl::autoexe_setupstart;
    void (Exesetup::* pautoexeinstall)(const QStringList& applist,bool commad_toollist) = &Exesetup::exeinstall;
    void (Exesetup::* pexecomplte)(int countapp,bool cmd_or_man) = &Exesetup::execomplte;                                                             //true is cmd exe run,false is Human clicked run install exe
    void (Exesetup::* pmsgcomplte)(const QString& msg) = &Exesetup::execomplte;                                                                              //Singal overload
    void (setting_corl::* pautoinsexeprocess)(int appcount,bool cmd_or_man)= &setting_corl::autoinsexeprocess;                             //
    void (setting_corl::* psetthredmsg)(const QString& msg)= &setting_corl::setthredmsg;                                                                   //

    connect(this,pautoapp_setupstart,pexesetup,pautoexeinstall,Qt::QueuedConnection);                                                                          //exe app first for run
    connect(pexesetup,pexecomplte,this,pautoinsexeprocess);
    connect(pexesetup,pmsgcomplte,this,psetthredmsg);
    exesetupthread->start();

    AutoInstalls* pappinstall_run = new AutoInstalls;
    pappinstall_run->moveToThread(installappthread);
    void (setting_corl::* pappinstallstart)(const QString& win_name,const QStringList& setuplist) = &setting_corl::autoinstallapp_start;
    void (AutoInstalls::* pappautoinstall)(const QString& win_name,const QStringList& setuplist) = &AutoInstalls::appautoinstall;

    connect(this,pappinstallstart,pappinstall_run,pappautoinstall,Qt::QueuedConnection);                   //install action
    installappthread->start();

    /*NEW---------------------------Find and install------------------------------------NEW*/
    connect(pfindtools,pfindcomplete,this,pthredfindcomplete);
    connect(pfindtools,pFindNoTools,this,pNoToolReturn);
    connect(pfindtools,pfindcomplete,pexesetup,pautoexeinstall);                  //thread start to another thread
 //---------------------------------------Find file only thread--------------------------------------------------//
    connect(pfindtools,pOnlyFindFiledone,this,pFindOnlyFile_complete);
    connect(pfindtools,pOnlyFindFiledoneList,this,pFindOnlyFile_completeList);
 //---------------------------------------Find file only thread--------------------------------------------------//
 /*NEW---------------------------Find and install------------------------------------NEW*/
}
setting_corl::~setting_corl()
{
    setting_corlthread->quit();
    setting_corlthread->wait();
    delete setting_corlthread;

    setting_mpmthread->quit();
    setting_mpmthread->wait();
    delete setting_mpmthread;

    setting_exeappthread->quit();
    setting_exeappthread->wait();
    delete setting_exeappthread;

    findtoolsthread->quit();
    findtoolsthread->wait();
    delete findtoolsthread;

    exesetupthread->quit();
    exesetupthread->wait();
    delete exesetupthread;

    installappthread->quit();
    installappthread->wait();
    delete installappthread;

   delete thredmsg;
}
void setting_corl::setting_complete(QList <QString >* information){
    if(!thredmsg->isEmpty()) thredmsg->clear();
    foreach (QString str, *information) {
        thredmsg->append(str);
    }
}
