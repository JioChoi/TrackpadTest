#include <iostream>
#include <Windows.h>

#include <SDL.h>
#include <SDL_syswm.h>

int main(int argc, char* argv[]) {
	SDL_Window* window = SDL_CreateWindow("Gesture", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, NULL);
	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;
	
	RAWINPUTDEVICE rawInputDevice;
	rawInputDevice.usUsagePage = 0x01;
	rawInputDevice.usUsage = 0x30;
	rawInputDevice.dwFlags = RIDEV_NOLEGACY;
	rawInputDevice.hwndTarget = hwnd;

	RegisterRawInputDevices(&rawInputDevice, 1, sizeof(RAWINPUTDEVICE));
	SDL_Event event;

	while (true) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_SYSWMEVENT) {
				if (event.syswm.msg->msg.win.msg == WM_INPUT) {
					std::cout << "SDF\n";
					/*UINT dwSize = 0;
					GetRawInputData((HRAWINPUT)event.syswm.msg->msg.win.lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
					if (dwSize > 0) {
						LPBYTE lpb = new BYTE[dwSize];
						GetRawInputData((HRAWINPUT)event.syswm.msg->msg.win.lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
						RAWINPUT* raw = (RAWINPUT*)lpb;
						if (raw->header.dwType == RIM_TYPEMOUSE) {
							std::cout << raw->data.mouse.ulExtraInformation << std::endl;
						}
					}*/
					
				}
			}
		}
	}

	return 0;
}