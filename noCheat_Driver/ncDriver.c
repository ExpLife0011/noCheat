#include <ntddk.h>

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
	DbgPrint("Hook fired!");
}


//just saves execution contect and calls check() 
_declspec(naked) Proxy()
{

	_asm {

			//save execution contect and calls check()
			pushfd
			pushad
			mov ebx,esp
			add ebx,40
			push ebx
			call check

			//proceed to the actual callee
			popad
			popfd
			jmp RealCallee
	}
}


NTSTATUS DrvDispatch(IN PDEVICE_OBJECT device,IN PIRP Irp)

{
	UCHAR*buff=0; ULONG a,base;



	PIO_STACK_LOCATION loc=IoGetCurrentIrpStackLocation(Irp);

	if(loc->Parameters.DeviceIoControl.IoControlCode==1000)
	{
		buff=(UCHAR*)Irp->AssociatedIrp.SystemBuffer;


		// hook service dispatch table
		memmove(&Index,buff,4);
		a=4*Index+(ULONG)KeServiceDescriptorTable->ServiceTable;
		base=(ULONG)MmMapIoSpace(MmGetPhysicalAddress((void*)a),4,0);
		a=(ULONG)&Proxy;

		_asm
		{
				mov eax,base
				mov ebx,dword ptr[eax]
				mov RealCallee,ebx
				mov ebx,a
				mov dword ptr[eax],ebx
		}

		MmUnmapIoSpace(base,4);

		memmove(&a,&buff[4],4);
		output=(char*)MmMapIoSpace(MmGetPhysicalAddress((void*)a),256,0);
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

	_asm
	{
		mov eax,base
			mov ebx,RealCallee
			mov dword ptr[eax],ebx
	}

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


	return 0;
}