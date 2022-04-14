
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
여기서 [] 안에 들어가는 문구를 section 이라고 하고, 어쩌고 = 숫자 에 어쩌고에 해당하는 값을 key 라고 한다.
저장할 때 ANSI 형으로 저장해야 한다. 꼭 한글을 주석으로라도 포함해야 UTF-8로 강제 변환되지 않는다.

2. C에서 ```#include <tchar.h>``` 헤더를 추가한다.

3. .ini 파일에서 int형 숫자를 읽어온다면 크게 무리가 없지만, float일 경우 ```TCHAR``` 형 string을 읽어와서 float로 변경시켜줘야 한다. string은 ```GetPrivateProfileString``` 라는 함수로 읽어올 수 있다. 이 함수의 인자는 다음과 같다. 

```c:
GetPrivateProfileString(
    LPCWSTR section,
    LPCWSTR key,
    LPCWSTR 못 찾았을때 디폴트값,
    LPWSTR str,
     버퍼 크기, 보통 sizeof(str)/sizeof(str[0]),
    LPCWSTR 파일 주소
    );
```
각각의 입력 인자에 또 형변환을 해줘야 되니 그냥 다음과 같은 매크로를 만들자:
```c:
#define INI_STRING_READ(SECTION, KEY, STRING) GetPrivateProfileStringW((L##SECTION), (L##KEY), "1212112", (STRING), sizeof(STRING)/sizeof((STRING)[0]), L##"C:\\preset.ini")

```

4. 마지막으로 버퍼에 저장된 스트링을 double/float형 변수에 저장하면 된다. 간략한 전체 예제를 보자:
```c:
#define INI_STRING_READ(SECTION, KEY, STRING) GetPrivateProfileStringW((L##SECTION), (L##KEY), "1212112", (STRING), sizeof(STRING)/sizeof((STRING)[0]), L##"C:\\preset.ini")

#include <tchar.h>
#include <stdio.h>

int main() {
	TCHAR				VrefString[20]; // double 받아올 string
	INI_STRING_READ("PRESET", "StartReferenceVoltage", VrefString); // 읽은 숫자 string에 저장
	double				Vref = _tstof(VrefString); //string -> double로 변환

}
```
이런 느낌이다.
