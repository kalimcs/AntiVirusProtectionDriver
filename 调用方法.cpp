#include <iostream>
#include <windows.h>

#define IOCTL_PROTECTION_PROCESS  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)  //进程保护传入pid即可（Win10,11用这个）
#define IOCTL_PROTECTION_PROCESS8 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)  //进程保护传入pid即可（Win8用这个）
#define IOCTL_TERMINATE_PROCESS   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)  //强杀进程
#define IOCTL_HIDDEN_PROCESS      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)  //隐藏进程(没啥用qwq)win10.11用这个
#define IOCTL_HIDDEN_PROCESS8     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)  //隐藏进程(没啥用qwq)win8用这个
int main()
{
  //这一段是设置自己保护
  //自己写驱动加载
  //通信DeviceIoControl就行了
  //自己判断系统版本进行替换BOOL status = DeviceIoControl(hDevice, IOCTL_PROTECTION_PROCESS, &pid, sizeof(DWORD), NULL, 0, &bytesReturned, NULL);这里的IOCTL_PROTECTION_PROCESS是win10，11的win8的话替换成IOCTL_PROTECTION_PROCESS
            DWORD pid = GetCurrentProcessId();
            HANDLE hDevice;
            TCHAR szDeviceName[] = TEXT("\\\\.\\AVDriver");

            hDevice = CreateFile(szDeviceName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
            if (hDevice == INVALID_HANDLE_VALUE) {
                printf("Failed to open device. Error %ld\n", GetLastError());
                return 1;
            }

            DWORD bytesReturned;
            BOOL status = DeviceIoControl(hDevice, IOCTL_PROTECTION_PROCESS, &pid, sizeof(DWORD), NULL, 0, &bytesReturned, NULL);
            if (status) {
                printf("PID %ld Protection successfully.\n", pid);
            }
            else {
                printf("Failed. Error %ld\n", GetLastError());
            }

            CloseHandle(hDevice);
}
