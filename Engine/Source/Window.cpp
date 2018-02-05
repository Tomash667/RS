#include "Pch.h"
#include "Window.h"
#include "InputManager.h"


Window::Window(WindowHandler* handler, InputManager* input) : handler(handler), hwnd(nullptr), size(1024, 768), title("Window"), input(input), active(false),
activation_point(-1, -1), fullscreen(false), replace_cursor(false), locked_cursor(true), cursor_visible(true)
{
	assert(input && handler);
}

void Window::Init()
{
	RegisterClass();
	AdjustSize();
	Create();
	Center();

	ShowWindow((HWND)hwnd, SW_SHOWNORMAL);
	replace_cursor = true;
	unlock_point = real_size / 2;
	input->GetMouseMove() = Int2(0, 0);
}

bool Window::Tick()
{
	MSG msg = { 0 };
	while(true)
	{
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if(msg.message == WM_QUIT)
				return false;
		}
		else
		{
			//Sleep(1);



			// update activity state
			HWND foreground = GetForegroundWindow();
			bool is_active = (foreground == (HWND)hwnd);
			bool was_active = active;
			UpdateActivity(is_active);

			// handle cursor movement
			auto& mouse_dif = input->GetMouseMove();
			mouse_dif = Int2(0, 0);
			if(active)
			{
				if(locked_cursor)
				{
					if(replace_cursor)
						replace_cursor = false;
					else if(was_active)
					{
						POINT pt;
						GetCursorPos(&pt);
						ScreenToClient((HWND)hwnd, &pt);
						mouse_dif = Int2(pt.x, pt.y) - real_size / 2;
					}
					PlaceCursor();
				}
			}
			else if(!locked_cursor && lock_on_focus)
				locked_cursor = true;

			return true;
		}
	}
}

void Window::SetTitle(Cstring new_title)
{
	title = new_title;
	if(hwnd)
		SetWindowText((HWND)hwnd, new_title);
}

void Window::SetSize(const Int2& new_size)
{
	if(size == new_size)
		return;

	assert(hwnd == nullptr); // todo change after creating window

	size = new_size;
}

void Window::RegisterClass()
{
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		[](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
	{
		if(msg == WM_NCCREATE)
		{
			CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
			Window* window = (Window*)cs->lpCreateParams;
			window->hwnd = hwnd;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (IntPointer)cs->lpCreateParams);
		}
		else
		{
			Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if(window)
			{
				assert(hwnd == (HWND)window->hwnd);
				return window->HandleEvents(msg, wParam, lParam);
			}
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	},
		0, 0, GetModuleHandle(nullptr), LoadIcon(GetModuleHandle(nullptr), "Icon"), LoadCursor(nullptr, IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH),
		nullptr, "Krystal", nullptr
	};

	if(!RegisterClassEx(&wc))
		throw Format("Failed to register window class (%d).", GetLastError());
}

void Window::AdjustSize()
{
	if(!fullscreen)
	{
		Rect rect = Rect::Create(Int2(0, 0), size);
		AdjustWindowRect((RECT*)&rect, WS_OVERLAPPEDWINDOW, false);
		real_size = rect.Size();
	}
	else
		real_size = size;
}

void Window::Create()
{
	auto hwnd = CreateWindowEx(0, "Krystal", title.c_str(), fullscreen ? WS_POPUPWINDOW : WS_OVERLAPPEDWINDOW, 0, 0, real_size.x, real_size.y,
		nullptr, nullptr, GetModuleHandle(nullptr), this);
	if(!hwnd)
		throw Format("Failed to create window (%d).", GetLastError());
	this->hwnd = hwnd;
}

void Window::Center()
{
	MoveWindow((HWND)hwnd,
		(GetSystemMetrics(SM_CXSCREEN) - real_size.x) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - real_size.y) / 2,
		real_size.x, real_size.y, false);
}

IntPointer Window::HandleEvents(uint msg, IntPointer wParam, UIntPointer lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		input->Process((Key)wParam, true);
		return 0;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		input->Process((Key)wParam, false);
		return 0;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
		return ProcessMouseButton(msg);
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	default:
		return DefWindowProc((HWND)hwnd, msg, wParam, lParam);
	}
}

IntPointer Window::ProcessMouseButton(IntPointer wParam)
{
	IntPointer result;
	Key key;
	bool down;

	// translate msg to key
	switch(wParam)
	{
	case WM_LBUTTONDOWN:
		result = 0;
		key = Key::LeftButton;
		down = true;
		break;
	case WM_LBUTTONUP:
		result = 0;
		key = Key::LeftButton;
		down = false;
		break;
	case WM_RBUTTONDOWN:
		result = 0;
		key = Key::RightButton;
		down = true;
		break;
	case WM_RBUTTONUP:
		result = 0;
		key = Key::RightButton;
		down = false;
		break;
	case WM_MBUTTONDOWN:
		result = 0;
		key = Key::MiddleButton;
		down = true;
		break;
	case WM_MBUTTONUP:
		result = 0;
		key = Key::MiddleButton;
		down = false;
		break;
	case WM_XBUTTONDOWN:
		result = TRUE;
		key = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? Key::X1Button : Key::X2Button);
		down = true;
		break;
	case WM_XBUTTONUP:
		result = TRUE;
		key = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? Key::X1Button : Key::X2Button);
		down = false;
		break;
	}

	// handle click activation
	if((!locked_cursor || !active) && down && lock_on_focus)
	{
		ShowCursor(false);
		Rect rect;
		GetClientRect((HWND)hwnd, (RECT*)&rect);
		Int2 wh = rect.Size();
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient((HWND)hwnd, &pt);
		activation_point = Int2(pt.x * size.x / wh.x, pt.y * size.y / wh.y);
		PlaceCursor();

		if(active)
			locked_cursor = true;

		return result;
	}

	// send to input
	input->Process(key, down);
	return result;
}

void Window::UpdateActivity(bool is_active)
{
	if(is_active == active)
		return;
	active = is_active;
	if(active)
	{
		if(locked_cursor)
		{
			ShowCursor(false);
			PlaceCursor();
		}
	}
	else
	{
		ShowCursor(true);
		input->ReleaseKeys();
	}
	//OnFocus(active, activation_point);
	activation_point = Int2(-1, -1);
}

void Window::PlaceCursor()
{
	POINT p;
	p.x = real_size.x / 2;
	p.y = real_size.y / 2;
	ClientToScreen((HWND)hwnd, &p);
	SetCursorPos(p.x, p.y);
}

void Window::UnlockCursor(bool _lock_on_focus)
{
	lock_on_focus = _lock_on_focus;
	if(!locked_cursor)
		return;
	locked_cursor = false;

	if(!IsCursorVisible())
	{
		Rect rect;
		GetClientRect((HWND)hwnd, (RECT*)&rect);
		Int2 wh = rect.Size();
		POINT pt;
		pt.x = int(float(unlock_point.x)*wh.x / size.x);
		pt.y = int(float(unlock_point.y)*wh.y / size.y);
		ClientToScreen((HWND)hwnd, &pt);
		SetCursorPos(pt.x, pt.y);
	}

	ShowCursor(true);
}

void Window::LockCursor()
{
	if(locked_cursor)
		return;
	lock_on_focus = true;
	locked_cursor = true;
	if(active)
	{
		ShowCursor(false);
		PlaceCursor();
	}
}
