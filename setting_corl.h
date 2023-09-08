#ifndef SETTING_CORL_H
#define SETTING_CORL_H

#include <QObject>
#include <QThread>

class setting_corl:public QObject
{
    Q_OBJECT
public:
    setting_corl(QObject *parent = nullptr);
    ~setting_corl();
    const QString* getthredmsg(){return thredmsg;};
signals:
    void setting_start(bool start);
    void setting_done(QList <QString >*);

    void setting_startMpm(QString filename,QString bcu_path);
    void setting_Me_status(bool Me,QString err_info);
    void setting_exe_run(QString appnamepath);

    void findtools_start(QString userseldir,QString confile,bool human = true);
    void findcomplete(const QStringList& Oabspath,bool human = true);
    void NoToolReturn(const QStringList& Oabspath,bool human = true);
    void FindOnlyFile_start(QString userseldir,QString toolname);
    void FindOnlyFile_start(QString userseldir,QStringList ListToolName);
    void FindOnlyFile_complete(const QString& Oabspath_file);
    void FindOnlyFile_complete(const QStringList& ListOabspath_file);

    void autoexe_setupstart(const QStringList& applist,bool cmd_or_man);
    void autoinstallapp_start(const QString& win_name,const QStringList& setuplist);
    void autoinsexeprocess(int appcount,bool cmd_or_man);
public slots:
    void setting_complete(QList <QString >*);
    void setthredmsg(const QString& msg){*thredmsg=msg;};
private:
    QThread*  setting_corlthread;
    QThread*  setting_mpmthread;
    QThread*  setting_exeappthread;
    QThread*  findtoolsthread;
    QThread*  exesetupthread;
    QThread*  installappthread;
 private:
    QString* thredmsg;
};

#endif // SETTING_CORL_H
