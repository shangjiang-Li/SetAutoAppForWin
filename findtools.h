#ifndef FINDTOOLS_H
#define FINDTOOLS_H

#include <QObject>
#include <QFile>

class FindTools : public QObject
{
    Q_OBJECT
public:
    explicit FindTools(QObject *parent = nullptr);
    virtual ~FindTools();
    const QString  findtoolsabspath(QString userseldir,QString toolname);
public slots:
        void finduserseldir(QString userseldir, QString confile, bool human=true);
        void OnlyFindFileThread(QString userseldir,QString toolname);
        void OnlyFindFileThread(QString userseldir,QStringList ListToolName);
signals:
        void FindNoTools(const QStringList& Oabspath,bool human = true);
        void findcomplete(const QStringList& Outabspath,bool human);
        void OnlyFindFiledone(const QString& Oabspath_file);
        void OnlyFindFiledone(const QStringList& Oabspath_file);
 private:
        QStringList* Oabspath;
        QStringList* PathNoTool;
};

#endif // FINDTOOLS_H
