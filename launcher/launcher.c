// Simple Win32 GUI launcher for Astonia server + client.
// Builds with MinGW: gcc -mwindows -O2 -o astonia-launcher.exe launcher.c

#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>

static HWND hStartServer;
static HWND hStartClient;
static HWND hStartBoth;
static HWND hServerHostEdit;
static HWND hServerPortEdit;
static HWND hStatus;
static wchar_t g_basePath[MAX_PATH];

static void SetStatus(const wchar_t *fmt, ...) {
    wchar_t buf[1024];
    va_list args;
    va_start(args, fmt);
    vswprintf_s(buf, _countof(buf), fmt, args);
    va_end(args);
    SetWindowTextW(hStatus, buf);
}

static void LaunchProcess(const wchar_t *path, const wchar_t *args) {
    STARTUPINFOW si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    wchar_t cmdline[MAX_PATH * 2];
    if (args && args[0]) {
        swprintf_s(cmdline, _countof(cmdline), L"\"%s\" %s", path, args);
    } else {
        swprintf_s(cmdline, _countof(cmdline), L"\"%s\"", path);
    }

    if (!CreateProcessW(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        SetStatus(L"Failed to start %s (error %d)", path, GetLastError());
        return;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    SetStatus(L"Started %s", path);
}

static void GetEditText(HWND hEdit, wchar_t *out, int outSize) {
    GetWindowTextW(hEdit, out, outSize);
}

static void OnStartServer() {
    wchar_t serverPath[MAX_PATH];
    PathCombineW(serverPath, g_basePath, L"astonia_community_server3\\server.exe");

    wchar_t portStr[32] = L"";
    GetEditText(hServerPortEdit, portStr, _countof(portStr));

    wchar_t args[64] = L"";
    if (portStr[0] != L'\0') {
        swprintf_s(args, _countof(args), L"-p %s", portStr);
    }

    LaunchProcess(serverPath, args);
}

static void OnStartClient() {
    wchar_t clientPath[MAX_PATH];
    PathCombineW(clientPath, g_basePath, L"astonia_community_server3\\astonia_community_client\\bin\\moac.exe");

    wchar_t host[128] = L"";
    GetEditText(hServerHostEdit, host, _countof(host));

    wchar_t portStr[32] = L"";
    GetEditText(hServerPortEdit, portStr, _countof(portStr));

    wchar_t args[256] = L"";
    if (host[0] != L'\0') {
        swprintf_s(args, _countof(args), L"-d %s", host);
    }
    if (portStr[0] != L'\0') {
        wcscat_s(args, _countof(args), L" -t ");
        wcscat_s(args, _countof(args), portStr);
    }

    LaunchProcess(clientPath, args);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        CreateWindowW(L"STATIC", L"Server Host:", WS_CHILD | WS_VISIBLE | SS_LEFT,
                      10, 10, 80, 20, hwnd, NULL, NULL, NULL);
        hServerHostEdit = CreateWindowW(L"EDIT", L"127.0.0.1", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                        90, 10, 140, 20, hwnd, NULL, NULL, NULL);

        CreateWindowW(L"STATIC", L"Server Port:", WS_CHILD | WS_VISIBLE | SS_LEFT,
                      240, 10, 80, 20, hwnd, NULL, NULL, NULL);
        hServerPortEdit = CreateWindowW(L"EDIT", L"5556", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                        320, 10, 70, 20, hwnd, NULL, NULL, NULL);

        hStartServer = CreateWindowW(L"BUTTON", L"Start Server", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                     10, 40, 120, 30, hwnd, (HMENU)1, NULL, NULL);
        hStartClient = CreateWindowW(L"BUTTON", L"Start Client", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                     140, 40, 120, 30, hwnd, (HMENU)2, NULL, NULL);
        hStartBoth = CreateWindowW(L"BUTTON", L"Start Both", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                   270, 40, 120, 30, hwnd, (HMENU)3, NULL, NULL);
        hStatus = CreateWindowW(L"STATIC", L"Ready", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                10, 80, 380, 25, hwnd, NULL, NULL, NULL);
    } break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1:
            OnStartServer();
            break;
        case 2:
            OnStartClient();
            break;
        case 3:
            OnStartServer();
            OnStartClient();
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    GetModuleFileNameW(NULL, g_basePath, MAX_PATH);
    PathRemoveFileSpecW(g_basePath);

    const wchar_t CLASS_NAME[] = L"AstoniaLauncherClass";

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"Astonia Launcher", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
                                CW_USEDEFAULT, CW_USEDEFAULT, 420, 180, NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return wWinMain(hInstance, hPrevInstance, GetCommandLineW(), nCmdShow);
}
