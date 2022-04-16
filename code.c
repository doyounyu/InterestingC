//프로그래스 바. label에 문구를 달고 메인문에 넣으면 step이 증가함에 따라 막대기가 참.

extern void DoProgress(char label[], int step, int total)
{
	//progress width
	const int pwidth = 72;

	//minus label len
	int width = pwidth - strlen(label);
	int pos = (step * width) / total;


	int percent = (step * 100) / total;

	//set green text color, only on Windows
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	printf("%s[", label);

	//fill progress bar with =
	for (int i = 0; i < pos; i++)  printf("%c", '#');

	//fill progress bar with spaces
	printf("% *c", width - pos + 1, ']');
	printf(" %3d%%\r", percent);

	//reset text color, only on Windows
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
}

//1차 linear regreession 
/* 
_datanum: 데이터 개수
_x[]: 입력받은 x축 값
_y[]: 입력받은 y축 값
out[]: out[0]이 y = ax + b의 a
       out[1]이 y = ax + b의 b
*/
extern void linReg1(int _datanum, double _x[], double _y[], double out[])
{
	double sumX = 0.0, sumX2 = 0.0, sumY = 0.0, sumXY = 0.0;

	double a = 0.0, b = 0.0;



	for (int i = 0; i < _datanum; i++)
	{
		sumX = sumX + _x[i];
		sumX2 = sumX2 + _x[i] * _x[i];
		sumY = sumY + _y[i];
		sumXY = sumXY + _x[i] * _y[i];
	}


	a = (_datanum * sumXY - sumX * sumY) / (_datanum * sumX2 - sumX * sumX);
	b = (sumY - a * sumX) / _datanum;


	out[0] = a;  out[1] = b;

	printf("\ny = ax + b, a = %lf, b = %lf\n", a, b);

}
