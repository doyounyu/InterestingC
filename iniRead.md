
## .ini를 C에서 읽기(float)

1. 메모장으로 .ini 파일을 만든다. 
예시:

```txt:preset.ini
; 한글이 들어가 있어야 UTF-8로 안바뀜
[PRESET]
StartReferenceVoltage	=	1.8	;[V] 시작 전압	
EndReferenceVoltage	=	2.0	;[V] 끝 전압

[LINEARITYCHECK]
w_start			=	900	;[rad/s] 시작 각속도
w_end			=	200	;[rad/s] 끝 각속도

[FREQUENCYSWEEP]
sweepInterval		 =	0.2	;[Hz] Frequency Sweep 인터벌
```
저장할 때 ANSI 형으로 저장해야 한다. 꼭 한글을 주석으로라도 포함해야 UTF-8로 강제 변환되지 않는다.

2. C에서 ```#include <tchar.h>``` 헤더를 추가한다.

3. 
