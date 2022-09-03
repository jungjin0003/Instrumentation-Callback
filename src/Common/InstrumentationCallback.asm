_TEXT SEGMENT

EXTERN RtlCaptureContext: PROC
EXTERN InstrumentationCallbackThunkEnd: PROC

PUBLIC InstrumentationCallbackThunkStart

InstrumentationCallbackThunkStart PROC
    mov qword ptr gs:[2E0h], rsp
    mov qword ptr gs:[2D8h], r10
    sub rsp, 4D0h
    and rsp, -10h
    mov rcx, rsp
    call RtlCaptureContext
    sub rsp, 20h
    call InstrumentationCallbackThunkEnd
InstrumentationCallbackThunkStart ENDP

_TEXT ENDS

END