#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")

#include "utils.h"

#include <windows.h>
#include <winuser.h>

global_variable bool running = true;

struct Render_State {
    int height, width;
    void* memory;

    BITMAPINFO bitmap_info;
};

global_variable Render_State render_state;

global_variable float fps;

#include "renderer.h"
#include "platform_common.cpp"

#include "game.cpp"

WNDPROC Wndproc;

LRESULT CALLBACK window_callback(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
) {
    LRESULT result;

    switch(uMsg) {
        case WM_CLOSE:
        case WM_DESTROY: {
            running = false;
        } break;

        case WM_SIZE: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            render_state.width = rect.right - rect.left;
            render_state.height = rect.bottom - rect.top;

            int size = render_state.width * render_state.height * sizeof(u32);
            
            if (render_state.memory) {
                VirtualFree(render_state.memory, 0, MEM_RELEASE);
            }
            
            render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
            render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
            render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
            render_state.bitmap_info.bmiHeader.biPlanes = 1;
            render_state.bitmap_info.bmiHeader.biBitCount = 32;
            render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
        }

        default: {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return result;
} 

int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd
) {
    //Create window class
    WNDCLASS window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = "Game Window Class";
    window_class.lpfnWndProc = window_callback;
    

    //Register class
    RegisterClass(&window_class);
    
    //Create window
    HWND window = CreateWindow(
        window_class.lpszClassName, 
        "THE TERORIST",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1280, 720,
        NULL, NULL,
        hInstance,
        NULL
    );
    HDC hdc = GetDC(window);

    Input input = {};
    
    float delta_time = 0.016666f;
    LARGE_INTEGER frame_begin_time;
    QueryPerformanceCounter(&frame_begin_time);

    float performance_frequency;
    {
        LARGE_INTEGER perf;
        QueryPerformanceFrequency(&perf);
        performance_frequency = (float)perf.QuadPart;
    }

    while (running) {
        //Input
        MSG message;

        for(int i = 0; i < BUTTON_COUNT; i++) {
            input.buttons[i].changed = false;
        }

        while(PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
            
            switch (message.message) {
                case WM_KEYUP:
                case WM_KEYDOWN: {
                    u32 vk_code = (u32)message.wParam;
                    bool is_down = ((message.lParam & (1 << 31)) == 0);

#define process_button(b, vk) \
case vk: { \
    bool is_down = (GetAsyncKeyState(vk) & 0x8000) != 0; \
    if (is_down != input.buttons[b].is_down) { \
        input.buttons[b].changed = true; \
        input.buttons[b].is_down = is_down; \
    } else { \
        input.buttons[b].changed = false; \
    } \
} break;

                    switch (vk_code) {
                        process_button(BUTTON_UP, VK_UP);
                        process_button(BUTTON_DOWN, VK_DOWN);
                        process_button(BUTTON_W, 0x57);
                        process_button(BUTTON_S, 0x53);
                        process_button(BUTTON_SPACE, VK_SPACE);
                    }
                } break;

                default: {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
            }            
        }
        
        //Simulate
        simulate_game(&input, delta_time);

        //Render
        StretchDIBits(
            hdc, 0, 0,
            render_state.width, render_state.height,
            0, 0, 
            render_state.width, render_state.height,
            render_state.memory,
            &render_state.bitmap_info,
            DIB_RGB_COLORS,
            SRCCOPY
        );

        //Text
        if (game_time < (int)(game_time) + 0.01) {
            fps = 1.0f / delta_time;
        }

        char buffer[256];
        
        sprintf(buffer, "Score: %d - %d   FPS: %.1f", game_score_2, game_score_1, fps);

        TextOutA(hdc, 20, 20, buffer, strlen(buffer));

        //Delta time type shit
        LARGE_INTEGER frame_end_time;
        QueryPerformanceCounter(&frame_end_time);
        delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
        frame_begin_time = frame_end_time;
    }

    return 0;
}
