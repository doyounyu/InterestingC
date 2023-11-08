/*

수신한 Serial Data 그대로 뱉음. Port 설정은 COM16으로 되어있음.

*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <time.h>

#define BUFFER_SIZE 100
#define MAX_BUFFER_SIZE 1024

double loop_start_time = 0.0f; double t_start = 0.0f;  double t_end = 0.0f;


double getWindowTime(void)
{
    LARGE_INTEGER	liEndCounter, liFrequency;

    QueryPerformanceCounter(&liEndCounter);
    QueryPerformanceFrequency(&liFrequency);

    return(liEndCounter.QuadPart / (double)(liFrequency.QuadPart) * 1000.0);
}

void print_error(const char* context)
{
    DWORD error_code = GetLastError();
    char buffer[256];
    DWORD size = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
        NULL, error_code, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        buffer, sizeof(buffer), NULL);
    if (size == 0) { buffer[0] = 0; }
    fprintf(stderr, "%s: %s\n", context, buffer);
}

// Opens the specified serial port, configures its timeouts, and sets its
// baud rate.  Returns a handle on success, or INVALID_HANDLE_VALUE on failure.
HANDLE open_serial_port(const char* device, uint32_t baud_rate)
{
    HANDLE port = CreateFileA(device, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    if (port == INVALID_HANDLE_VALUE)
    {
        print_error(device);
        return INVALID_HANDLE_VALUE;
    }

    // Flush away any bytes previously read or written.
    BOOL success = FlushFileBuffers(port);
    if (!success)
    {
        print_error("Failed to flush serial port");
        CloseHandle(port);
        return INVALID_HANDLE_VALUE;
    }

    // Configure read and write operations to time out after 100 ms.
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    //trial 1 : 001100 trial2 X01111 trial3 X11111 30ms! trial4 0 10 111 trial5 X01000 15ms!
    success = SetCommTimeouts(port, &timeouts);
    if (!success)
    {
        print_error("Failed to set serial timeouts");
        CloseHandle(port);
        return INVALID_HANDLE_VALUE;
    }

    // Set the baud rate and other options.
    DCB state = { 0 };
    state.DCBlength = sizeof(DCB);
    state.BaudRate = baud_rate;
    state.ByteSize = 8;
    state.Parity = NOPARITY;
    state.StopBits = ONESTOPBIT;
    success = SetCommState(port, &state);
    if (!success)
    {
        print_error("Failed to set serial settings");
        CloseHandle(port);
        return INVALID_HANDLE_VALUE;
    }

    return port;
}

// Writes bytes to the serial port, returning 0 on success and -1 on failure.
int write_port(HANDLE port, uint8_t* buffer, size_t size)
{
    DWORD written;
    BOOL success = WriteFile(port, buffer, size, &written, NULL);
    if (!success)
    {
        print_error("Failed to write to port");
        return -1;
    }
    if (written != size)
    {
        print_error("Failed to write all bytes to port");
        return -1;
    }
    return 0;
}

// Reads bytes from the serial port.
// Returns after all the desired bytes have been read, or if there is a
// timeout or other error.
// Returns the number of bytes successfully read into the buffer, or -1 if
// there was an error reading.
SSIZE_T read_port(HANDLE port, uint8_t* buffer, size_t size)
{
    DWORD received;
    t_start = getWindowTime(); 
    BOOL success = ReadFile(port, buffer, size, &received, NULL);
    t_end = getWindowTime();
    if (!success)
    {
        print_error("Failed to read from port");
        return -1;
    }
    return received;
}

//prints read string

void print_received_string(HANDLE port)
{
    uint8_t buffer[BUFFER_SIZE] = { 0 }; // Initiate all bytes to 0
    SSIZE_T received = read_port(port, buffer, BUFFER_SIZE - 1);
    if (received < 0)
    {
        fprintf(stderr, "Failed to read from port\n");
        return;
    }

    printf("Received string: %s\n", buffer);
}

void write_received_string(HANDLE port, FILE* output_file)
{
    uint8_t buffer[BUFFER_SIZE] = { 0 }; // Initiate all bytes to 0
    SSIZE_T received = read_port(port, buffer, BUFFER_SIZE - 1);
    if (received < 0)
    {
        fprintf(stderr, "Failed to read from port\n");
        return;
    }

    fprintf(output_file, "%s\n", buffer);
}



void print_time_consumed(int count, double time_start, double time_end, FILE* time_file)
{
    double time_taken = ((double)time_end - time_start) / CLOCKS_PER_SEC * 1000; // in ms
    fprintf(time_file, "%d, %lf\n", count, time_taken);
}





int main()
{


    double    Ay_buff[BUFFER_SIZE] = { 0.0f, };  double     Az_buff[BUFFER_SIZE] = { 0.0f, };  double Phi_buff[BUFFER_SIZE] = { 0.0f, };
    double AzCmd_buff[BUFFER_SIZE] = { 0.0f, };  double PhiCmd_buff[BUFFER_SIZE] = { 0.0f, };
    double     P_buff[BUFFER_SIZE] = { 0.0f, };  double      Q_buff[BUFFER_SIZE] = { 0.0f, };  double   R_buff[BUFFER_SIZE] = { 0.0f, };





    const char* device = "\\\\.\\COM16";
    uint32_t baud_rate = 57600;
    HANDLE port = open_serial_port(device, baud_rate);
    if (port == INVALID_HANDLE_VALUE) { return 1; }

    FILE* sim_data_file = fopen("SimulationData.txt", "r");
    FILE* actuator_output_file = fopen("ActuatorOutput.txt", "w");
    FILE* time_file = fopen("TimeConsumed.txt", "w");
    if (!sim_data_file || !actuator_output_file || !time_file)
    {
        fprintf(stderr, "Failed to open file\n");
        return 1;
    }

    char buffer[BUFFER_SIZE] = { 0 }; //sensor data belongs here


    int count = 0;
    while (1)
    {
        //original t_start location

        
        if (fgets(buffer, BUFFER_SIZE, sim_data_file) != NULL)
        {
            
            if (write_port(port, (uint8_t*)buffer, strlen(buffer)) != 0) printf("write failed at count: %d\n", count);
            //else  printf("Sent string: %s\n", buffer);

            // print_received_string(port);
            write_received_string(port, actuator_output_file); //this only takes 100ms fuck!
            
            //t_end = getWindowTime();

            count++;
            print_time_consumed(count, t_start, t_end, time_file);
            memset(buffer, 0, BUFFER_SIZE);
            printf("   cnt = %d\r", count);
        }
        else
        {
            break; // end of file reached
        }
        Sleep(100);
    }

    fclose(sim_data_file);
    fclose(actuator_output_file);
    fclose(time_file);
    CloseHandle(port);

    system("pause");
    return 0;
}











/*
int main()
{
    // Choose the serial port name.  If the Jrk is connected directly via USB,
    // you can run "jrk2cmd --cmd-port" to get the right name to use here.
    // COM ports higher than COM9 need the \\.\ prefix, which is written as
    // "\\\\.\\" in C because we need to escape the backslashes.
    const char* device = "\\\\.\\COM16";

    // Choose the baud rate (bits per second).  This does not matter if you are
    // connecting to the Jrk over USB.  If you are connecting via the TX and RX
    // lines, this should match the baud rate in the Jrk's serial settings.
    uint32_t baud_rate = 57600;

    HANDLE port = open_serial_port(device, baud_rate);
    if (port == INVALID_HANDLE_VALUE) { return 1; }




    while (1)
    {
        print_received_string(port);
        
        Sleep(500);
    }


    return 0;
}
*/
