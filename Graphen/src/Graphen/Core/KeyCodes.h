#pragma once

namespace gn
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space               = VK_SPACE,
		Apostrophe          = 39, /* ' */
		Comma               = 44, /* , */
		Minus               = 45, /* - */
		Period              = 46, /* . */
		Slash               = 47, /* / */

		D0                  = 0x30, /* 0 */
		D1, /* 1 */
		D2, /* 2 */
		D3, /* 3 */
		D4, /* 4 */
		D5, /* 5 */
		D6, /* 6 */
		D7, /* 7 */
		D8, /* 8 */
		D9, /* 9 */

		Semicolon           = 59, /* ; */
		Equal               = 61, /* = */

		A                   = 0x41,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		LeftBracket         = 91,  /* [ */
		Backslash           = 92,  /* \ */
		RightBracket        = 93,  /* ] */
		GraveAccent         = 96,  /* ` */

		World1              = 161, /* non-US #1 */
		World2              = 162, /* non-US #2 */

		/* Function keys */
		Escape              = VK_ESCAPE,
		Enter               = VK_RETURN,
		Tab                 = VK_TAB,
		Backspace           = VK_BACK,
		Insert              = VK_INSERT,
		Delete              = VK_DELETE,
		Right               = VK_RIGHT,
		Left                = VK_LEFT,
		Down                = VK_DOWN,
		Up                  = VK_UP,
		PageUp              = VK_PRIOR,
		PageDown            = VK_NEXT,
		Home                = VK_HOME,
		End                 = VK_END,
		CapsLock            = VK_CAPITAL,
		ScrollLock          = VK_SCROLL,
		NumLock             = VK_NUMLOCK,
		PrintScreen         = VK_SNAPSHOT,
		Pause               = VK_PAUSE,
		F1                  = VK_F1,
		F2 ,
		F3 ,
		F4 ,
		F5 ,
		F6 ,
		F7 ,
		F8 ,
		F9 ,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		F25,

		/* Keypad */
		KP0                 = VK_NUMPAD0,
		KP1,
		KP2,
		KP3,
		KP4,
		KP5,
		KP6,
		KP7,
		KP8,
		KP9,
		KPDecimal           = 330,
		KPDivide            = 331,
		KPMultiply          = 332,
		KPSubtract          = 333,
		KPAdd               = 334,
		KPEnter             = 335,
		KPEqual             = 336,

		LeftShift           = VK_LSHIFT,
		LeftControl         = VK_CONTROL, //VK_LCONTROL, // todo:
		LeftAlt             = VK_MENU,
		LeftSuper           = VK_LWIN,
		RightShift          = VK_RSHIFT,
		RightControl        = VK_RCONTROL,
		RightAlt            = 346,
		RightSuper          = VK_RWIN,
		Menu                = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

// From glfw3.h
#define HZ_KEY_SPACE           ::gn::Key::Space
#define HZ_KEY_APOSTROPHE      ::gn::Key::Apostrophe    /* ' */
#define HZ_KEY_COMMA           ::gn::Key::Comma         /* , */
#define HZ_KEY_MINUS           ::gn::Key::Minus         /* - */
#define HZ_KEY_PERIOD          ::gn::Key::Period        /* . */
#define HZ_KEY_SLASH           ::gn::Key::Slash         /* / */
#define HZ_KEY_0               ::gn::Key::D0
#define HZ_KEY_1               ::gn::Key::D1
#define HZ_KEY_2               ::gn::Key::D2
#define HZ_KEY_3               ::gn::Key::D3
#define HZ_KEY_4               ::gn::Key::D4
#define HZ_KEY_5               ::gn::Key::D5
#define HZ_KEY_6               ::gn::Key::D6
#define HZ_KEY_7               ::gn::Key::D7
#define HZ_KEY_8               ::gn::Key::D8
#define HZ_KEY_9               ::gn::Key::D9
#define HZ_KEY_SEMICOLON       ::gn::Key::Semicolon     /* ; */
#define HZ_KEY_EQUAL           ::gn::Key::Equal         /* = */
#define HZ_KEY_A               ::gn::Key::A
#define HZ_KEY_B               ::gn::Key::B
#define HZ_KEY_C               ::gn::Key::C
#define HZ_KEY_D               ::gn::Key::D
#define HZ_KEY_E               ::gn::Key::E
#define HZ_KEY_F               ::gn::Key::F
#define HZ_KEY_G               ::gn::Key::G
#define HZ_KEY_H               ::gn::Key::H
#define HZ_KEY_I               ::gn::Key::I
#define HZ_KEY_J               ::gn::Key::J
#define HZ_KEY_K               ::gn::Key::K
#define HZ_KEY_L               ::gn::Key::L
#define HZ_KEY_M               ::gn::Key::M
#define HZ_KEY_N               ::gn::Key::N
#define HZ_KEY_O               ::gn::Key::O
#define HZ_KEY_P               ::gn::Key::P
#define HZ_KEY_Q               ::gn::Key::Q
#define HZ_KEY_R               ::gn::Key::R
#define HZ_KEY_S               ::gn::Key::S
#define HZ_KEY_T               ::gn::Key::T
#define HZ_KEY_U               ::gn::Key::U
#define HZ_KEY_V               ::gn::Key::V
#define HZ_KEY_W               ::gn::Key::W
#define HZ_KEY_X               ::gn::Key::X
#define HZ_KEY_Y               ::gn::Key::Y
#define HZ_KEY_Z               ::gn::Key::Z
#define HZ_KEY_LEFT_BRACKET    ::gn::Key::LeftBracket   /* [ */
#define HZ_KEY_BACKSLASH       ::gn::Key::Backslash     /* \ */
#define HZ_KEY_RIGHT_BRACKET   ::gn::Key::RightBracket  /* ] */
#define HZ_KEY_GRAVE_ACCENT    ::gn::Key::GraveAccent   /* ` */
#define HZ_KEY_WORLD_1         ::gn::Key::World1        /* non-US #1 */
#define HZ_KEY_WORLD_2         ::gn::Key::World2        /* non-US #2 */

/* Function keys */
#define HZ_KEY_ESCAPE          ::gn::Key::Escape
#define HZ_KEY_ENTER           ::gn::Key::Enter
#define HZ_KEY_TAB             ::gn::Key::Tab
#define HZ_KEY_BACKSPACE       ::gn::Key::Backspace
#define HZ_KEY_INSERT          ::gn::Key::Insert
#define HZ_KEY_DELETE          ::gn::Key::Delete
#define HZ_KEY_RIGHT           ::gn::Key::Right
#define HZ_KEY_LEFT            ::gn::Key::Left
#define HZ_KEY_DOWN            ::gn::Key::Down
#define HZ_KEY_UP              ::gn::Key::Up
#define HZ_KEY_PAGE_UP         ::gn::Key::PageUp
#define HZ_KEY_PAGE_DOWN       ::gn::Key::PageDown
#define HZ_KEY_HOME            ::gn::Key::Home
#define HZ_KEY_END             ::gn::Key::End
#define HZ_KEY_CAPS_LOCK       ::gn::Key::CapsLock
#define HZ_KEY_SCROLL_LOCK     ::gn::Key::ScrollLock
#define HZ_KEY_NUM_LOCK        ::gn::Key::NumLock
#define HZ_KEY_PRINT_SCREEN    ::gn::Key::PrintScreen
#define HZ_KEY_PAUSE           ::gn::Key::Pause
#define HZ_KEY_F1              ::gn::Key::F1
#define HZ_KEY_F2              ::gn::Key::F2
#define HZ_KEY_F3              ::gn::Key::F3
#define HZ_KEY_F4              ::gn::Key::F4
#define HZ_KEY_F5              ::gn::Key::F5
#define HZ_KEY_F6              ::gn::Key::F6
#define HZ_KEY_F7              ::gn::Key::F7
#define HZ_KEY_F8              ::gn::Key::F8
#define HZ_KEY_F9              ::gn::Key::F9
#define HZ_KEY_F10             ::gn::Key::F10
#define HZ_KEY_F11             ::gn::Key::F11
#define HZ_KEY_F12             ::gn::Key::F12
#define HZ_KEY_F13             ::gn::Key::F13
#define HZ_KEY_F14             ::gn::Key::F14
#define HZ_KEY_F15             ::gn::Key::F15
#define HZ_KEY_F16             ::gn::Key::F16
#define HZ_KEY_F17             ::gn::Key::F17
#define HZ_KEY_F18             ::gn::Key::F18
#define HZ_KEY_F19             ::gn::Key::F19
#define HZ_KEY_F20             ::gn::Key::F20
#define HZ_KEY_F21             ::gn::Key::F21
#define HZ_KEY_F22             ::gn::Key::F22
#define HZ_KEY_F23             ::gn::Key::F23
#define HZ_KEY_F24             ::gn::Key::F24
#define HZ_KEY_F25             ::gn::Key::F25

/* Keypad */
#define HZ_KEY_KP_0            ::gn::Key::KP0
#define HZ_KEY_KP_1            ::gn::Key::KP1
#define HZ_KEY_KP_2            ::gn::Key::KP2
#define HZ_KEY_KP_3            ::gn::Key::KP3
#define HZ_KEY_KP_4            ::gn::Key::KP4
#define HZ_KEY_KP_5            ::gn::Key::KP5
#define HZ_KEY_KP_6            ::gn::Key::KP6
#define HZ_KEY_KP_7            ::gn::Key::KP7
#define HZ_KEY_KP_8            ::gn::Key::KP8
#define HZ_KEY_KP_9            ::gn::Key::KP9
#define HZ_KEY_KP_DECIMAL      ::gn::Key::KPDecimal
#define HZ_KEY_KP_DIVIDE       ::gn::Key::KPDivide
#define HZ_KEY_KP_MULTIPLY     ::gn::Key::KPMultiply
#define HZ_KEY_KP_SUBTRACT     ::gn::Key::KPSubtract
#define HZ_KEY_KP_ADD          ::gn::Key::KPAdd
#define HZ_KEY_KP_ENTER        ::gn::Key::KPEnter
#define HZ_KEY_KP_EQUAL        ::gn::Key::KPEqual

#define HZ_KEY_LEFT_SHIFT      ::gn::Key::LeftShift
#define HZ_KEY_LEFT_CONTROL    ::gn::Key::LeftControl
#define HZ_KEY_LEFT_ALT        ::gn::Key::LeftAlt
#define HZ_KEY_LEFT_SUPER      ::gn::Key::LeftSuper
#define HZ_KEY_RIGHT_SHIFT     ::gn::Key::RightShift
#define HZ_KEY_RIGHT_CONTROL   ::gn::Key::RightControl
#define HZ_KEY_RIGHT_ALT       ::gn::Key::RightAlt
#define HZ_KEY_RIGHT_SUPER     ::gn::Key::RightSuper
#define HZ_KEY_MENU            ::gn::Key::Menu
