#include <ntddk.h>

extern __forceinline void aProxyBody(void);
extern __forceinline void aStoreCallee(void);
extern __forceinline void aUnload(void);

//#define NAKED _declspec(naked)
#define NAKED

struct SYS_SERVICE_TABLE { 
	void **ServiceTable; 
	unsigned long CounterTable; 
	unsigned long ServiceLimit; 
	void **ArgumentsTable; 
};

const WCHAR devicename[]=L"\\Device\\noCheat";
const WCHAR devicelink[]=L"\\DosDevices\\NOCHEAT";

KEVENT event;
ULONG Index,RealCallee;
char* output;
extern struct SYS_SERVICE_TABLE *KeServiceDescriptorTable; 


//this function decides whether we should allow NtCreateSection() call to be successfull
VOID __stdcall check(PULONG arg)
{
	DbgPrint("[nC]Hook fired!");
}


//just saves execution contect and calls check() 
NAKED Proxy()
{
	DbgPrint("[nC]Proxy!");
	aProxyBody();
}


NTSTATUS DrvDispatch(IN PDEVICE_OBJECT device,IN PIRP Irp)

{
	UCHAR*buff=0; ULONG a,base;

	PIO_STACK_LOCATION loc=IoGetCurrentIrpStackLocation(Irp);

	if(loc->Parameters.DeviceIoControl.IoControlCode==1000)
	{
		DbgPrint("[nC]Kernel Dispatcher");

		buff=(UCHAR*)Irp->AssociatedIrp.SystemBuffer;


		// hook service dispatch table
		memmove(&Index,buff,4);
		a=4*Index+(ULONG)KeServiceDescriptorTable->ServiceTable;
		base=(ULONG)MmMapIoSpace(MmGetPhysicalAddress((void*)a),4,0);
		a=(ULONG)&Proxy;

		_asm {
			mov rax,base
			mov rbx,dword ptr[rax]
			mov RealCallee,rbx
			mov rbx,a
			mov dword ptr[rax],rbx
		}

		MmUnmapIoSpace(base,4);

		memmove(&a,&buff[4],4);
		output=(char*)MmMapIoSpace(MmGetPhysicalAddress((void*)a),256,0);
		DbgPrint("[nC]Mapped address");
	}



	Irp->IoStatus.Status=0;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);
	return 0;


}



// nothing special
NTSTATUS DrvCreateClose(IN PDEVICE_OBJECT device,IN PIRP Irp)

{

	Irp->IoStatus.Information=0;
	Irp->IoStatus.Status=0;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);
	return 0;

}



// nothing special -just a cleanup
void DrvUnload(IN PDRIVER_OBJECT driver)
{
	UNICODE_STRING devlink;
	ULONG a,base;

	//unhook dispatch table
	a=4*Index+(ULONG)KeServiceDescriptorTable->ServiceTable;
	base=(ULONG)MmMapIoSpace(MmGetPhysicalAddress((void*)a),4,0);

	aUnload();

	MmUnmapIoSpace(base,4);
	MmUnmapIoSpace(output,256);

	RtlInitUnicodeString(&devlink,devicelink);
	IoDeleteSymbolicLink(&devlink);
	IoDeleteDevice(driver->DeviceObject);
}


//DriverEntry just creates our device - nothing special here
NTSTATUS DriverEntry(IN PDRIVER_OBJECT driver,IN PUNICODE_STRING path)
{
	PDEVICE_OBJECT devobject=0;

	UNICODE_STRING devlink,devname;

	ULONG a,b;



	RtlInitUnicodeString(&devname,devicename);
	RtlInitUnicodeString(&devlink,devicelink);

	IoCreateDevice(driver,256,&devname,FILE_DEVICE_UNKNOWN,0,TRUE,&devobject);
	IoCreateSymbolicLink(&devlink,&devname);



	driver->MajorFunction[IRP_MJ_DEVICE_CONTROL]=DrvDispatch;
	driver->MajorFunction[IRP_MJ_CREATE]=DrvCreateClose;
	driver->MajorFunction[IRP_MJ_CLOSE]=DrvCreateClose;
	driver->DriverUnload=DrvUnload;
	KeInitializeEvent(&event,SynchronizationEvent,1);


	DbgPrint("[nC]Starting Service");

	return 0;
}