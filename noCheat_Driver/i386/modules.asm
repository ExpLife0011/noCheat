.code

aProxyBody PROC

	pushfd
	pushad
	mov ebx,esp
	add ebx,40
	push ebx
	call check

	popad
	popfd
	jmp RealCallee

aProxyBody ENDP

aStoreCallee PROC

	mov eax,base
	mov ebx,dword ptr[eax]
	mov RealCallee,ebx
	mov ebx,a
	mov dword ptr[eax],ebx

aStoreCallee ENDP

aUnload PROC
	mov eax,base
	mov ebx,RealCallee
	mov dword ptr[eax],ebx
aUnload ENDP

END