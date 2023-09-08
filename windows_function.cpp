#include "windows_function.h"
#include "windows.h"
#include "powrprof.h"
#include "netfw.h"
#include "setupapi.h"
#include "cfgmgr32.h"
#include <QDebug>

HRESULT WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2);//disable_fire call
static BOOL  CALLBACK  EnumChildProc(HWND hwndChild, LPARAM lParam);
BOOL CALLBACK EnumWindowsProc(HWND hWnd,LPARAM lParam);
Windows_function::Windows_function(QObject *parent) : QObject(parent)
{
    info_result_list.clear();
}
Windows_function::~Windows_function()
{
}
void Windows_function::setting_work(){
        WindowsSetting();
        DevicCheck();
        advancesharing(true);
        emit Windows_function::setting_complemt(&info_result_list);
    }
void Windows_function::WindowsSetting(){
        UINT uiID=0;
        POWER_POLICY PwrPolicy;
        PGLOBAL_POWER_POLICY pGlobalPowerPolicy = new GLOBAL_POWER_POLICY;
        //PowerEnumerate
        GUID SchemeGuid, PowerGuid;
        ULONG index = 0;
        DWORD BufferSize = sizeof(GUID);
        //Firewall enable and disable
        HRESULT hrComInit = S_OK;
        HRESULT hr = S_OK;
        INetFwPolicy2* pNetFwPolicy2 = NULL;
        PowerEnumerate(NULL, NULL, NULL, ACCESS_SCHEME, 0, (PUCHAR)&SchemeGuid, &BufferSize);//get ActiveScheme
        PowerGuid = SchemeGuid;//Save ActiveScheme for Restore
        while (ERROR_SUCCESS == PowerEnumerate(NULL, NULL, NULL, ACCESS_SCHEME, index, (PUCHAR)&SchemeGuid, &BufferSize)){
            uiID = index;
            PowerSetActiveScheme(NULL, &SchemeGuid);
            if (GetActivePwrScheme(&uiID)){
                ReadPwrScheme(uiID, &PwrPolicy);
                PwrPolicy.user.VideoTimeoutAc = 0;
                PwrPolicy.user.VideoTimeoutDc = 0;                                            //PwrPolicy.user.MaxSleepAc = PowerSystemMaximum; // Will affect PowerButtonAc.Action value
                WritePwrScheme(&uiID, NULL, NULL, &PwrPolicy);                //PowerReadFriendlyName(NULL, (GUID*)&SchemeGuid, NULL, NULL, (PUCHAR)pSchemeBuf, &dwSchemeSize);
                 SetActivePwrScheme(uiID, NULL, &PwrPolicy);                        //WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pSchemeBuf, -1, (LPSTR)szSchemeName, MAX_PATH, NULL, NULL);
            }
            index++;
        }
        PowerSetActiveScheme(NULL, &PowerGuid);//Restore ActiveoldScheme
        if (ReadGlobalPwrPolicy(pGlobalPowerPolicy) && GetActivePwrScheme(&uiID)){
            pGlobalPowerPolicy->user.PowerButtonAc.Action = PowerActionNone;//Do nothing    PowerActionNone = 0, PowerActionReserved, PowerActionSleep, PowerActionHibernate, PowerActionShutdown, PowerActionShutdownReset, PowerActionShutdownOff, PowerActionWarmEject
            pGlobalPowerPolicy->user.SleepButtonAc.Action = PowerActionNone;//Do nothing     PowerActionNone = 0, PowerActionReserved, PowerActionSleep, PowerActionHibernate, PowerActionShutdown, PowerActionShutdownReset, PowerActionShutdownOff, PowerActionWarmEject
            WriteGlobalPwrPolicy(pGlobalPowerPolicy);
            SetActivePwrScheme(GetActivePwrScheme(&uiID),pGlobalPowerPolicy,NULL);
        } else {
                    info_result_list.append("<h3><font color=red>Get And Read Power Set config Failed</font></h3>");
           }
        if (0==WriteGlobalPwrPolicy(pGlobalPowerPolicy)) {
            info_result_list.append("<h3><font color=red>Write Nothing Powerbutton Failed</font></h3>");
        } else {
                    SetActivePwrScheme(uiID, pGlobalPowerPolicy, NULL);
                    info_result_list.append("<h3><font color=green>Write Nothing Powerbutton Successfully</font></h3>");
           }
        if (ClearEventLogW(OpenEventLogA(NULL, "system"), NULL) && ClearEventLogW(OpenEventLogA(NULL, "Application"), NULL))  {
            info_result_list.append("<h3><font color=green>Clean System and  APP Log Successfully</font></h3>");
        } else{
                     info_result_list.append("<h3><font color=red>Clean System and  APP Failed</font></h3>");
           }
        if (system("wmic recoveros set AutoReboot = False") || system("wmic recoveros set DebugInfoType = 1")){
              info_result_list.append("<h3><font color=red>AutoReboot and complete memory dump setting Failure");
        } else {
                     info_result_list.append("<h3><font color=green>AutoReboot and complete memory dump setting PASS</font></h3>");
          }
        if (system("sc config wuauserv start=disabled")){
            info_result_list.append("<h3><font color=color=red>Windows update service setting disable Failure</font></h3>");
        }else {
                    info_result_list.append("<h3><font color=green>Windows update service setting disable PASS</font></h3>");
        }
        if (system("%windir%\\system32\\reg.exe ADD HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v EnableLUA /t REG_DWORD /d 0 /f"))   {//User Account Control Settings
        } else{
                    info_result_list.append("<h3><font color=green>User Account Control Settings (UAC) setting  PASS</font></h3>");
           }
        if (!system("%windir%\\system32\\reg.exe ADD \"HKLM\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Power\" /V HiberbootEnabled /T REG_dWORD /D 0 /F"))  {
            info_result_list.append("<h3><font color=green>Disable Fast Startup PASS</font></h3>");
        }else{
                    info_result_list.append("<h3><font color=red>Disable Fast Startup Failure</font></h3>");
                    system("%windir%\\system32\\control.exe /name Microsoft.PowerOptions /page pageGlobalSettings");
          }
        if (!system("%windir%\\system32\\reg.exe ADD HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\Explorer /V ShowHibernateOption /T REG_dWORD /D 1 /F"))  {
            info_result_list.append("<h3><font color=green>Enable ShowHibernateOption PASS</font></h3>");
        } else {
                    info_result_list.append("<h3><font color=red>Enable ShowHibernateOption Failure</font></h3>");
                    system("%windir%\\system32\\control.exe /name Microsoft.PowerOptions /page pageGlobalSettings");
           }
        if (!system("%windir%\\system32\\reg.exe ADD HKLM\\SYSTEM\\ControlSet001\\Control\\CI\\Config /V  VulnerableDriverBlocklistEnable /T REG_dWORD /D 0 /F"))  {
            info_result_list.append("<h3><font color=green>Disable DriverBlocklist (RW.EXE)PASS</font></h3>");
        } else {
                    info_result_list.append("<h3><font color=red>Disable DriverBlocklist (RW.EXE) Failure</font></h3>");
                    system("%windir%\\system32\\control.exe /name Microsoft.PowerOptions /page pageGlobalSettings");
           }
        if (!system("%windir%\\system32\\reg.exe ADD HKLM\\SYSTEM\\CurrentControlSet\\Control\\DeviceGuard\\Scenarios\\HypervisorEnforcedCodeIntegrity /V Enabled /T REG_dWORD /D 0 /F"))  {
            info_result_list.append("<h3><font color=green>OFF Memory Integrity (RW.EXE)PASS</font></h3>");
        } else {
                    info_result_list.append("<h3><font color=red>OFF Memory Integrity (RW.EXE) Failure</font></h3>");
                    system("%windir%\\system32\\control.exe /name Microsoft.PowerOptions /page pageGlobalSettings");
           }
        if (system("%windir%\\system32\\control.exe /name Microsoft.NetworkAndSharingCenter /page Advanced"))  {// control.exe /name Microsoft.NetworkAndSharingCenter /page Advanced
            info_result_list.append("<h3><font color=green>Open Network And Sharing Center sucessfully</font></h3>");
        } else {
                    info_result_list.append("<h3><font color=red>Open Network And Sharing Center sucessfully Failure</font></h3>");
                    system("%windir%\\system32\\control.exe");
           }
        // Initialize COM.
        hrComInit = CoInitializeEx(
                0,
                COINIT_APARTMENTTHREADED
        );
        // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
        // initialized with a different mode. Since we don't care what the mode is,
        // we'll just use the existing mode.
        if (hrComInit != RPC_E_CHANGED_MODE) {
            if (FAILED(hrComInit))  {
                info_result_list.append("<h3><font color=red>CoInitializeEx failed: 0x%08lx</font></h3>");
                goto Cleanup;
            }
        }
        // Retrieve INetFwPolicy2
        hr = WFCOMInitialize(&pNetFwPolicy2);
        if (FAILED(hr))  {
            goto Cleanup;
        }
        // Disable Windows Firewall for the Domain profile
        hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_DOMAIN, FALSE);
        if (FAILED(hr))   {
            info_result_list.append("<h3><font color=red>put_FirewallEnabled failed for Domain: 0x%08lx</font></h3>");
            goto Cleanup;
        } else {
                       info_result_list.append("<h3><font color=green>put_FirewallEnabled PASS for Domain</font></h3>");
          }
        // Disable Windows Firewall for the Private profile
        hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PRIVATE, FALSE);
        if (FAILED(hr))  {
             info_result_list.append("<h3><font color=red>put_FirewallEnabled failed for Private: 0x%08l</font></h3>");
            goto Cleanup;
        } else {
                   info_result_list.append("<h3><font color=green>put_FirewallEnabled PASS for Private</font></h3>");
           }

        // Disable Windows Firewall for the Public profile
        hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, FALSE);
        if (FAILED(hr))  {
            info_result_list.append("<h3><font color=red>put_FirewallEnabled failed for Public: 0x%08lx</font></h3>");
            goto Cleanup;
        } else {
                    info_result_list.append("<h3><font color=green>put_FirewallEnabled PASS for Public</font></h3>");
           }
    Cleanup:
        // Release INetFwPolicy2
        if (pNetFwPolicy2 != NULL) {
            pNetFwPolicy2->Release();
        }
        // Uninitialize COM.
        if (SUCCEEDED(hrComInit))  {
            CoUninitialize();
        }
        delete pGlobalPowerPolicy;
}
void Windows_function::DevicCheck(){
        int Dev_err=0;
        bool check_status=false;
        HDEVINFO hDevInfo;
        SP_DEVINFO_DATA DeviceInfoData;
        DWORD dwDevStatus, dwProblem,i;
        SP_DEVINFO_DATA DevInfoData = { sizeof(SP_DEVINFO_DATA),{0,0,0,{0}},0,0};
        //step1. Create a HDEVINFO with all present devices.
        hDevInfo = SetupDiGetClassDevs(NULL,
            0, // Enumerator
            0,
            DIGCF_PRESENT | DIGCF_ALLCLASSES);
        if (hDevInfo == INVALID_HANDLE_VALUE)  {
            info_result_list.append("<h3><font color=red>Device error INVALID</font></h3>");
        }
        DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++) {
            DWORD DataT;
            LPTSTR buffer = NULL;
            DWORD buffersize = 0;
            while (!SetupDiGetDeviceRegistryProperty(
                hDevInfo,
                &DeviceInfoData,
                SPDRP_HARDWAREID,
                &DataT,
                (PBYTE)buffer,
                buffersize,
                &buffersize)) {
                                        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {      // Change the buffer size.
                                                if (buffer) LocalFree(buffer);
                                                buffer = (LPTSTR)LocalAlloc(LPTR, buffersize);
                                        } else {
                                                     break;// Insert error handling here.
                                           }
                                     }
            if (!SetupDiEnumDeviceInfo(hDevInfo, i, &DevInfoData)) {
                info_result_list.append("<h3><font color=red>EnumDeviceInfo Error!</font></h3>");
            }
            if (CM_Get_DevNode_Status(&dwDevStatus, &dwProblem, DevInfoData.DevInst, 0) != CR_SUCCESS)  {
                info_result_list.append("<h3><font color=red>DevNode_Status Error!</font></h3>");
            }
            //Device Prolem status
            if (dwProblem!=0) {
                Dev_err++;
                check_status=true;
             }
           qDebug()<< DevInfoData.Reserved;
       }
        if (check_status) {
            QString count;
            info_result_list.append("<h3><font color=red>The System Device Management have YB and unknown device!</font></h3>");
            info_result_list.append("<h3><font color=red>Error Device Count:    "+count.setNum(Dev_err)+"</font></h3>");
            if (system("START devmgmt.msc")) {
                info_result_list.append("<h3><font color=red>Open Decice Manager Error!</font></h3>");
            }else {
                        info_result_list.append("<h3><font color=red>Open Decice Manager sucessfully!</font></h3>");
                   }
        } else {
                    info_result_list.append("<h1><font color=green>Device Management Check PASS!</font></h1>");
                 }

}
HRESULT WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2) {
    HRESULT hr = S_OK;
    hr = CoCreateInstance(
        __uuidof(NetFwPolicy2),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwPolicy2),
        (void**)ppNetFwPolicy2);

    if (FAILED(hr))  {
        printf("CoCreateInstance for INetFwPolicy2 failed: 0x%08lx\n", hr);
        goto Cleanup;
    }
Cleanup:
    return hr;
}

void Windows_function::advancesharing(bool info){
    HWND advanceSet;
    int count = 0;
    do {
             advanceSet = FindWindow(NULL,L"Advanced sharing settings");
             if (!advanceSet) {
                 advanceSet = FindWindow(NULL,L"Settings");
             }
             if (65535==count++) {
                 info_result_list.append("<h2><font color=red>Settings page break count!!</font></h2>");
                 break;
             }
        }
    while (!advanceSet);
    if(info)info_result_list.append("<h2><font color=#8D0B9E>You can Set with Advanced sharing settings page!!</font></h2>");
     for (int var = 0; var < 4; ++var) {
              EnumChildWindows(advanceSet,EnumChildProc,var);
     }
     RECT adv_rect,desktop_rect;
     GetWindowRect(advanceSet,&adv_rect);
     GetWindowRect(GetDesktopWindow(),&desktop_rect);
     MoveWindow(advanceSet,50,200,800,900,true);
}
//WINBOOL (CALLBACK *WNDENUMPROC) (HWND, LPARAM);
BOOL  CALLBACK   EnumChildProc(HWND hwndChild, LPARAM lParam){
    char sBuf[256];
    QStringList Optionlist;
    Optionlist<<QString("Turn on sharing so anyone with network access can read and write files in the Public folders")
                    <<QString("Use 128-bit encryption to help protect file sharing connections (recommended)")
                    <<QString("Turn off password protected sharing")
                    <<QString("Save changes")
                    <<QString("Public networks")
                    <<QString("Current profile")
                    <<QString("Network discovery")
                    <<QString("Off")
                    <<QString("All networks")
                    <<QString("Public folder sharing")
                    <<QString("Off")
                    <<QString("Password protected sharing")
                    <<QString("On")
                    <<QString("Public folder sharing")
                    <<QString("Off")
                    <<QString("Password protected sharing")
                    <<QString("On")
                    <<QString("On");
    GetWindowTextA(hwndChild,sBuf,256);
    for (int var = 0; var < lParam; ++var) {
        foreach (QString var, Optionlist) {
            if(QString::fromUtf8(sBuf)==var){
                WORD setupID = (WORD)GetDlgCtrlID(hwndChild);
                SendMessage(hwndChild,WM_COMMAND,MAKELONG(setupID,BN_CLICKED),LPARAM(hwndChild));
            }
        }
        HWND button = nullptr;
        foreach (QString var, Optionlist) {
              if((hwndChild=FindWindowEx(hwndChild,nullptr,nullptr,var.toStdWString().c_str())) != nullptr) {
                SendMessage(hwndChild,WM_COMMAND,MAKELONG((WORD)GetDlgCtrlID(button),BN_CLICKED),LPARAM(button));
            }
        }
  }
    return TRUE;
}
BOOL CALLBACK EnumWindowsProc(HWND hWnd,LPARAM lParam){//All desktop windows
    char sBuf[256];
    qDebug()<<sBuf<<hWnd<<lParam;
    return TRUE;
}
