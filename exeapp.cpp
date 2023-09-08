#include "exeapp.h"
#include "QFile"
#include "qprocess.h"
#include "qtemporarydir.h"
EXEAPP::EXEAPP(QObject *parent)
    : QObject{parent}
{

}
EXEAPP::~EXEAPP()
{
}
void EXEAPP::exeappRun(QString exeapp)
{
    QTemporaryDir exetemp;
    QString smcfile;
    QString hpsmc=exeapp;
    QString exetemppath;
    if(exeapp.contains(":/")){
        if(exetemp.isValid()) {
            exetemppath=exetemp.path();
        }
            smcfile=exetemppath+exeapp.replace(":/","/");
            QFile exedst=(smcfile);
            QFile exesrc=(hpsmc);
          if(exedst.open(QIODevice::WriteOnly)){
              if(exesrc.open(QIODevice::ReadOnly)){
                  QByteArray tmp = exesrc.readAll();
                  exedst.write(tmp);
              }
          }
          exedst.close();
          exesrc.close();
    }else  smcfile=exeapp;
      QProcess phpsmc(this);
      phpsmc.setProgram(smcfile);
      phpsmc.execute(smcfile);
}
