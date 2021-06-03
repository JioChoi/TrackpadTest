#include <iostream>
#include <Windows.h>
#include <vector>

#include <SDL.h>
#include <SDL_syswm.h>

struct touchData {
	BYTE whatever;
	BYTE x;
	BYTE xm;
	BYTE y;
	BYTE ym;
};

int main(int argc, char* argv[]) {
	SDL_Window* window = SDL_CreateWindow("Gesture", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIDDEN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	
	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;
	
	RAWINPUTDEVICE raw_input_device[1];
	// Starting with Windows 7, multitouch digitizers appear as HID touch digitizers (page 0x0D, usage 0x04), 
	// but they also contain the contact ID usage in their report descriptor (page 0x0D, usage 0x51).
	raw_input_device[0].usUsagePage = 0x0D;
	// RIDEV_PAGEONLY specifies all devices whose top level collection is from the specified usUsagePage.
	// Note that usUsage must be zero.
	raw_input_device[0].dwFlags = RIDEV_INPUTSINK | RIDEV_PAGEONLY;
	raw_input_device[0].usUsage = 0x00;
	// route the RAWINPUT messages to our window; this is required for the RIDEV_INPUTSINK option
	raw_input_device[0].hwndTarget = hwnd;
	// listen to digitizer events
	RegisterRawInputDevices(raw_input_device, 1, sizeof(raw_input_device[0]));

	SDL_Event event;
	POINT prv = { 0, 0 };
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	while (true) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_SYSWMEVENT) {
				if (event.syswm.msg->msg.win.msg == WM_INPUT) {
					UINT data_size = 0;
					GetRawInputData((HRAWINPUT)event.syswm.msg->msg.win.lParam, RID_INPUT, NULL, &data_size, sizeof(RAWINPUTHEADER));
					if (data_size <= 0) {
						continue;
					}
					std::vector<BYTE> data;
					data.resize(data_size);
					// and then read the input data in
					GetRawInputData((HRAWINPUT)event.syswm.msg->msg.win.lParam, RID_INPUT, &data[0], &data_size, sizeof(RAWINPUTHEADER));
					RAWINPUT* raw = (RAWINPUT*)(&data[0]);

					if (raw->header.dwType == RIM_TYPEHID) {
						RID_DEVICE_INFO device_info;
						UINT info_size(sizeof(RID_DEVICE_INFO));
						GetRawInputDeviceInfo(raw->header.hDevice, RIDI_DEVICEINFO, (LPVOID)&device_info, &info_size);

						std::cout << device_info.hid.dwProductId << "\t" << device_info.hid.dwVendorId << std::endl;

						//system("cls");
						int touchNum = 0;
						POINT point = { 0,0 };

						for (int group = 0; group < 5; group++) {
							touchData* td = (touchData*)&raw->data.hid.bRawData[group * 5 + 1];
							if (td->whatever != 0) {
								touchNum++;
							}
							point.x += (int)td->x + (td->xm * 255);
							point.y += (int)td->y + (td->ym * 255);
						}

						/*
						system("cls");
						for (int group = 0; group < 30; group++) {
							std::cout << (int)raw->data.hid.bRawData[group] << std::endl;
						}*/
						
						if (touchNum == 0) {
							break;
						}
						else if (touchNum == 1) {
							SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
							SDL_RenderClear(renderer);
							break;
						}

						SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
						SDL_RenderDrawLine(renderer, point.x / touchNum, point.y / touchNum, prv.x, prv.y);
						prv.x = (int)(point.x / touchNum);
						prv.y = (int)(point.y / touchNum);
					}
				}
			}
		}

		SDL_RenderPresent(renderer);
	}

	return 0;
}