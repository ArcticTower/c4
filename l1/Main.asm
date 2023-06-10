format PE GUI 4.0
entry start
 
include 'win32a.inc'
 
ID_MESSAGE = 102
ID_OK = 103
ID_CANCEL = 104
buf_size = 1024
 
section '.text' code readable executable 
 
  start:
        invoke GetModuleHandle,0
        invoke DialogBoxParam,eax,0,HWND_DESKTOP,DialogProc,0
  exit:
        invoke ExitProcess,0

proc DialogProc hwnddlg,msg,wparam,lparam
        local buffer[buf_size]:BYTE
        push ebx esi edi
        lea esi,[buffer]
        cmp     [msg],WM_INITDIALOG                                      
        je      .wminitdialog
        cmp     [msg],WM_COMMAND                                         
        je      .wmcommand
        cmp     [msg],WM_CLOSE                                           
        je      .wmclose
        xor     eax,eax                                                  
        jmp     .finish
  .wminitdialog:
        jmp     .processed
  .wmcommand:
        cmp     [wparam],BN_CLICKED shl 16 + ID_CANCEL                   
        je      .wmclose
        cmp     [wparam],BN_CLICKED shl 16 + ID_OK                       
        jne     .processed
        invoke GetDlgItemText,[hwnddlg],ID_MESSAGE,esi,100h
        
        ;rev
        mov ecx,eax
        test eax, eax
        je .nonentry
        mov edx,eax
        .loop1:
        movzx eax, byte [esi]   ;byte
        inc   esi
        inc   esi     ;-
        ;push  eax
        dec   ecx
        jnz   .loop1
        lea esi,[buffer]
        .loop2:
        pop   eax
        mov   [esi], al ;al
        inc   esi
        ;inc   esi    ;-
        dec   edx
        jnz   .loop2
        .nonentry:
     
        lea esi,[buffer]
        invoke MessageBox,HWND_DESKTOP,esi,caption,0
        invoke EndDialog,[hwnddlg],1                                 
        jmp     .processed
  .wmclose:
        invoke EndDialog,[hwnddlg],0                                 
  .processed:
        mov     eax,1                                                    
  .finish:
        pop     edi esi ebx
        ret
endp
 
section '.bss' readable writeable
  caption du 'T','E','X','T',0,0
 
section '.idata' import data readable writeable
 
  library kernel,'KERNEL32.DLL',\
          user,'USER32.DLL'
 
  import kernel,\
         GetModuleHandle,'GetModuleHandleA',\
         ExitProcess,'ExitProcess'
 
  import user,\
         DialogBoxParam,'DialogBoxParamA',\  ;A
         GetDlgItemText,'GetDlgItemTextA',\
         MessageBox,'MessageBoxA',\
         EndDialog,'EndDialog'
 
section '.rsrc' resource data readable
 
  directory RT_DIALOG,dialogs
 
  resource dialogs,0,LANG_ENGLISH+SUBLANG_DEFAULT,demonstration
 
  dialog demonstration,'Lab1',70,70,200,70,WS_CAPTION+WS_POPUP+WS_SYSMENU+DS_MODALFRAME
    dialogitem 'STATIC','Message:',-1,10,10,70,8,WS_VISIBLE
    dialogitem 'EDIT','',ID_MESSAGE,10,20,170,13,WS_VISIBLE+WS_BORDER+WS_TABSTOP+ES_AUTOHSCROLL
    dialogitem 'BUTTON','OK',ID_OK,80,40,45,15,WS_VISIBLE+WS_TABSTOP+BS_DEFPUSHBUTTON
    dialogitem 'BUTTON','Cancel',ID_CANCEL,135,80,45,15,WS_VISIBLE+WS_TABSTOP+BS_PUSHBUTTON
  enddialog