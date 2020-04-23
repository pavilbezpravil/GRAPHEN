#pragma once

#include "pch.h"

namespace gn
{
	typedef enum class KeyCode : uint16_t
	{
		Space               = 32,
		Apostrophe          = 39, /* ' */
		Comma               = 44, /* , */
		Minus               = 45, /* - */
		Period              = 46, /* . */
		Slash               = 47, /* / */

		D0                  = 48, /* 0 */
		D1                  = 49, /* 1 */
		D2                  = 50, /* 2 */
		D3                  = 51, /* 3 */
		D4                  = 52, /* 4 */
		D5                  = 53, /* 5 */
		D6                  = 54, /* 6 */
		D7                  = 55, /* 7 */
		D8                  = 56, /* 8 */
		D9                  = 57, /* 9 */

		Semicolon           = 59, /* ; */
		Equal               = 61, /* = */

		A                   = 65,
		B                   = 66,
		C                   = 67,
		D                   = 68,
		E                   = 69,
		F                   = 70,
		G                   = 71,
		H                   = 72,
		I                   = 73,
		J                   = 74,
		K                   = 75,
		L                   = 76,
		M                   = 77,
		N                   = 78,
		O                   = 79,
		P                   = 80,
		Q                   = 81,
		R                   = 82,
		S                   = 83,
		T                   = 84,
		U                   = 85,
		V                   = 86,
		W                   = 87,
		X                   = 88,
		Y                   = 89,
		Z                   = 90,

		LeftBracket         = 91,  /* [ */
		Backslash           = 92,  /* \ */
		RightBracket        = 93,  /* ] */
		GraveAccent         = 96,  /* ` */

		World1              = 161, /* non-US #1 */
		World2              = 162, /* non-US #2 */

		/* Function keys */
		Escape              = 256,
		Enter               = 257,
		Tab                 = 258,
		Backspace           = 259,
		Insert              = 260,
		Delete              = 261,
		Right               = 262,
		Left                = 263,
		Down                = 264,
		Up                  = 265,
		PageUp              = 266,
		PageDown            = 267,
		Home                = 268,
		End                 = 269,
		CapsLock            = 280,
		ScrollLock          = 281,
		NumLock             = 282,
		PrintScreen         = 283,
		Pause               = 284,
		F1                  = 290,
		F2                  = 291,
		F3                  = 292,
		F4                  = 293,
		F5                  = 294,
		F6                  = 295,
		F7                  = 296,
		F8                  = 297,
		F9                  = 298,
		F10                 = 299,
		F11                 = 300,
		F12                 = 301,
		F13                 = 302,
		F14                 = 303,
		F15                 = 304,
		F16                 = 305,
		F17                 = 306,
		F18                 = 307,
		F19                 = 308,
		F20                 = 309,
		F21                 = 310,
		F22                 = 311,
		F23                 = 312,
		F24                 = 313,
		F25                 = 314,

		/* Keypad */
		KP0                 = 320,
		KP1                 = 321,
		KP2                 = 322,
		KP3                 = 323,
		KP4                 = 324,
		KP5                 = 325,
		KP6                 = 326,
		KP7                 = 327,
		KP8                 = 328,
		KP9                 = 329,
		KPDecimal           = 330,
		KPDivide            = 331,
		KPMultiply          = 332,
		KPSubtract          = 333,
		KPAdd               = 334,
		KPEnter             = 335,
		KPEqual             = 336,

		LeftShift           = 340,
		LeftControl         = 341,
		LeftAlt             = 342,
		LeftSuper           = 343,
		RightShift          = 344,
		RightControl        = 345,
		RightAlt            = 346,
		RightSuper          = 347,
		Menu                = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

#define GN_KEY_SPACE           ::gn::Key::Space
#define GN_KEY_APOSTROPHE      ::gn::Key::Apostrophe    /* ' */
#define GN_KEY_COMMA           ::gn::Key::Comma         /* , */
#define GN_KEY_MINUS           ::gn::Key::Minus         /* - */
#define GN_KEY_PERIOD          ::gn::Key::Period        /* . */
#define GN_KEY_SLASH           ::gn::Key::Slash         /* / */
#define GN_KEY_0               ::gn::Key::D0
#define GN_KEY_1               ::gn::Key::D1
#define GN_KEY_2               ::gn::Key::D2
#define GN_KEY_3               ::gn::Key::D3
#define GN_KEY_4               ::gn::Key::D4
#define GN_KEY_5               ::gn::Key::D5
#define GN_KEY_6               ::gn::Key::D6
#define GN_KEY_7               ::gn::Key::D7
#define GN_KEY_8               ::gn::Key::D8
#define GN_KEY_9               ::gn::Key::D9
#define GN_KEY_SEMICOLON       ::gn::Key::Semicolon     /* ; */
#define GN_KEY_EQUAL           ::gn::Key::Equal         /* = */
#define GN_KEY_A               ::gn::Key::A
#define GN_KEY_B               ::gn::Key::B
#define GN_KEY_C               ::gn::Key::C
#define GN_KEY_D               ::gn::Key::D
#define GN_KEY_E               ::gn::Key::E
#define GN_KEY_F               ::gn::Key::F
#define GN_KEY_G               ::gn::Key::G
#define GN_KEY_H               ::gn::Key::H
#define GN_KEY_I               ::gn::Key::I
#define GN_KEY_J               ::gn::Key::J
#define GN_KEY_K               ::gn::Key::K
#define GN_KEY_L               ::gn::Key::L
#define GN_KEY_M               ::gn::Key::M
#define GN_KEY_N               ::gn::Key::N
#define GN_KEY_O               ::gn::Key::O
#define GN_KEY_P               ::gn::Key::P
#define GN_KEY_Q               ::gn::Key::Q
#define GN_KEY_R               ::gn::Key::R
#define GN_KEY_S               ::gn::Key::S
#define GN_KEY_T               ::gn::Key::T
#define GN_KEY_U               ::gn::Key::U
#define GN_KEY_V               ::gn::Key::V
#define GN_KEY_W               ::gn::Key::W
#define GN_KEY_X               ::gn::Key::X
#define GN_KEY_Y               ::gn::Key::Y
#define GN_KEY_Z               ::gn::Key::Z
#define GN_KEY_LEFT_BRACKET    ::gn::Key::LeftBracket   /* [ */
#define GN_KEY_BACKSLASH       ::gn::Key::Backslash     /* \ */
#define GN_KEY_RIGHT_BRACKET   ::gn::Key::RightBracket  /* ] */
#define GN_KEY_GRAVE_ACCENT    ::gn::Key::GraveAccent   /* ` */
#define GN_KEY_WORLD_1         ::gn::Key::World1        /* non-US #1 */
#define GN_KEY_WORLD_2         ::gn::Key::World2        /* non-US #2 */

/* Function keys */
#define GN_KEY_ESCAPE          ::gn::Key::Escape
#define GN_KEY_ENTER           ::gn::Key::Enter
#define GN_KEY_TAB             ::gn::Key::Tab
#define GN_KEY_BACKSPACE       ::gn::Key::Backspace
#define GN_KEY_INSERT          ::gn::Key::Insert
#define GN_KEY_DELETE          ::gn::Key::Delete
#define GN_KEY_RIGHT           ::gn::Key::Right
#define GN_KEY_LEFT            ::gn::Key::Left
#define GN_KEY_DOWN            ::gn::Key::Down
#define GN_KEY_UP              ::gn::Key::Up
#define GN_KEY_PAGE_UP         ::gn::Key::PageUp
#define GN_KEY_PAGE_DOWN       ::gn::Key::PageDown
#define GN_KEY_HOME            ::gn::Key::Home
#define GN_KEY_END             ::gn::Key::End
#define GN_KEY_CAPS_LOCK       ::gn::Key::CapsLock
#define GN_KEY_SCROLL_LOCK     ::gn::Key::ScrollLock
#define GN_KEY_NUM_LOCK        ::gn::Key::NumLock
#define GN_KEY_PRINT_SCREEN    ::gn::Key::PrintScreen
#define GN_KEY_PAUSE           ::gn::Key::Pause
#define GN_KEY_F1              ::gn::Key::F1
#define GN_KEY_F2              ::gn::Key::F2
#define GN_KEY_F3              ::gn::Key::F3
#define GN_KEY_F4              ::gn::Key::F4
#define GN_KEY_F5              ::gn::Key::F5
#define GN_KEY_F6              ::gn::Key::F6
#define GN_KEY_F7              ::gn::Key::F7
#define GN_KEY_F8              ::gn::Key::F8
#define GN_KEY_F9              ::gn::Key::F9
#define GN_KEY_F10             ::gn::Key::F10
#define GN_KEY_F11             ::gn::Key::F11
#define GN_KEY_F12             ::gn::Key::F12
#define GN_KEY_F13             ::gn::Key::F13
#define GN_KEY_F14             ::gn::Key::F14
#define GN_KEY_F15             ::gn::Key::F15
#define GN_KEY_F16             ::gn::Key::F16
#define GN_KEY_F17             ::gn::Key::F17
#define GN_KEY_F18             ::gn::Key::F18
#define GN_KEY_F19             ::gn::Key::F19
#define GN_KEY_F20             ::gn::Key::F20
#define GN_KEY_F21             ::gn::Key::F21
#define GN_KEY_F22             ::gn::Key::F22
#define GN_KEY_F23             ::gn::Key::F23
#define GN_KEY_F24             ::gn::Key::F24
#define GN_KEY_F25             ::gn::Key::F25

/* Keypad */
#define GN_KEY_KP_0            ::gn::Key::KP0
#define GN_KEY_KP_1            ::gn::Key::KP1
#define GN_KEY_KP_2            ::gn::Key::KP2
#define GN_KEY_KP_3            ::gn::Key::KP3
#define GN_KEY_KP_4            ::gn::Key::KP4
#define GN_KEY_KP_5            ::gn::Key::KP5
#define GN_KEY_KP_6            ::gn::Key::KP6
#define GN_KEY_KP_7            ::gn::Key::KP7
#define GN_KEY_KP_8            ::gn::Key::KP8
#define GN_KEY_KP_9            ::gn::Key::KP9
#define GN_KEY_KP_DECIMAL      ::gn::Key::KPDecimal
#define GN_KEY_KP_DIVIDE       ::gn::Key::KPDivide
#define GN_KEY_KP_MULTIPLY     ::gn::Key::KPMultiply
#define GN_KEY_KP_SUBTRACT     ::gn::Key::KPSubtract
#define GN_KEY_KP_ADD          ::gn::Key::KPAdd
#define GN_KEY_KP_ENTER        ::gn::Key::KPEnter
#define GN_KEY_KP_EQUAL        ::gn::Key::KPEqual

#define GN_KEY_LEFT_SHIFT      ::gn::Key::LeftShift
#define GN_KEY_LEFT_CONTROL    ::gn::Key::LeftControl
#define GN_KEY_LEFT_ALT        ::gn::Key::LeftAlt
#define GN_KEY_LEFT_SUPER      ::gn::Key::LeftSuper
#define GN_KEY_RIGHT_SHIFT     ::gn::Key::RightShift
#define GN_KEY_RIGHT_CONTROL   ::gn::Key::RightControl
#define GN_KEY_RIGHT_ALT       ::gn::Key::RightAlt
#define GN_KEY_RIGHT_SUPER     ::gn::Key::RightSuper
#define GN_KEY_MENU            ::gn::Key::Menu
