#pragma once

struct Window
{
	typedef void* Handle;

	Window(WindowHandler* handler, InputManager* input);
	void Init();
	bool Tick();
	void UnlockCursor(bool lock_on_focus);
	void LockCursor();

	bool IsActive() const { return active; }
	bool IsCursorLocked() const { return locked_cursor; }
	bool IsCursorVisible() const { return cursor_visible; }

	Handle GetHandle() { return hwnd; }
	const Int2& GetSize() const { return size; }

	void SetTitle(Cstring title);
	void SetSize(const Int2& size);

private:
	void RegisterClass();
	void AdjustSize();
	void Create();
	void Center();
	IntPointer HandleEvents(uint msg, IntPointer wParam, UIntPointer lParam);
	IntPointer ProcessMouseButton(IntPointer wParam);
	void UpdateActivity(bool is_active);
	void PlaceCursor();

	WindowHandler* handler;
	InputManager* input;
	Handle hwnd;
	string title;
	Int2 size, real_size, unlock_point, activation_point;
	bool fullscreen, active, replace_cursor, locked_cursor, lock_on_focus, cursor_visible;
};

