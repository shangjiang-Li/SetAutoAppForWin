#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <qdebug.h>
#include "findtools.h"


FindTools::FindTools(QObject *parent)
    : QObject{parent},Oabspath(new QStringList),PathNoTool(new QStringList)
{
}
FindTools::~FindTools()
{
  delete Oabspath;
  delete PathNoTool;
}
void FindTools::finduserseldir(QString userseldir, QString confile, bool human)
{
    if(!Oabspath->isEmpty())Oabspath->clear();
    static QRegularExpression reg(".*\\\\");
    QString readline;
    QString userselpath;
    QFile ConfFile(confile);
    if (ConfFile.open(QFile::ReadOnly))
    while ((readline = ConfFile.readLine()) != nullptr) {
        QString tempbk = readline;
        if (readline.contains("#")) {
            if (readline.contains("\\")) {
                userselpath = readline.replace(reg,userseldir+"\\");
                QString outstring = findtoolsabspath(userseldir,readline.split('#').first().replace(reg,""));
                if( !outstring.isEmpty()){
                    Oabspath->append(outstring+"#"+readline.split('#').last());
                     continue;
                }else {
                    QFile Tool_existsconf(tempbk.split('#').first());
                    if (Tool_existsconf.exists()) {
                        Oabspath->append(tempbk);
                        continue;
                    }else {
                        if (!userselpath.isEmpty()) PathNoTool->append(userselpath);
                        continue;   //tools  can not find with configure file and user select Dir
                    }
                }
            }else {
                QString outstring = findtoolsabspath(userseldir,readline.split('#').first());
                if( !outstring.isEmpty() ){
                    Oabspath->append(outstring+"#"+readline.split('#').last());
                    continue;
                }else {
                    QFile Tool_existsconf(tempbk.split('#').first());
                    if (Tool_existsconf.exists()) {
                        Oabspath->append(tempbk);
                        continue;
                    }else {
                        if(!human) if(readline.split("#").count()>2)continue;                 /////////////////////////skip click app arguments
                        if (!tempbk.isEmpty()) PathNoTool->append(tempbk);
                        continue;   //tools  can not find with configure file and user select Dir
                    }
                }
            }
        }else{
            if (!tempbk.isEmpty()) PathNoTool->append(tempbk);
                continue;   //skip can not contains("#") strings
        }
     }
    if (!PathNoTool->isEmpty()) {
           emit FindNoTools(*PathNoTool,human);
           PathNoTool->clear();
           }
    if (Oabspath->isEmpty()) {
           emit FindNoTools(*PathNoTool,human);
           PathNoTool->clear();
           return;
           }
    if (human) {
        emit findcomplete(*Oabspath,human);
        Oabspath->clear();
    }else {
        emit findcomplete(*Oabspath,human);// find for find tools app clicked
        Oabspath->clear();
    }
}
const QString FindTools::findtoolsabspath(QString userseldir,QString toolname)
{
    QDir dir_tool(userseldir);
    QString outstirng;
  //------------------------------------------------------------------------------------------------------------------
    dir_tool.setFilter( QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Files);
    if (!dir_tool.entryInfoList().isEmpty()) {
        foreach (QFileInfo var, dir_tool.entryInfoList()) {
            if (toolname.simplified() == var.fileName().simplified())
                   return var.absoluteFilePath();
        }
    }
 //------------------------------------------------------------------------------------------------------------------
    dir_tool.setFilter( QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Dirs); //dir
    if(!dir_tool.entryInfoList().isEmpty()){
        foreach (QFileInfo var, dir_tool.entryInfoList()) {
            if( (outstirng = findtoolsabspath(var.canonicalFilePath(),toolname)) != nullptr )return outstirng;
        }
    }else {
               dir_tool.setFilter( QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Files);
               if (!dir_tool.entryInfoList().isEmpty()) {
                   foreach (QFileInfo var, dir_tool.entryInfoList()) {
                       if (toolname.simplified() == var.fileName().simplified())
                                 return var.absoluteFilePath();
                   }
               }
    }
    return nullptr;
}
void FindTools::OnlyFindFileThread(QString userseldir,QString toolname)
{
     emit OnlyFindFiledone(findtoolsabspath(userseldir,toolname));
}
void FindTools::OnlyFindFileThread(QString userseldir,QStringList ListToolName)
{
  QStringList FindItlist;
  QString tempfindname = NULL;
  foreach (QString var, ListToolName) {
         if(!tempfindname.isEmpty()) tempfindname.clear();
          tempfindname = findtoolsabspath(userseldir,var);
        if(!tempfindname.isEmpty()) FindItlist.append(tempfindname);
    }
      emit OnlyFindFiledone(FindItlist);
}









