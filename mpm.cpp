#include "mpm.h"
#include "QFile"
#include "QDir"
#include "QFileInfo"
#include "QByteArray"
#include "QProcess"

MPM::MPM(QObject *parent)
    : QObject{parent}
{
}
MPM::~MPM()
{
}
void MPM::mpmwork(QString filename,QString bcu_path){

    QString Metoolpath("METools/MEInfo/WINDOWS64");
    QString Metoolinfoname("MEINFO.LOG");
    QString Metoolcmd("/MEInfoWin64.exe   -feat  \"NVAR Configuration State\" >"+Metoolinfoname);
    QString Mecheckstr("NVAR Configuration State: Locked");
    QString Mecscript("/CLSMNF64.BAT");
    QString Mecscriptdir("METools/Batch");
    QString Mpmcheckstr("*Lock");
    QString Mmpcmd("  -setconfig:");
    QString Mpmopenexe("notepad    ");

    QFileInfo bcu(bcu_path),mpmpath(filename);
    QDir bcudir(bcu.absolutePath()),tmpdir;
    bcudir.cdUp();
    tmpdir=bcudir;
    bcudir.cd(Metoolpath);
    QFile mpmfile(filename),Mefile(Metoolinfoname);;
    QByteArray mpminfo,meinfo;
    bool ME_status=false;
    if(!system(bcudir.absolutePath().toUtf8()+Metoolcmd.toUtf8())){
        if(Mefile.open(QFile::ReadOnly)){
             meinfo=Mefile.readAll();
            if(meinfo.contains(Mecheckstr.toUtf8()) && meinfo!=nullptr ){
                ME_status=true;
            }
       }
        Mefile.close();
     }
    if (!mpmfile.exists()) {
       emit MpmMe_status(ME_status,"Can not open or have no permission: "+filename);
    }
    if(mpmfile.open(QFile::ReadOnly)){
         mpminfo=mpmfile.readAll();
         if( mpminfo!=nullptr && mpminfo.contains(Mpmcheckstr.toUtf8())){
                mpmfile.close();
                emit MpmMe_status(ME_status,"MPM Locked");
              }else if(ME_status){
                             if(!system(Mpmopenexe.toUtf8()+filename.toUtf8())){
                                 if(!system(bcu_path.toUtf8()+Mmpcmd.toUtf8()+mpmpath.absoluteFilePath().toUtf8())){
                                     emit MpmMe_status(ME_status,"Reboot and MPM Lock if you set Lock");
                                  }
                               }
               }else if(!mpminfo.isEmpty()){
                         bcudir=tmpdir;
                         bcudir.cd(Mecscriptdir);
                         QProcess meprocess(this);
                         meprocess.setWorkingDirectory(bcudir.absolutePath().toUtf8());                             //Other script workdir and environment
                         QString tempdir=QDir::current().absolutePath();
                         QDir::setCurrent(bcudir.absolutePath().toUtf8());
                         meprocess.startDetached(bcudir.absolutePath().toUtf8()+Mecscript.toUtf8());        //System will reboot and lock ME
                         QDir::setCurrent(tempdir);
                         emit MpmMe_status(ME_status,"MPM Unlocked-------<h1><font color=red>Waitting System Reboot</font></h1>--------");
                       }else emit MpmMe_status(ME_status,"Can not open or have no permission: "+filename);
    }
}


