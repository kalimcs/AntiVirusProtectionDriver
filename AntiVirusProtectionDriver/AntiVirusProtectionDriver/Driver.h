#pragma once

#include <ntifs.h>
#include <ntddk.h>

#define DEVICE_NAME         L"\\Device\AVDriver"
#define SYMBOLIC_LINK_NAME  L"\\DosDevices\\AVDriver"  //这里是设备名

#define IOCTL_PROTECTION_PROCESS  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)  //进程保护传入pid即可（Win10,11用这个）
#define IOCTL_PROTECTION_PROCESS8 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)  //进程保护传入pid即可（Win8用这个）
#define IOCTL_TERMINATE_PROCESS   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)  //强杀进程
#define IOCTL_HIDDEN_PROCESS      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)  //隐藏进程(没啥用qwq)win10.11用这个
#define IOCTL_HIDDEN_PROCESS8     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)  //隐藏进程(没啥用qwq)win8用这个

PDEVICE_OBJECT g_DeviceObject = NULL;
NTSTATUS CreateDevice(PDRIVER_OBJECT DriverObject);
NTSTATUS DeleteDevice();
VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject);
NTSTATUS DispatchCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp);
NTSTATUS DispatchDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp);

