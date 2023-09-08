#include "other_mainwindow.h"
#include "qevent.h"
#include "ui_other_mainwindow.h"
#include<windows.h>
#include<tchar.h>
#include <QFileSystemModel>


Other_MainWindow::Other_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Other_MainWindow)
{
    ui->setupUi(this);
    QFileSystemModel *model = new QFileSystemModel;
    ui->treeView->setModel(model);
    model->setRootPath(QDir::currentPath());
    ui->treeView->setModel(model);
    ui->Play_list->setModel(model);
    ui->Data_display->setStyleSheet("background-color:#C5FFCB");
    connect(ui->Play_list,&QListView::clicked,ui->treeView,&QTreeView::setRootIndex);//connect(ui->treeView,&QTreeView::clicked,ui->Play_list,&QListView::setRootIndex);
    //connect(ui->treeView,&QTreeView::)
    QString dataplay = "\
            天有不测风云，人有旦夕祸福。\
            \
            蜈蚣百足，行不及蛇；雄鸡两翼，飞不过鸦。\
            \
            马有千里之程，无骑不能自往；人有冲天之志，非运不能自通。\
            \
            盖闻，人生在世，富贵不能淫，贫贱不能移。\
            \
            文章盖世，孔子厄于陈邦；武略超群，太公钓于渭水。颜渊命短，殊非凶恶之徒；盗跖年长，岂是善良之辈。尧帝明圣，却生不肖之儿；瞽叟愚顽，反生大孝之子。\
            \
            张良原是布衣，萧何称谓县吏。晏子身无五尺，封作齐国宰相；孔明卧居草庐，能作蜀汉军师。楚霸虽雄，败于乌江自刎；汉王虽弱，竟有万里江山。李广有射虎之威，到老无封；冯唐有乘龙之才，一生不遇。韩信未遇之时，无一日三餐，及至遇行，腰悬三尺玉印，一旦时衰，死于阴人之手。\
            \
            有先贫而后富，有老壮而少衰。满腹文章，白发竟然不中；才疏学浅，少年及第登科。深院宫娥，运退反为妓妾；风流妓女，时来配作夫人。\
            \
            青春美女，却招愚蠢之夫；俊秀郎君，反配粗丑之妇。蛟龙未遇，潜水于鱼鳖之间；君子失时，拱手于小人之下。衣服虽破，常存仪礼之容；面带忧愁，每抱怀安之量。时遭不遇，只宜安贫守份；心若不欺，必然扬眉吐气。初贫君子，天然骨骼生成；乍富小人，不脱贫寒肌体。\
            \
            天不得时，日月无光；地不得时，草木不生；水不得时，风浪不平；人不得时，利运不通。注福注禄，命里已安排定，富贵谁不欲？人若不依根基八字，岂能为卿为相？\
            \
            吾昔寓居洛阳，朝求僧餐，暮宿破窖，思衣不可遮其体，思食不可济其饥，上人憎，下人厌，人道我贱，非我不弃也。今居朝堂，官至极品，位置三公，身虽鞠躬于一人之下，而列职于千万人之上，有挞百僚之杖，有斩鄙吝之剑，思衣而有罗锦千箱，思食而有珍馐百味，出则壮士执鞭，入则佳人捧觞，上人宠，下人拥。人道我贵，非我之能也，此乃时也、运也、命也。\
            ";
    ui->Data_display->append(dataplay.replace("。","。<br>"));
}
Other_MainWindow::~Other_MainWindow()
{
    delete ui;
}
void Other_MainWindow::keyPressEvent(QKeyEvent *event)
{
   //qDebug()<<event;
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
    case Qt::Key_F:
    {

    }break;
    case Qt::Key_T:
    {
          struct SoftInfo
          {
            QString m_strSoftName;
            QString m_strSoftVersion;
            QString m_strInstallLocation;
            QString m_strPublisher;
            QString m_strMainProPath;
            QString m_strUninstallPth;
          };
          struct SoftInfo softinfo;
                HKEY RootKey;                                                                           // 主键
                LPCTSTR lpSubKey;                                                                  // 子键名称
                HKEY hkResult;                                                                         // 将要打开键的句柄
                HKEY hkRKey;
                LONG lReturn;                                                                        // 记录读取注册表是否成功
                QString strBuffer;
                QString strMidReg;

             DWORD index = 0;
             TCHAR szKeyName[255] = {0};        // 注册表项名称
             TCHAR szBuffer[255] = {0};
             DWORD dwKeyLen = 255;
             DWORD dwNameLen = 255;
             DWORD dwType = REG_BINARY|REG_DWORD|REG_EXPAND_SZ|REG_MULTI_SZ|REG_NONE|REG_SZ;
             RootKey = HKEY_LOCAL_MACHINE;
             //RootKey = HKEY_CURRENT_USER;//"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
             //SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall
             //SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall
          lpSubKey = _T(/*"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"*/"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
          lReturn = RegOpenKeyEx(RootKey, lpSubKey, 0, KEY_ALL_ACCESS, &hkResult);
          if (lReturn == ERROR_SUCCESS){
              while (ERROR_NO_MORE_ITEMS !=RegEnumKeyEx(hkResult, index, szKeyName, &dwKeyLen, 0, NULL, NULL, NULL)){
                  index++;
                  strBuffer=QString::fromStdWString(szKeyName);
                      if (!strBuffer.isEmpty()){
                               strMidReg = QString::fromStdWString(lpSubKey) +"\\" + strBuffer;
                                if (RegOpenKeyEx(RootKey, strMidReg.toStdWString().c_str(), 0, KEY_ALL_ACCESS, &hkRKey)==ERROR_SUCCESS) {
                                        RegQueryValueEx(hkRKey, _T("DisplayName"), 0, &dwType, (LPBYTE)szBuffer,&dwNameLen);
                                        softinfo.m_strSoftName = QString::fromStdWString(szBuffer);
                                        dwNameLen = 255;
                                        memset(szBuffer, 0, sizeof(szBuffer));

                                        RegQueryValueEx(hkRKey, _T("DisplayVersion"), 0, &dwType, (LPBYTE)szBuffer,&dwNameLen);
                                        softinfo.m_strSoftVersion = QString::fromStdWString(szBuffer);
                                        dwNameLen = 255;
                                        memset(szBuffer, 0, sizeof(szBuffer));

                                        RegQueryValueEx(hkRKey, _T("InstallLocation"), 0, &dwType, (LPBYTE)szBuffer,&dwNameLen);
                                        softinfo.m_strInstallLocation =QString::fromStdWString(szBuffer);
                                        dwNameLen = 255;
                                        memset(szBuffer, 0, sizeof(szBuffer));

                                        RegQueryValueEx(hkRKey, _T("Publisher"), 0, &dwType, (LPBYTE)szBuffer,&dwNameLen);
                                        softinfo.m_strPublisher = QString::fromStdWString(szBuffer);
                                        memset(szBuffer, 0, sizeof(szBuffer));

                                        RegQueryValueEx(hkRKey, _T("InstallLocation"), 0, &dwType, (LPBYTE)szBuffer,&dwNameLen);
                                        softinfo.m_strMainProPath = QString::fromStdWString(szBuffer);
                                        dwNameLen = 255;
                                        memset(szBuffer, 0, sizeof(szBuffer));

                                        RegQueryValueEx(hkRKey, _T("UninstallString"), 0, &dwType, (LPBYTE)szBuffer,&dwNameLen);
                                        softinfo.m_strUninstallPth = QString::fromStdWString(szBuffer);
                                        dwNameLen = 255;
                                        memset(szBuffer, 0, sizeof(szBuffer));
                                        if(!softinfo.m_strSoftName.isEmpty()) {
                                                qDebug()<<softinfo.m_strSoftName;
                                               qDebug()<<softinfo.m_strSoftVersion;
                                    }
                                }
                                  dwKeyLen = 255;
                                  memset(szBuffer, 0, sizeof(szBuffer));
                          }
                      }
                      RegCloseKey(hkResult);
               } else   ::MessageBox(NULL, _T("打开注册表失败!"), NULL, MB_ICONWARNING);
    }break;
    case Qt::Key_O:
    {
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
void Other_MainWindow::keyReleaseEvent(QKeyEvent *event)
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
