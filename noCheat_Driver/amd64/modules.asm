.code

aProxyBody PROC

	pushfq
	
	push RAX
	push RCX
	push RDX
	push RBX
	push RSP
	push RBP
	push RSI
	push RDI
	
	mov rbx,rsp
	add rbx,40
	push rbx
	call check

	pop RDI
	pop RSI
	pop RBP
	pop RSP
	pop RBX
	pop RDX
	pop RCX
	pop RAX
	
	popfq
	jmp RealCallee

aProxyBody ENDP

aStoreCallee PROC

	mov rax,base
	mov rbx,dword ptr[rax]
	mov RealCallee,rbx
	mov rbx,a
	mov dword ptr[rax],rbx

aStoreCallee ENDP

aUnload PROC
	mov rax,base
	mov rbx,RealCallee
	mov dword ptr[rax],rbx
aUnload ENDP

END