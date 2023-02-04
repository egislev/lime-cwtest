#include<windows.h>
#include<stdio.h>
#include<conio.h>
#include "lime/LimeSuite.h"
#include <iostream>
#include <chrono>

using namespace std;

#define	BUFFER_SIZE 1024 * 8

lms_device_t	*device = NULL;
lms_stream_t	tx_stream,rx_stream;
float			tx_buffer[2 * BUFFER_SIZE];
const double	frequency = 145.5e6;
const double	sample_rate = 4e6;
const double	tone_freq = 1000;
const double	f_ratio = tone_freq / sample_rate;
// --------------------------------------------------------------------------------------------------------------------------
int error()
{
	if (device != NULL)
		LMS_Close(device);
	exit(-1);
}
// --------------------------------------------------------------------------------------------------------------------------
void	lime_init ()
{
	int				n, rc = 0;
	lms_info_str_t	list[8];

	if ((n = LMS_GetDeviceList(list)) < 0) {
		cout << "LMS_GetDeviceList error" << endl;
		error();
	}
	if (!n) {
		cout << "No devices found." << endl;
		error();
	}
	cout << "Devices found: " << n << endl;
	if (LMS_Open(&device, list[0], NULL))
		error();
	if (LMS_Init(device) != 0)
		error();
	if (LMS_EnableChannel(device, LMS_CH_TX, 0, true) != 0)
		error();
	if (LMS_EnableChannel(device, LMS_CH_RX, 0, true) != 0)
		error();
	if (LMS_SetLOFrequency(device, LMS_CH_TX, 0, 145.5e6) != 0)
		error();
	if (LMS_SetLOFrequency(device, LMS_CH_RX, 0, 145.5e6) != 0)
		error();
	if (LMS_SetSampleRate(device, 8e6, 2) != 0)
		error();
	if (LMS_SetAntenna(device, LMS_CH_TX, 0, LMS_PATH_TX1) != 0)
		error();
	if (LMS_SetNormalizedGain(device, LMS_CH_TX, 0, 1) != 0)
		error();
	LMS_Calibrate(device, LMS_CH_TX, 0, sample_rate, 0);
	tx_stream.channel = 0;
	tx_stream.fifoSize = 256 * 1024;
	tx_stream.throughputVsLatency = 0.5;
	tx_stream.dataFmt = lms_stream_t::LMS_FMT_F32;
	tx_stream.isTx = true;
	LMS_SetupStream(device, &tx_stream);
	rx_stream.channel = 0;
	rx_stream.fifoSize = 256 * 1024;
	rx_stream.throughputVsLatency = 1.0;
	rx_stream.isTx = false;
	rx_stream.dataFmt = lms_stream_t::LMS_FMT_F32;
	LMS_SetupStream(device, &rx_stream);
}
// --------------------------------------------------------------------------------------------------------------------------
DWORD WINAPI	tx_thread(LPVOID lpParam)
{

	for (int i = 0; i < BUFFER_SIZE; i++) { // TX tone
		const double pi = acos(-1);
		double w = 2 * pi*i*f_ratio;
		tx_buffer[2 * i] = cos(w);
		tx_buffer[2 * i + 1] = sin(w);
	}
	const int send_cnt = int(BUFFER_SIZE * f_ratio) / f_ratio;
	LMS_StartStream(&tx_stream);
	while (1) {
		int ret = LMS_SendStream(&tx_stream, tx_buffer, send_cnt, nullptr, 1000);
		if (ret != send_cnt)
			cout << "error: samples sent: " << ret << "/" << send_cnt << endl;
	}
	return (0);
}
// --------------------------------------------------------------------------------------------------------------------------
DWORD WINAPI	rx_thread(LPVOID lpParam)
{
	const int bufersize = 10000;
	float buffer[bufersize * 2];
	LMS_StartStream(&rx_stream);

	while (1) {
		int samplesRead = LMS_RecvStream(&rx_stream, buffer, bufersize, NULL, 1000);
	}
	return (0);
}
// --------------------------------------------------------------------------------------------------------------------------
DWORD WINAPI	status_thread(LPVOID lpParam)
{
	auto t1 = chrono::high_resolution_clock::now();
	while (1) {
		if (chrono::high_resolution_clock::now() - t1 > chrono::seconds(1))
		{
			t1 = chrono::high_resolution_clock::now();
			lms_stream_status_t status;
			LMS_GetStreamStatus(&rx_stream, &status);
			cout << "RX rate: " << status.linkRate / 1e6 << " MB/s " << "RX fifo: " << 100 * status.fifoFilledCount / status.fifoSize << "% ";
			LMS_GetStreamStatus(&tx_stream, &status);
			cout << "TX rate: " << status.linkRate / 1e6 << " MB/s " << "TX fifo: " << 100 * status.fifoFilledCount / status.fifoSize << "%                  \r";
		}
	}
	return (0);
}
// --------------------------------------------------------------------------------------------------------------------------
int main ()
{
	HANDLE	txt, rxt,st;
	DWORD	txtid, rxtid,stid;

	cout << "Lime CW test utility" << endl;

	lime_init();
	txt = CreateThread(NULL, 0, tx_thread, NULL, 0, &txtid);
	rxt = CreateThread(NULL, 0, rx_thread, NULL, 0, &rxtid);
	st = CreateThread(NULL, 0, status_thread, NULL, 0, &stid);
	cout << "Test is running ... Press Ctrl-C to stop." << endl;
	while (1) {}
	return (0);
}
