#include <iostream>
#include <Windows.h>
#include <vector>

#include <SDL.h>
#include <SDL_syswm.h>

int main(int argc, char* argv[]) {
	SDL_Window* window = SDL_CreateWindow("Gesture", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, NULL);
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

	while (true) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_SYSWMEVENT) {
				if (event.syswm.msg->msg.win.msg == WM_INPUT) {
					UINT data_size = 0;
					GetRawInputData((HRAWINPUT)event.syswm.msg->msg.win.lParam, RID_INPUT, NULL, &data_size, sizeof(RAWINPUTHEADER));
					if (data_size < 0) {
						continue;
					}
					std::vector<BYTE> data;
					data.resize(data_size);
					// and then read the input data in
					GetRawInputData((HRAWINPUT)event.syswm.msg->msg.win.lParam, RID_INPUT, &data[0], &data_size, sizeof(RAWINPUTHEADER));
					RAWINPUT* raw = (RAWINPUT*)(&data[0]);

					if (raw->header.dwType == RIM_TYPEHID) {
						std::cout << "SDF\n";
					}
				}
			}
		}
	}

	return 0;
}