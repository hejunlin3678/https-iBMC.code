import { USB_KEY_BACK, USB_KEY_TAB, USB_KEY_ENTER, USB_KEY_SHIFT, USB_KEY_CTRL, USB_KEY_ALT, USB_KEY_BREAK, USB_KEY_CAPS,
    USB_KEY_ESC, USB_KEY_SPACE, USB_KEY_PGUP, USB_KEY_PGDN, USB_KEY_END, USB_KEY_HOME, USB_KEY_LEFT, USB_KEY_UP, USB_KEY_RIGHT,
    USB_KEY_DOWN, USB_KEY_COMMA, USB_KEY_DIV, USB_KEY_DOT, USB_KEY_7, USB_KEY_0, USB_KEY_1, USB_KEY_2, USB_KEY_3, USB_KEY_4, USB_KEY_5,
    USB_KEY_6, USB_KEY_8, USB_KEY_9,  USB_KEY_A, USB_KEY_B, USB_KEY_C, USB_KEY_D, USB_KEY_E, USB_KEY_F, USB_KEY_G, USB_KEY_H, USB_KEY_I,
    USB_KEY_J, USB_KEY_K, USB_KEY_L, USB_KEY_M, USB_KEY_N, USB_KEY_O, USB_KEY_P, USB_KEY_Q, USB_KEY_R, USB_KEY_S, USB_KEY_T, USB_KEY_U,
    USB_KEY_V, USB_KEY_W, USB_KEY_X, USB_KEY_Z, USB_KEY_Y, USB_KEY_0P, USB_KEY_1P, USB_KEY_2P, USB_KEY_3P, USB_KEY_4P, USB_KEY_5P,
    USB_KEY_6P, USB_KEY_7P, USB_KEY_8P, USB_KEY_9P, USB_KEY_MULP, USB_KEY_PLUSP, USB_KEY_MINP, USB_KEY_DELP, USB_KEY_DIVP, USB_KEY_DEL,
    USB_KEY_NUMP, USB_KEY_PRNT, USB_KEY_INSERT, USB_KEY_F1, USB_KEY_F2, USB_KEY_F3, USB_KEY_F4, USB_KEY_F5, USB_KEY_F6, USB_KEY_F7,
    USB_KEY_F8, USB_KEY_F9, USB_KEY_F10, USB_KEY_F11, USB_KEY_F12, USB_KEY_MENU, USB_KEY_QUOTO, USB_KEY_OR, USB_KEY_OR_1,
    USB_KEY_BR2, USB_KEY_SCROLL, USB_KEY_MIN, USB_KEY_BR1, USB_KEY_SEMI, USB_KEY_QUOT, USB_KEY_ALTGRAPH, USB_KEY_PLUS, USB_KEY_KANJI3,
    USB_KEY_ENTERP, USB_KEY_KANJI1, USB_KEY_KANJI5, USB_KEY_KANJI4, USB_KEY_KANJI2, JS_KEY_SHIFT, JS_KEY_CTRL, JS_KEY_ALT, JS_KEY_LWIN,
    JS_KEY_RWIN, JS_KEY_NUM, JS_KEY_CAPS, JS_KEY_SCROLL, JS_KEY_TAB, JS_KEY_ESC, JS_KEY_SPACE, JS_KEY_DEL, JS_KEY_0P, JS_KEY_9P,
    JS_KEY_DELP, JS_KEY_PAGEUP, JS_KEY_DOWN, JS_KEY_CLEAR, JS_KEY_INSERT, uskeyboardTable } from 'src/app/common-app/utils';
import { packet } from './packet';

// USB_KEY和JS_KEY以及USKeyboardTable在CommonCtrl.js中定义
const JP_KEY_ESC = 0x1b;
const JP_KEY_F1 = 0x70;
const JP_KEY_F2 = 0x71;
const JP_KEY_F3 = 0x72;
const JP_KEY_F4 = 0x73;
const JP_KEY_F5 = 0x74;
const JP_KEY_F6 = 0x75;
const JP_KEY_F7 = 0x76;
const JP_KEY_F8 = 0x77;
const JP_KEY_F9 = 0x78;
const JP_KEY_F10 = 0x79;
const JP_KEY_F11 = 0x7a;
const JP_KEY_F12 = 0x7b;
const JP_KEY_HALF = 0xf4;
const JP_KEY_FULL = 0xf3;
const JP_KEY_0 = 0x30;
const JP_KEY_1 = 0x31;
const JP_KEY_2 = 0x32;
const JP_KEY_3 = 0x33;
const JP_KEY_4 = 0x34;
const JP_KEY_5 = 0x35;
const JP_KEY_6 = 0x36;
const JP_KEY_7 = 0x37;
const JP_KEY_8 = 0x38;
const JP_KEY_9 = 0x39;
const JP_KEY_EQUAL = 0xbd;
const JP_KEY_BOLANG = 0xde;
const JP_KEY_INTLYEN = 0xdc;
const JP_KEY_BACKSPACE = 0x08;
const JP_KEY_TAB = 0x09;
const JP_KEY_A = 0x41;
const JP_KEY_B = 0x42;
const JP_KEY_C = 0x43;
const JP_KEY_D = 0x44;
const JP_KEY_E = 0x45;
const JP_KEY_F = 0x46;
const JP_KEY_G = 0x47;
const JP_KEY_H = 0x48;
const JP_KEY_I = 0x49;
const JP_KEY_J = 0x4a;
const JP_KEY_K = 0x4b;
const JP_KEY_L = 0x4c;
const JP_KEY_M = 0x4d;
const JP_KEY_N = 0x4e;
const JP_KEY_O = 0x4f;
const JP_KEY_P = 0x50;
const JP_KEY_Q = 0x51;
const JP_KEY_R = 0x52;
const JP_KEY_S = 0x53;
const JP_KEY_T = 0x54;
const JP_KEY_U = 0x55;
const JP_KEY_V = 0x56;
const JP_KEY_W = 0x57;
const JP_KEY_X = 0x58;
const JP_KEY_Y = 0x59;
const JP_KEY_Z = 0x5a;
const JP_KEY_AT = 0xc0;
const JP_KEY_BR1 = 0xdb;
const JP_KEY_ENTER = 0x0d;
const JP_KEY_SEMI = 0xbb;
const JP_KEY_COLON = 0xba;
const JP_KEY_BR2 = 0xdd;
const JP_KEY_COMMA = 0xbc;
const JP_KEY_PERIOD = 0xbe;
const JP_KEY_SLASH = 0xbf;
const JP_KEY_INTLRO = 0xe2;
const JP_KEY_NONCONVERT = 0x1d;
const JP_KEY_SPACE = 0x20;
const JP_KEY_CONVERT = 0x1c;
const JP_KEY_KARAMODE = 0xf2;
const JP_KEY_CONTEXTMENU = 0x5d;
const JP_KEY_CAPSLOCK = 0x14;
const JP_KEY_INSERT = 0x2d;
const JP_KEY_DELETE = 0x2e;
const JP_KEY_HOME = 0x24;
const JP_KEY_END = 0x23;
const JP_KEY_PAGEUP = 0x21;
const JP_KEY_PAGEDOWN = 0x22;
const JP_KEY_SCROLL = 0x91;
const JP_KEY_PAUSE = 0x13;
const JP_KEY_NUMLOCK = 0x90;
const JP_KEY_DIVIDE = 0x6f;
const JP_KEY_MULTY = 0x6a;
const JP_KEY_DECREA = 0x6d;
const JP_KEY_ADD = 0x6b;
const JP_KEY_0P = 0x60;
const JP_KEY_1P = 0x61;
const JP_KEY_2P = 0x62;
const JP_KEY_3P = 0x63;
const JP_KEY_4P = 0x64;
const JP_KEY_5P = 0x65;
const JP_KEY_6P = 0x66;
const JP_KEY_7P = 0x67;
const JP_KEY_8P = 0x68;
const JP_KEY_9P = 0x69;
const JP_KEY_DECIMAL = 0x6e;
const JP_KEY_LEFT = 0x25;
const JP_KEY_UP = 0x26;
const JP_KEY_RIGHT = 0x27;
const JP_KEY_DOWN = 0x28;
const JP_KEY_AT2 = 0x40;
const JP_KEY_BOLANG2 = 0xa0;
const JP_KEY_EQUAL2 = 0xad;
const JP_KEY_COLON2 = 0x3a;
const JP_KEY_SEMI2 = 0x3b;
const JP_KEY_CONVERT2 = 0x19;

const IT_KEY_ESC = 0x1b;
const IT_KEY_F1 = 0x70;
const IT_KEY_F2 = 0x71;
const IT_KEY_F3 = 0x72;
const IT_KEY_F4 = 0x73;
const IT_KEY_F5 = 0x74;
const IT_KEY_F6 = 0x75;
const IT_KEY_F7 = 0x76;
const IT_KEY_F8 = 0x77;
const IT_KEY_F9 = 0x78;
const IT_KEY_F10 = 0x79;
const IT_KEY_F11 = 0x7a;
const IT_KEY_F12 = 0x7b;
const IT_KEY_BACK_SLISH = 0xdc;
const IT_KEY_0 = 0x30;
const IT_KEY_1 = 0x31;
const IT_KEY_2 = 0x32;
const IT_KEY_3 = 0x33;
const IT_KEY_4 = 0x34;
const IT_KEY_5 = 0x35;
const IT_KEY_6 = 0x36;
const IT_KEY_7 = 0x37;
const IT_KEY_8 = 0x38;
const IT_KEY_9 = 0x39;
const IT_KEY_QUOTE = 0xde;
const IT_KEY_BACKSPACE = 0x08;
const IT_KEY_TAB = 0x09;
const IT_KEY_A = 0x41;
const IT_KEY_B = 0x42;
const IT_KEY_C = 0x43;
const IT_KEY_D = 0x44;
const IT_KEY_E = 0x45;
const IT_KEY_F = 0x46;
const IT_KEY_G = 0x47;
const IT_KEY_H = 0x48;
const IT_KEY_I = 0x49;
const IT_KEY_J = 0x4a;
const IT_KEY_K = 0x4b;
const IT_KEY_L = 0x4c;
const IT_KEY_M = 0x4d;
const IT_KEY_N = 0x4e;
const IT_KEY_O = 0x4f;
const IT_KEY_P = 0x50;
const IT_KEY_Q = 0x51;
const IT_KEY_R = 0x52;
const IT_KEY_S = 0x53;
const IT_KEY_T = 0x54;
const IT_KEY_U = 0x55;
const IT_KEY_V = 0x56;
const IT_KEY_W = 0x57;
const IT_KEY_X = 0x58;
const IT_KEY_Y = 0x59;
const IT_KEY_Z = 0x5a;
const IT_KEY_CAPSLOCK = 0x14;
const IT_KEY_CONTEXTMENU = 0x5d;
const IT_KEY_COMMA = 0xbc;
const IT_KEY_PERIOD = 0xbe;
const IT_KEY_MINUS = 0xad;
const IT_KEY_SPACE = 0x20;
const IT_KEY_INSERT = 0x2d;
const IT_KEY_DELETE = 0x2e;
const IT_KEY_HOME = 0x24;
const IT_KEY_END = 0x23;
const IT_KEY_PAGEUP = 0x21;
const IT_KEY_PAGEDOWN = 0x22;
const IT_KEY_LEFT = 0x25;
const IT_KEY_RIGHT = 0x27;
const IT_KEY_UP = 0x26;
const IT_KEY_DOWN = 0x28;
const IT_KEY_SCROLL = 0x91;
const IT_KEY_PAUSE = 0x13;
const IT_KEY_NUMLOCK = 0x90;
const IT_KEY_DIVIDE = 0x6f;
const IT_KEY_MULTY = 0x6a;
const IT_KEY_DECREA = 0x6d;
const IT_KEY_ADD = 0x6b;
const IT_KEY_DECIMAL = 0x6e;
const IT_KEY_0P = 0x60;
const IT_KEY_1P = 0x61;
const IT_KEY_2P = 0x62;
const IT_KEY_3P = 0x63;
const IT_KEY_4P = 0x64;
const IT_KEY_5P = 0x65;
const IT_KEY_6P = 0x66;
const IT_KEY_7P = 0x67;
const IT_KEY_8P = 0x68;
const IT_KEY_9P = 0x69;
const IT_KEY_SHIFT = 0x10;
const IT_KEY_CTRL = 0x11;
const IT_KEY_ALT = 0x12;
const IT_KEY_ENTER = 0x0d;
const IT_KEY_ALTGRPH = 0xe1;
const IT_KEY_SQUOTE = 0xdb;
const IT_KEY_UNDERLINE = 0xbd;
const IT_KEY_EITE = 0xc0;
const IT_KEY_JIN = 0xde;
const IT_KEY_LEFTFK = 0xba;
const IT_KEY_RIGHTFK = 0xbb;
const IT_KEY_SPEU = 0xbf;
const IT_KEY_LEFTJK = 0xe2;
const IT_KEY_SPEI = 0xdd;

const DE_KEY_BACKSPACE = 0x08;
const DE_KEY_TAB = 0x09;
const DE_KEY_ENTER = 0x0d;
const DE_KEY_SHIFT = 0x10;
const DE_KEY_CONTROL = 0x11;
const DE_KEY_ALT = 0x12;
const DE_KEY_PAUSE = 0x13;
const DE_KEY_CAPS_LOCK = 0x14;
const DE_KEY_ESPACE = 0x1b;
const DE_KEY_SPACE = 0x20;
const DE_KEY_PAGE_UP = 0x21;
const DE_KEY_PAGE_DOWN = 0x22;
const DE_KEY_END = 0x23;
const DE_KEY_HOME = 0x24;
const DE_KEY_LEFT = 0x25;
const DE_KEY_UP = 0x26;
const DE_KEY_RIGHT = 0x27;
const DE_KEY_DOWN = 0x28;
const DE_KEY_COMMA = 0xbc;
const DE_KEY_MINUS = 0xbd;
const DE_KEY_PERIOD = 0xbe;
const DE_KEY_SLASH = 0x37;
const DE_KEY_0 = 0x30;
const DE_KEY_1 = 0x31;
const DE_KEY_2 = 0x32;
const DE_KEY_3 = 0x33;
const DE_KEY_4 = 0x34;
const DE_KEY_5 = 0x35;
const DE_KEY_6 = 0x36;
const DE_KEY_7 = 0x37;
const DE_KEY_8 = 0x38;
const DE_KEY_9 = 0x39;
const DE_KEY_BACK_QUOTE = 0xdd;
const DE_KEY_A = 0x41;
const DE_KEY_B = 0x42;
const DE_KEY_C = 0x43;
const DE_KEY_D = 0x44;
const DE_KEY_E = 0x45;
const DE_KEY_F = 0x46;
const DE_KEY_G = 0x47;
const DE_KEY_H = 0x48;
const DE_KEY_I = 0x49;
const DE_KEY_J = 0x4a;
const DE_KEY_K = 0x4b;
const DE_KEY_L = 0x4c;
const DE_KEY_M = 0x4d;
const DE_KEY_N = 0x4e;
const DE_KEY_O = 0x4f;
const DE_KEY_P = 0x50;
const DE_KEY_Q = 0x51;
const DE_KEY_R = 0x52;
const DE_KEY_S = 0x53;
const DE_KEY_T = 0x54;
const DE_KEY_U = 0x55;
const DE_KEY_V = 0x56;
const DE_KEY_W = 0x57;
const DE_KEY_X = 0x58;
const DE_KEY_Y = 0x59;
const DE_KEY_Z = 0x5a;
const DE_KEY_NUM0 = 0x60;
const DE_KEY_NUM1 = 0x61;
const DE_KEY_NUM2 = 0x62;
const DE_KEY_NUM3 = 0x63;
const DE_KEY_NUM4 = 0x64;
const DE_KEY_NUM5 = 0x65;
const DE_KEY_NUM6 = 0x66;
const DE_KEY_NUM7 = 0x67;
const DE_KEY_NUM8 = 0x68;
const DE_KEY_NUM9 = 0x69;
const DE_KEY_MULTIPLY = 0x6a;
const DE_KEY_ADD = 0x6b;
const DE_KEY_SUBTRACT = 0x6d;
const DE_KEY_DECIMAL = 0x6e;
const DE_KEY_DIVIDE = 0x6f;
const DE_KEY_DELETE = 0x2e;
const DE_KEY_NUMLOCK = 0x90;
const DE_KEY_F1 = 0x70;
const DE_KEY_F2 = 0x71;
const DE_KEY_F3 = 0x72;
const DE_KEY_F4 = 0x73;
const DE_KEY_F5 = 0x74;
const DE_KEY_F6 = 0x75;
const DE_KEY_F7 = 0x76;
const DE_KEY_F8 = 0x77;
const DE_KEY_F9 = 0x78;
const DE_KEY_F10 = 0x79;
const DE_KEY_F11 = 0x7a;
const DE_KEY_F12 = 0x7b;
const DE_KEY_PRINTSCREEN = 0x9a;
const DE_KEY_INSERT = 0x9b;
const DE_KEY_CONTEXT_MENU = 0x5d;
const DE_KEY_ASCII_CIRCUM = 0xdc;
const DE_KEY_MINUTE = 0xdd;
const DE_KEY_NUMBERSIGN = 0xbf;
const DE_KEY_LESS = 0xe2;
const DE_KEY_PLUS = 0xbb;
const DE_KEY_SCROLL_LOCK = 0x91;
const DE_KEY_MINUS_FOX = 0xad;
const DE_KEY_BACK_QUOTE_FOX = 0xc0;
const DE_KEY_NUMBERSIGN_FOX = 0xa3;
const DE_KEY_BACK_SLASH = 0xdb;
const DE_KEY_BACK_SLASH_FOX = 0x3f;
const DE_KEY_ASCII_CIRCUM_FOX = 0xa0;
const DE_KEY_LESS_FOX = 0x3c;
const DE_KEY_PLUS_FOX = 0xab;
const DE_KEY_DE_U = 0xba;
const DE_KEY_DE_UE = 0xc0;
const DE_KEY_DE_AI = 0xde;
const DE_KEY_SEPARATER = 0x6c;

const deKeyboardTable = new Array(
    [DE_KEY_BACKSPACE, USB_KEY_BACK],
    [DE_KEY_TAB, USB_KEY_TAB],
    [DE_KEY_ENTER, USB_KEY_ENTER],
    [DE_KEY_SHIFT, USB_KEY_SHIFT],
    [DE_KEY_CONTROL, USB_KEY_CTRL],
    [DE_KEY_ALT, USB_KEY_ALT],
    [DE_KEY_PAUSE, USB_KEY_BREAK],
    [DE_KEY_CAPS_LOCK, USB_KEY_CAPS],
    [DE_KEY_ESPACE, USB_KEY_ESC],
    [DE_KEY_SPACE, USB_KEY_SPACE],
    [DE_KEY_PAGE_UP, USB_KEY_PGUP],
    [DE_KEY_PAGE_DOWN, USB_KEY_PGDN],
    [DE_KEY_END, USB_KEY_END],
    [DE_KEY_HOME, USB_KEY_HOME],
    [DE_KEY_LEFT, USB_KEY_LEFT],
    [DE_KEY_UP, USB_KEY_UP],
    [DE_KEY_RIGHT, USB_KEY_RIGHT],
    [DE_KEY_DOWN, USB_KEY_DOWN],
    [DE_KEY_COMMA, USB_KEY_COMMA],
    [DE_KEY_MINUS, USB_KEY_DIV],
    [DE_KEY_PERIOD, USB_KEY_DOT],
    [DE_KEY_SLASH, USB_KEY_7],
    [DE_KEY_0, USB_KEY_0],
    [DE_KEY_1, USB_KEY_1],
    [DE_KEY_2, USB_KEY_2],
    [DE_KEY_3, USB_KEY_3],
    [DE_KEY_4, USB_KEY_4],
    [DE_KEY_5, USB_KEY_5],
    [DE_KEY_6, USB_KEY_6],
    [DE_KEY_7, USB_KEY_7],
    [DE_KEY_8, USB_KEY_8],
    [DE_KEY_9, USB_KEY_9],
    [DE_KEY_A, USB_KEY_A],
    [DE_KEY_B, USB_KEY_B],
    [DE_KEY_C, USB_KEY_C],
    [DE_KEY_D, USB_KEY_D],
    [DE_KEY_E, USB_KEY_E],
    [DE_KEY_F, USB_KEY_F],
    [DE_KEY_G, USB_KEY_G],
    [DE_KEY_H, USB_KEY_H],
    [DE_KEY_I, USB_KEY_I],
    [DE_KEY_J, USB_KEY_J],
    [DE_KEY_K, USB_KEY_K],
    [DE_KEY_L, USB_KEY_L],
    [DE_KEY_M, USB_KEY_M],
    [DE_KEY_N, USB_KEY_N],
    [DE_KEY_O, USB_KEY_O],
    [DE_KEY_P, USB_KEY_P],
    [DE_KEY_Q, USB_KEY_Q],
    [DE_KEY_R, USB_KEY_R],
    [DE_KEY_S, USB_KEY_S],
    [DE_KEY_T, USB_KEY_T],
    [DE_KEY_U, USB_KEY_U],
    [DE_KEY_V, USB_KEY_V],
    [DE_KEY_W, USB_KEY_W],
    [DE_KEY_X, USB_KEY_X],
    [DE_KEY_Y, USB_KEY_Z],
    [DE_KEY_Z, USB_KEY_Y],
    [DE_KEY_O, USB_KEY_O],
    [DE_KEY_P, USB_KEY_P],
    [DE_KEY_Q, USB_KEY_Q],
    [DE_KEY_NUM0, USB_KEY_0P],
    [DE_KEY_NUM1, USB_KEY_1P],
    [DE_KEY_NUM2, USB_KEY_2P],
    [DE_KEY_NUM3, USB_KEY_3P],
    [DE_KEY_NUM4, USB_KEY_4P],
    [DE_KEY_NUM5, USB_KEY_5P],
    [DE_KEY_NUM6, USB_KEY_6P],
    [DE_KEY_NUM7, USB_KEY_7P],
    [DE_KEY_NUM8, USB_KEY_8P],
    [DE_KEY_NUM9, USB_KEY_9P],
    [DE_KEY_MULTIPLY, USB_KEY_MULP],
    [DE_KEY_ADD, USB_KEY_PLUSP],
    [DE_KEY_SUBTRACT, USB_KEY_MINP],
    [DE_KEY_DECIMAL, USB_KEY_DELP],
    [DE_KEY_DIVIDE, USB_KEY_DIVP],
    [DE_KEY_DELETE, USB_KEY_DEL],
    [DE_KEY_NUMLOCK, USB_KEY_NUMP],
    [DE_KEY_PRINTSCREEN, USB_KEY_PRNT],
    [DE_KEY_INSERT, USB_KEY_INSERT],
    [DE_KEY_F1, USB_KEY_F1],
    [DE_KEY_F2, USB_KEY_F2],
    [DE_KEY_F3, USB_KEY_F3],
    [DE_KEY_F4, USB_KEY_F4],
    [DE_KEY_F5, USB_KEY_F5],
    [DE_KEY_F6, USB_KEY_F6],
    [DE_KEY_F7, USB_KEY_F7],
    [DE_KEY_F8, USB_KEY_F8],
    [DE_KEY_F9, USB_KEY_F9],
    [DE_KEY_F10, USB_KEY_F10],
    [DE_KEY_F11, USB_KEY_F11],
    [DE_KEY_F12, USB_KEY_F12],
    [DE_KEY_CONTEXT_MENU, USB_KEY_MENU],
    [DE_KEY_ASCII_CIRCUM, USB_KEY_QUOTO],
    [DE_KEY_NUMBERSIGN, USB_KEY_OR],
    [DE_KEY_LESS, USB_KEY_OR_1],
    [DE_KEY_PLUS, USB_KEY_BR2],
    [DE_KEY_SCROLL_LOCK, USB_KEY_SCROLL],
    [DE_KEY_MINUS_FOX, USB_KEY_DIV],
    [DE_KEY_NUMBERSIGN_FOX, USB_KEY_OR],
    [DE_KEY_BACK_SLASH, USB_KEY_MIN],
    [DE_KEY_BACK_SLASH_FOX, USB_KEY_MIN],
    [DE_KEY_ASCII_CIRCUM_FOX, USB_KEY_QUOTO],
    [DE_KEY_LESS_FOX, USB_KEY_OR_1],
    [DE_KEY_PLUS_FOX, USB_KEY_BR2],
    [DE_KEY_DE_U, USB_KEY_BR1],
    [DE_KEY_DE_UE, USB_KEY_SEMI],
    [DE_KEY_DE_AI, USB_KEY_QUOT],
    [DE_KEY_SEPARATER, USB_KEY_DELP]
);

const itKeyboardTable = new Array(
    [IT_KEY_ESC, USB_KEY_ESC],
    [IT_KEY_F1, USB_KEY_F1],
    [IT_KEY_F2, USB_KEY_F2],
    [IT_KEY_F3, USB_KEY_F3],
    [IT_KEY_F4, USB_KEY_F4],
    [IT_KEY_F5, USB_KEY_F5],
    [IT_KEY_F6, USB_KEY_F6],
    [IT_KEY_F7, USB_KEY_F7],
    [IT_KEY_F8, USB_KEY_F8],
    [IT_KEY_F9, USB_KEY_F9],
    [IT_KEY_F10, USB_KEY_F10],
    [IT_KEY_F11, USB_KEY_F11],
    [IT_KEY_F12, USB_KEY_F12],
    [IT_KEY_BACK_SLISH, USB_KEY_QUOTO],
    [IT_KEY_0, USB_KEY_0],
    [IT_KEY_1, USB_KEY_1],
    [IT_KEY_2, USB_KEY_2],
    [IT_KEY_3, USB_KEY_3],
    [IT_KEY_4, USB_KEY_4],
    [IT_KEY_5, USB_KEY_5],
    [IT_KEY_6, USB_KEY_6],
    [IT_KEY_7, USB_KEY_7],
    [IT_KEY_8, USB_KEY_8],
    [IT_KEY_9, USB_KEY_9],
    [IT_KEY_BACKSPACE, USB_KEY_BACK],
    [IT_KEY_TAB, USB_KEY_TAB],
    [IT_KEY_A, USB_KEY_A],
    [IT_KEY_B, USB_KEY_B],
    [IT_KEY_C, USB_KEY_C],
    [IT_KEY_D, USB_KEY_D],
    [IT_KEY_E, USB_KEY_E],
    [IT_KEY_F, USB_KEY_F],
    [IT_KEY_G, USB_KEY_G],
    [IT_KEY_H, USB_KEY_H],
    [IT_KEY_I, USB_KEY_I],
    [IT_KEY_J, USB_KEY_J],
    [IT_KEY_K, USB_KEY_K],
    [IT_KEY_L, USB_KEY_L],
    [IT_KEY_M, USB_KEY_M],
    [IT_KEY_N, USB_KEY_N],
    [IT_KEY_O, USB_KEY_O],
    [IT_KEY_P, USB_KEY_P],
    [IT_KEY_Q, USB_KEY_Q],
    [IT_KEY_R, USB_KEY_R],
    [IT_KEY_S, USB_KEY_S],
    [IT_KEY_T, USB_KEY_T],
    [IT_KEY_U, USB_KEY_U],
    [IT_KEY_V, USB_KEY_V],
    [IT_KEY_W, USB_KEY_W],
    [IT_KEY_X, USB_KEY_X],
    [IT_KEY_Y, USB_KEY_Y],
    [IT_KEY_Z, USB_KEY_Z],
    [IT_KEY_CAPSLOCK, USB_KEY_CAPS],
    [IT_KEY_CONTEXTMENU, USB_KEY_MENU],
    [IT_KEY_INSERT, USB_KEY_INSERT],
    [IT_KEY_DELETE, USB_KEY_DEL],
    [IT_KEY_HOME, USB_KEY_HOME],
    [IT_KEY_END, USB_KEY_END],
    [IT_KEY_PAGEUP, USB_KEY_PGUP],
    [IT_KEY_PAGEDOWN, USB_KEY_PGDN],
    [IT_KEY_LEFT, USB_KEY_LEFT],
    [IT_KEY_RIGHT, USB_KEY_RIGHT],
    [IT_KEY_UP, USB_KEY_UP],
    [IT_KEY_DOWN, USB_KEY_DOWN],
    [IT_KEY_SCROLL, USB_KEY_SCROLL],
    [IT_KEY_PAUSE, USB_KEY_BREAK],
    [IT_KEY_NUMLOCK, USB_KEY_NUMP],
    [IT_KEY_DIVIDE, USB_KEY_DIVP],
    [IT_KEY_MULTY, USB_KEY_MULP],
    [IT_KEY_DECREA, USB_KEY_MINP],
    [IT_KEY_ADD, USB_KEY_PLUSP],
    [IT_KEY_DECIMAL, USB_KEY_DELP],
    [IT_KEY_0P, USB_KEY_0P],
    [IT_KEY_1P, USB_KEY_1P],
    [IT_KEY_2P, USB_KEY_2P],
    [IT_KEY_3P, USB_KEY_3P],
    [IT_KEY_4P, USB_KEY_4P],
    [IT_KEY_5P, USB_KEY_5P],
    [IT_KEY_6P, USB_KEY_6P],
    [IT_KEY_7P, USB_KEY_7P],
    [IT_KEY_8P, USB_KEY_8P],
    [IT_KEY_9P, USB_KEY_9P],
    [IT_KEY_SHIFT, USB_KEY_SHIFT],
    [IT_KEY_CTRL, USB_KEY_CTRL],
    [IT_KEY_ALT, USB_KEY_ALT],
    [IT_KEY_ENTER, USB_KEY_ENTER],
    [IT_KEY_ALTGRPH, USB_KEY_ALTGRAPH],
    [IT_KEY_COMMA, USB_KEY_COMMA],
    [IT_KEY_PERIOD, USB_KEY_DOT],
    [IT_KEY_MINUS, USB_KEY_DIV],
    [IT_KEY_SPACE, USB_KEY_SPACE],
    [IT_KEY_SQUOTE, USB_KEY_MIN],
    [IT_KEY_UNDERLINE, USB_KEY_DIV],
    [IT_KEY_EITE, USB_KEY_SEMI],
    [IT_KEY_JIN, USB_KEY_QUOT],
    [IT_KEY_LEFTFK, USB_KEY_BR1],
    [IT_KEY_RIGHTFK, USB_KEY_BR2],
    [IT_KEY_SPEU, USB_KEY_OR],
    [IT_KEY_LEFTJK, USB_KEY_OR_1],
    [IT_KEY_SPEI, USB_KEY_PLUS]
);

const jpKeyboardTable = new Array(
    [JP_KEY_ESC, USB_KEY_ESC],
    [JP_KEY_F1, USB_KEY_F1],
    [JP_KEY_F2, USB_KEY_F2],
    [JP_KEY_F3, USB_KEY_F3],
    [JP_KEY_F4, USB_KEY_F4],
    [JP_KEY_F5, USB_KEY_F5],
    [JP_KEY_F6, USB_KEY_F6],
    [JP_KEY_F7, USB_KEY_F7],
    [JP_KEY_F8, USB_KEY_F8],
    [JP_KEY_F9, USB_KEY_F9],
    [JP_KEY_F10, USB_KEY_F10],
    [JP_KEY_F11, USB_KEY_F11],
    [JP_KEY_F12, USB_KEY_F12],
    [JP_KEY_HALF, USB_KEY_QUOTO],
    [JP_KEY_FULL, USB_KEY_QUOTO],
    [JP_KEY_0, USB_KEY_0],
    [JP_KEY_1, USB_KEY_1],
    [JP_KEY_2, USB_KEY_2],
    [JP_KEY_3, USB_KEY_3],
    [JP_KEY_4, USB_KEY_4],
    [JP_KEY_5, USB_KEY_5],
    [JP_KEY_6, USB_KEY_6],
    [JP_KEY_7, USB_KEY_7],
    [JP_KEY_8, USB_KEY_8],
    [JP_KEY_9, USB_KEY_9],
    [JP_KEY_EQUAL, USB_KEY_MIN],
    [JP_KEY_BOLANG, USB_KEY_PLUS],
    [JP_KEY_INTLYEN, USB_KEY_KANJI3],
    [JP_KEY_BACKSPACE, USB_KEY_BACK],
    [JP_KEY_TAB, USB_KEY_TAB],
    [JP_KEY_A, USB_KEY_A],
    [JP_KEY_B, USB_KEY_B],
    [JP_KEY_C, USB_KEY_C],
    [JP_KEY_D, USB_KEY_D],
    [JP_KEY_E, USB_KEY_E],
    [JP_KEY_F, USB_KEY_F],
    [JP_KEY_G, USB_KEY_G],
    [JP_KEY_H, USB_KEY_H],
    [JP_KEY_I, USB_KEY_I],
    [JP_KEY_J, USB_KEY_J],
    [JP_KEY_K, USB_KEY_K],
    [JP_KEY_L, USB_KEY_L],
    [JP_KEY_M, USB_KEY_M],
    [JP_KEY_N, USB_KEY_N],
    [JP_KEY_O, USB_KEY_O],
    [JP_KEY_P, USB_KEY_P],
    [JP_KEY_Q, USB_KEY_Q],
    [JP_KEY_R, USB_KEY_R],
    [JP_KEY_S, USB_KEY_S],
    [JP_KEY_T, USB_KEY_T],
    [JP_KEY_U, USB_KEY_U],
    [JP_KEY_V, USB_KEY_V],
    [JP_KEY_W, USB_KEY_W],
    [JP_KEY_X, USB_KEY_X],
    [JP_KEY_Y, USB_KEY_Y],
    [JP_KEY_Z, USB_KEY_Z],
    [JP_KEY_AT, USB_KEY_BR1],
    [JP_KEY_BR1, USB_KEY_BR2],
    [JP_KEY_ENTER, USB_KEY_ENTERP],
    [JP_KEY_SEMI, USB_KEY_SEMI],
    [JP_KEY_COLON, USB_KEY_QUOT],
    [JP_KEY_BR2, USB_KEY_OR],
    [JP_KEY_COMMA, USB_KEY_COMMA],
    [JP_KEY_PERIOD, USB_KEY_DOT],
    [JP_KEY_SLASH, USB_KEY_DIV],
    [JP_KEY_INTLRO, USB_KEY_KANJI1],
    [JP_KEY_NONCONVERT, USB_KEY_KANJI5],
    [JP_KEY_SPACE, USB_KEY_SPACE],
    [JP_KEY_CONVERT, USB_KEY_KANJI4],
    [JP_KEY_KARAMODE, USB_KEY_KANJI2],
    [JP_KEY_CONTEXTMENU, USB_KEY_MENU],
    [JP_KEY_CAPSLOCK, USB_KEY_CAPS],
    [JP_KEY_INSERT, USB_KEY_INSERT],
    [JP_KEY_DELETE, USB_KEY_DEL],
    [JP_KEY_HOME, USB_KEY_HOME],
    [JP_KEY_END, USB_KEY_END],
    [JP_KEY_PAGEUP, USB_KEY_PGUP],
    [JP_KEY_PAGEDOWN, USB_KEY_PGDN],
    [JP_KEY_SCROLL, USB_KEY_SCROLL],
    [JP_KEY_PAUSE, USB_KEY_BREAK],
    [JP_KEY_NUMLOCK, USB_KEY_NUMP],
    [JP_KEY_DIVIDE, USB_KEY_DIVP],
    [JP_KEY_MULTY, USB_KEY_MULP],
    [JP_KEY_DECREA, USB_KEY_MINP],
    [JP_KEY_ADD, USB_KEY_PLUSP],
    [JP_KEY_0P, USB_KEY_0P],
    [JP_KEY_1P, USB_KEY_1P],
    [JP_KEY_2P, USB_KEY_2P],
    [JP_KEY_3P, USB_KEY_3P],
    [JP_KEY_4P, USB_KEY_4P],
    [JP_KEY_5P, USB_KEY_5P],
    [JP_KEY_6P, USB_KEY_6P],
    [JP_KEY_7P, USB_KEY_7P],
    [JP_KEY_8P, USB_KEY_8P],
    [JP_KEY_9P, USB_KEY_9P],
    [JP_KEY_DECIMAL, USB_KEY_DELP],
    [JP_KEY_LEFT, USB_KEY_LEFT],
    [JP_KEY_UP, USB_KEY_UP],
    [JP_KEY_RIGHT, USB_KEY_RIGHT],
    [JP_KEY_DOWN, USB_KEY_DOWN],
    [JP_KEY_AT2, USB_KEY_BR1],
    [JP_KEY_BOLANG2, USB_KEY_PLUS],
    [JP_KEY_EQUAL2, USB_KEY_MIN],
    [JP_KEY_COLON2, USB_KEY_QUOT],
    [JP_KEY_SEMI2, USB_KEY_SEMI],
    [JP_KEY_CONVERT2, USB_KEY_KANJI4]
);

const NULL_LOCATION = 0;
const LEFT_LOCATION = 1;
const RIGHT_LOCATION = 2;
const NUMBER_LOCATION = 3;

const LEFT_CTRL = 0x01;
const RIGHT_CTRL = 0x10;
const LEFT_CTRL_V = 0xfe;
const RIGHT_CTRL_V = 0xef;
const LEFT_SHIFT = 0x02;
const RIGHT_SHIFT = 0x20;
const LEFT_SHIFT_V = 0xfd;
const RIGHT_SHIFT_V = 0xdf;
const LEFT_ALT = 0x04;
const RIGHT_ALT = 0x40;
const LEFT_ALT_V = 0xfb;
const RIGHT_ALT_V = 0xbf;
const LEFT_WIN = 0x08;
const RIGHT_WIN = 0x80;
const LEFT_WIN_V = 0xf7;
const RIGHT_WIN_V = 0x7f;

const keypad1 = new Array('1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.');

const keypad2 = new Array(
    'End',
    'ArrowDown',
    'PageDown',
    'ArrowLeft',
    'Clear',
    'ArrowRight',
    'Home',
    'ArrowUp',
    'PageUp',
    'Insert',
    'Delete'
);

export class Keyboard {
    public _layout = 0;
    public _client;
    public _keyCache = 0;
    public _numlockTimer = null;
    public _capslockTimer = null;
    public _scrolllockTimer = null;
    public _LocalMACOS = 0;
    public canvas = document.getElementById('image_panel');
    constructor(owner) {
        this._client = owner;
        this.canvas.onkeydown = (e) => {
            this._KeyDown(e);
            return false;
        };
        this.canvas.onkeyup = (e) => {
            this._KeyUp(e);
            return false;
        };
        this.canvas.onfocus = (e) => {
            this._SendNullKey();
        };
        this.canvas.onblur = (e) => {
            this._SendNullKey();
        };
    }

    _ConvertToUsb(keyCode, keyType, keyLocation, key, scanCodeString?): number {
        const keyboardlayout = Number(this._client._toolbar._keyboardlayout);
        keyCode = Number(keyCode);
        if (keyCode === JS_KEY_SHIFT) {
            if (keyType === 'keydown') {
                this._keyCache |= keyLocation === LEFT_LOCATION ? LEFT_SHIFT : RIGHT_SHIFT;
            } else {
                if (keyLocation === NULL_LOCATION) {
                    this._keyCache &= LEFT_SHIFT_V;
                    this._keyCache &= RIGHT_SHIFT_V;
                } else {
                    this._keyCache &= keyLocation === LEFT_LOCATION ? LEFT_SHIFT_V : RIGHT_SHIFT_V;
                }
            }
        } else if (keyCode === JS_KEY_CTRL) {
            if (keyType === 'keydown') {
                this._keyCache |= keyLocation === LEFT_LOCATION ? LEFT_CTRL : RIGHT_CTRL;
            } else {
                if (keyLocation === NULL_LOCATION) {
                    this._keyCache &= LEFT_CTRL_V;
                    this._keyCache &= RIGHT_CTRL_V;
                } else {
                    this._keyCache &= keyLocation === LEFT_LOCATION ? LEFT_CTRL_V : RIGHT_CTRL_V;
                }
            }
        } else if (keyCode === JS_KEY_ALT) {
            if (keyType === 'keydown') {
                this._keyCache |= keyLocation === RIGHT_LOCATION ? RIGHT_ALT : LEFT_ALT;
            } else {
                if (keyLocation === NULL_LOCATION) {
                    this._keyCache &= LEFT_ALT_V;
                    this._keyCache &= RIGHT_ALT_V;
                } else {
                    this._keyCache &= keyLocation === RIGHT_LOCATION ? RIGHT_ALT_V : LEFT_ALT_V;
                }
            }
        } else if (keyCode === JS_KEY_LWIN || keyCode === JS_KEY_RWIN) {
            if (keyType === 'keydown') {
                this._keyCache |= keyLocation === LEFT_LOCATION ? LEFT_WIN : RIGHT_WIN;
            } else {
                if (keyLocation === NULL_LOCATION) {
                    this._keyCache &= LEFT_WIN_V;
                    this._keyCache &= RIGHT_WIN_V;
                } else {
                    this._keyCache &= keyLocation === LEFT_LOCATION ? LEFT_WIN_V : RIGHT_WIN_V;
                }
            }
        } else if (keyboardlayout === 4 && keyCode === 0xe1 && key === 'AltGraph') {
            // 德语键盘linux系统下ALTGraph的keycode为225，按下时置标志位为0x40，松开时置0
            if (keyType === 'keydown') {
                this._keyCache = 0x40;
            } else {
                this._keyCache = 0;
            }
        } else {
            if (keyType === 'keyup') {
                return null;
            }

            if (Number(keyboardlayout) === 1) {
                // 经过eod软件后keycode值等于220的有两个键，以key区分
                if (keyCode === 0xdc) {
                    if (key === '_') {
                        return USB_KEY_KANJI1;
                    } else {
                        return USB_KEY_KANJI3;
                    }
                }

                // 经过eod软件后keycode值等于0的有三个键，以key区分
                if (keyCode === 0x00) {
                    if (key === 'ZenkakuHankaku') {
                        return USB_KEY_QUOTO;
                    } else if (key === 'Alphanumeric') {
                        return USB_KEY_CAPS;
                    } else if (key === 'Hiragana') {
                        return USB_KEY_KANJI2;
                    }
                }

                if (keyCode === 0x30 && key === '~') {
                    return 0;
                }

                if (keyCode === 0x35 && keyLocation === NUMBER_LOCATION) {
                    return USB_KEY_5P;
                }

                for (const jpKeyboard of jpKeyboardTable) {
                    if (jpKeyboard[0] === keyCode) {
                        return jpKeyboard[1];
                    }
                }
            } else if (keyboardlayout === 3) {
                /*
                 * FireFox浏览器下意大利键盘的特殊字符keycode部分为0，部分keycode与chrome/IE不同,MAC系统下keycode与windows部分不同
                 *  所以意大利键盘的FireFox下的特殊字符和MacOS下部分字符单独拿出来以字符区分
                 *  IE浏览器下可以获得keyCode,但是无法获取部分特殊件的字符key
                 *   字符key      keyCode_FireFox(45)     keyCode_chrome/IE
                 *    ' ?               222                    219
                 *    ì ^               160                    221
                 *    èé[               0                      186
                 *    +*]               171                    187
                 *    òç@               0                      192
                 *    à°#               0                      222
                 *    ù §               0                      191
                 *    < >               60                     226
                 */
                if (key === 'ì' || key === '^') {
                    return 0x2e;
                } else if (key === '\'' || key === '?') {
                    return 0x2d;
                } else if (key === 'è' || key === '[' || key === 'é') {
                    return 0x2f;
                } else if (key === '+' || key === ']' || key === '*') {
                    return 0x30;
                } else if (key === 'ò' || key === '@' || key === 'ç') {
                    return 0x33;
                } else if (key === 'à' || key === '#' || key === '°') {
                    return 0x34;
                } else if (key === 'ù' || key === '§') {
                    return 0x31;
                } else if (key === '<' || key === '>') {
                    return 0x64;
                } else if (key === ';') {
                    // MAC系统下，;:两个字符对应的keycode和在windows下不同
                    return 0x36;
                } else if (key === ':') {
                    return 0x37;
                }
                for (const itKeyboard of itKeyboardTable) {
                    if (itKeyboard[0] === keyCode) {
                        return itKeyboard[1];
                    }
                }
            } else if (Number(keyboardlayout) === 4) {
                /**
                 * firefox浏览器下´`按键的keycode与chrome/IE浏览器下öÖ按键的keycode相同，此处通过字符加以区分
                 * 且在firefox浏览器下üöä按键的keycode为0，通过字符转换usbcode
                 * MAC系统下部分按键的keycode与windows系统下不同，且存在keycode冲突，需要通过字符协助区分
                 */
                const _pf = navigator.platform;
                if (_pf === 'Mac68K' || _pf === 'MacPPC' || _pf === 'Macintosh' || _pf === 'MacIntel') {
                    this._LocalMACOS = 1;
                    const result = this._ConvertToUsbForMac(keyCode, key);
                    if (result !== 0) {
                        return result;
                    }
                }
                if (keyCode === 0xc0 && key !== 'ö' && key !== 'Ö' && scanCodeString !== 'Backquote') {
                    return 0x2e;
                }
                if (key === 'ü' || key === 'Ü') {
                    return 0x2f;
                } else if (key === 'ö' || key === 'Ö') {
                    return 0x33;
                } else if (key === 'ä' || key === 'Ä') {
                    return 0x34;
                }
                if (scanCodeString === 'Backquote') {
                    return 0x35;
                }
                if (scanCodeString === 'Equal') {
                    return 0x2e;
                } else {
                    for (const deKeyboard of deKeyboardTable) {
                        if (deKeyboard[0] === keyCode) {
                            return deKeyboard[1];
                        }
                    }
                }
            } else {
                if (typeof scanCodeString !== 'undefined') {
                    for (const uskeyboard of uskeyboardTable) {
                        if (uskeyboard[2] === scanCodeString) {
                            return Number(uskeyboard[1]);
                        }
                    }
                }

                for (const uskeyboard of uskeyboardTable) {
                    if (uskeyboard[0] === keyCode) {
                        return Number(uskeyboard[1]);
                    }
                }
            }
        }

        return 0;
    }

    // 处理MAC系统下keycode冲突的键值
    _ConvertToUsbForMac(keyCode, key) {
        keyCode = Number(keyCode);
        if (keyCode === 0xba && key === ':') {
            return 0x37;
        } else if (keyCode === 0xba && key === ';') {
            return 0x36;
        } else if (keyCode === 0xdd) {
            return 0x30;
        } else if (keyCode === 0xc0) {
            return 0x64;
        } else if ((keyCode === 0xbf || keyCode === 0xbd) && key !== '-' && key !== '_') {
            // -_和ß\的keycode相同
            return 0x2d;
        } else if (keyCode === 0xdc || (keyCode === 0xde && key !== 'ä' && key !== 'Ä')) {
            // '和äÄ的keycode相同
            return 0x31;
        } else if (keyCode === 0xbb && key === 'Dead') {
            // +~和´的keycode相同
            return 0x2e;
        } else if ((keyCode === 0xbe || keyCode === 0xbc) && key !== ',' && key !== '.') {
            // ,.和^°的keycode相同
            return 0x35;
        }
        return 0;
    }

    _ConvertAndSend(
        keyCode,
        keyType,
        keyLocation,
        scanCodeString?,
        ctrlDown?,
        altDown?,
        shiftDown?,
        key?
    ) {
        const keyData = new Uint8Array(8);
        const usbKey = this._ConvertToUsb(keyCode, keyType, keyLocation, key, scanCodeString);
        if (usbKey == null) {
            return;
        }

        const keyboardlayout = Number(this._client._toolbar._keyboardlayout);
        if ((keyboardlayout === 3 || keyboardlayout === 4) && this._keyCache & RIGHT_ALT) {
            this._keyCache &= RIGHT_ALT;
        } else {
            if (this._keyCache & RIGHT_ALT) {
                this._keyCache &= LEFT_CTRL_V;
                this._keyCache &= RIGHT_CTRL_V;
            }

            if (typeof ctrlDown !== 'undefined' && ctrlDown === false) {
                this._keyCache &= LEFT_CTRL_V;
                this._keyCache &= RIGHT_CTRL_V;
            }

            if (typeof altDown !== 'undefined' && altDown === false) {
                this._keyCache &= LEFT_ALT_V;
                this._keyCache &= RIGHT_ALT_V;
            }

            if (typeof shiftDown !== 'undefined' && shiftDown === false) {
                this._keyCache &= LEFT_SHIFT_V;
                this._keyCache &= RIGHT_SHIFT_V;
            }
        }
        // MAC系统，德语键盘模式下，数字小键盘的“,”符号需要使用shift+","按键才会输出，单按“,”为delete
        if (keyboardlayout === 4 && this._keyCache === 0 && this._LocalMACOS === 1 && keyCode === 0x6e) {
            this._keyCache = 0x20;
        }

        keyData[0] = this._keyCache;
        keyData[1] = 0;
        keyData[2] = usbKey;
        keyData[3] = 0;
        keyData[4] = 0;
        keyData[5] = 0;
        keyData[6] = 0;
        keyData[7] = 0;

        const data = packet.keyPressedPacket(0, this._client._security, keyData);
        this._client._comunication._Send(data);
    }

    _KeyDown(e) {
        const self = this;
        const lcCapState = e.getModifierState && e.getModifierState('CapsLock');
        const osCapState = self._client._toolbar._keyboardState & 0x02 ? true : false;
        const lcNumState = e.getModifierState && this.getNumLockState(e);
        const osNumState = self._client._toolbar._keyboardState & 0x01 ? true : false;
        const lcScrollState = e.getModifierState && e.getModifierState('ScrollLock');
        const osScrollState = self._client._toolbar._keyboardState & 0x04 ? true : false;
        const keyCode = Number(e.keyCode || e.which || e.charCode);
        if (
            (keyCode === JS_KEY_NUM && this._numlockTimer != null) ||
            (keyCode === JS_KEY_CAPS && this._capslockTimer != null) ||
            (keyCode === JS_KEY_SCROLL && this._scrolllockTimer != null)
        ) {
            return;
        }

        if (keyCode === JS_KEY_NUM) {
            this._numlockTimer = setTimeout(() => {
                self._numlockTimer = null;
            }, 250);
        }
        if (keyCode === JS_KEY_CAPS) {
            this._capslockTimer = setTimeout(() => {
                self._capslockTimer = null;
            }, 250);
        }
        if (keyCode === JS_KEY_SCROLL) {
            this._scrolllockTimer = setTimeout(() => {
                self._scrolllockTimer = null;
            }, 250);
        }

        if (typeof lcCapState !== 'undefined') {
            if (this._client.scope.kvmParams.capsSync !== '0' && keyCode !== JS_KEY_CAPS && lcCapState !== osCapState) {
                this.sendSelfDef(JS_KEY_CAPS, '', '', '', '', '', '', '', '', '', '', '');
            } else if (this._client.scope.kvmParams.capsSync !== '0' && keyCode === JS_KEY_CAPS && lcCapState === osCapState) {
                return false;
            }
        }

        if (typeof lcNumState !== 'undefined') {
            if (keyCode !== JS_KEY_NUM && lcNumState !== osNumState) {
                this.sendSelfDef(JS_KEY_NUM, '', '', '', '', '', '', '', '', '', '', '');
            } else if (keyCode === JS_KEY_NUM && lcNumState === osNumState) {
                return false;
            }
        }

        if (typeof lcScrollState !== 'undefined') {
            if (keyCode !== JS_KEY_SCROLL && lcScrollState !== osScrollState) {
                this.sendSelfDef(JS_KEY_SCROLL, '', '', '', '', '', '', '', '', '', '', '');
            } else if (keyCode === JS_KEY_SCROLL && lcScrollState === osScrollState) {
                return false;
            }
        }

        const keyboardlayout = Number(this._client._toolbar._keyboardlayout);

        if (keyboardlayout === 1 || keyboardlayout === 3 || keyboardlayout === 4) {
            if (keyCode === JS_KEY_SHIFT) {
                this._ConvertAndSend(
                    keyCode,
                    'keydown',
                    e.location || e.keyLocation,
                    e.code,
                    e.ctrlKey,
                    e.altKey,
                    true,
                    e.key
                );
            } else if (keyCode === JS_KEY_ALT) {
                this._ConvertAndSend(
                    keyCode,
                    'keydown',
                    e.location || e.keyLocation,
                    e.code,
                    e.ctrlKey,
                    true,
                    e.shiftKey,
                    e.key
                );
            } else if (keyCode === JS_KEY_CTRL) {
                this._ConvertAndSend(
                    keyCode,
                    'keydown',
                    e.location || e.keyLocation,
                    e.code,
                    true,
                    e.altKey,
                    e.shiftKey,
                    e.key
                );
            } else {
                this._ConvertAndSend(
                    keyCode,
                    'keydown',
                    e.location || e.keyLocation,
                    e.code,
                    e.ctrlKey,
                    e.altKey,
                    e.shiftKey,
                    e.key
                );
            }
        } else {
            this._ConvertAndSend(
                keyCode,
                'keydown',
                e.location || e.keyLocation,
                e.code,
                e.ctrlKey,
                e.altKey,
                e.shiftKey,
                e.key
            );
        }
        return false;
    }

    _KeyUp(e) {
        const keyCode = Number(e.keyCode || e.which || e.charCode);
        if (keyCode === JS_KEY_NUM && this._numlockTimer != null) {
            clearTimeout(this._numlockTimer);
            this._numlockTimer = null;
        }
        if (keyCode === JS_KEY_CAPS && this._capslockTimer != null) {
            clearTimeout(this._capslockTimer);
            this._capslockTimer = null;
        }
        if (keyCode === JS_KEY_SCROLL && this._scrolllockTimer != null) {
            clearTimeout(this._scrolllockTimer);
            this._scrolllockTimer = null;
        }
        this._ConvertAndSend(
            keyCode,
            'keyup',
            e.location || e.keyLocation,
            e.code,
            e.ctrlKey,
            e.altKey,
            e.shiftKey,
            e.key
        );
        return false;
    }

    _SendNullKey() {
        this._keyCache = 0;
        this._ConvertAndSend(JS_KEY_ALT, 'keyup', LEFT_LOCATION);
    }

    sendAltTab() {
        this._keyCache |= LEFT_ALT;
        this._ConvertAndSend(JS_KEY_TAB, 'keydown', NULL_LOCATION);
        this._ConvertAndSend(JS_KEY_ALT, 'keyup', LEFT_LOCATION);
    }

    sendCtrlEsc() {
        this._keyCache |= LEFT_CTRL;
        this._ConvertAndSend(JS_KEY_ESC, 'keydown', NULL_LOCATION);
        this._ConvertAndSend(JS_KEY_CTRL, 'keyup', LEFT_LOCATION);
    }

    sendCtrlShift() {
        this._keyCache |= LEFT_CTRL | LEFT_SHIFT;
        this._ConvertAndSend(JS_KEY_SHIFT, 'keydown', LEFT_LOCATION);
        this._ConvertAndSend(JS_KEY_CTRL, 'keyup', LEFT_LOCATION);
    }

    sendCtrlSpace() {
        this._keyCache |= LEFT_CTRL;
        this._ConvertAndSend(JS_KEY_SPACE, 'keydown', LEFT_LOCATION);
        this._ConvertAndSend(JS_KEY_CTRL, 'keyup', LEFT_LOCATION);
    }

    sendCtrlAltDel() {
        // 组合键是独立的，防止Shift键缓存对该组合键影响，_keyCache重置
        this._keyCache = LEFT_CTRL | LEFT_ALT;
        this._ConvertAndSend(JS_KEY_DEL, 'keydown', LEFT_LOCATION);
        this._ConvertAndSend(JS_KEY_ALT, 'keyup', LEFT_LOCATION);
        this._ConvertAndSend(JS_KEY_CTRL, 'keyup', LEFT_LOCATION);
    }

    sendSelfDef(
        keyA,
        keyCharA,
        keyB,
        keyCharB,
        keyC,
        keyCharC,
        keyD,
        keyCharD,
        keyE,
        keyCharE,
        keyF,
        keyCharF
    ) {
        const keyData = new Uint8Array(8);
        const keyboardlayout = Number(this._client._toolbar._keyboardlayout);
        keyData[1] = 0;
        /**
         * 当为意大利键盘时，右边AltGraph键和左边Alt键功能不同，不能全部用左边Alt来替代;
         * 且firefox浏览器下，获取不到keycode,所以需要将按键字符keychar作为参数传递
         */
        if ((keyboardlayout === 3 || keyboardlayout === 4) && keyCharA === 'AltGraph') {
            keyData[2] = this._ConvertToUsb(keyA, 'keydown', RIGHT_LOCATION, keyCharA);
        } else {
            keyData[2] = this._ConvertToUsb(keyA, 'keydown', LEFT_LOCATION, keyCharA);
        }
        if ((keyboardlayout === 3 || keyboardlayout === 4) && keyCharB === 'AltGraph') {
            keyData[3] = this._ConvertToUsb(keyB, 'keydown', RIGHT_LOCATION, keyCharB);
        } else {
            keyData[3] = this._ConvertToUsb(keyB, 'keydown', LEFT_LOCATION, keyCharB);
        }
        if ((keyboardlayout === 3 || keyboardlayout === 4) && keyCharC === 'AltGraph') {
            keyData[4] = this._ConvertToUsb(keyC, 'keydown', RIGHT_LOCATION, keyCharC);
        } else {
            keyData[4] = this._ConvertToUsb(keyC, 'keydown', LEFT_LOCATION, keyCharC);
        }
        if ((keyboardlayout === 3 || keyboardlayout === 4) && keyCharD === 'AltGraph') {
            keyData[5] = this._ConvertToUsb(keyD, 'keydown', RIGHT_LOCATION, keyCharD);
        } else {
            keyData[5] = this._ConvertToUsb(keyD, 'keydown', LEFT_LOCATION, keyCharD);
        }
        if ((keyboardlayout === 3 || keyboardlayout === 4) && keyCharE === 'AltGraph') {
            keyData[6] = this._ConvertToUsb(keyE, 'keydown', RIGHT_LOCATION, keyCharE);
        } else {
            keyData[6] = this._ConvertToUsb(keyE, 'keydown', LEFT_LOCATION, keyCharE);
        }
        if ((keyboardlayout === 3 || keyboardlayout === 4) && keyCharF === 'AltGraph') {
            keyData[7] = this._ConvertToUsb(keyF, 'keydown', RIGHT_LOCATION, keyCharF);
        } else {
            keyData[7] = this._ConvertToUsb(keyF, 'keydown', LEFT_LOCATION, keyCharF);
        }
        keyData[0] = this._keyCache;
        const data = packet.keyPressedPacket(0, this._client._security, keyData);
        this._client._comunication._Send(data);

        this._keyCache = 0;
        this._ConvertAndSend(JS_KEY_CTRL, 'keyup', LEFT_LOCATION);
    }

    getNumLockState(e) {
        if (Number(e.location || e.keyLocation) === NUMBER_LOCATION) {
            if (typeof e.key === 'undefined') {
                const keycode = Number(e.keyCode || e.which || e.charCode);
                if ((keycode >= JS_KEY_0P && keycode <= JS_KEY_9P) || keycode === JS_KEY_DELP) {
                    if (e.getModifierState('NumLock') === false) {
                        return true;
                    }
                }
                if (
                    (keycode >= JS_KEY_PAGEUP && keycode <= JS_KEY_DOWN) ||
                    keycode === JS_KEY_CLEAR ||
                    keycode === JS_KEY_INSERT ||
                    keycode === JS_KEY_DEL
                ) {
                    if (e.getModifierState('NumLock') === true) {
                        return false;
                    }
                }
            }

            if (keypad1.indexOf(e.key) !== -1 && e.getModifierState('NumLock') === false) {
                return true;
            }
            if (keypad2.indexOf(e.key) !== -1 && e.getModifierState('NumLock') === true) {
                return false;
            }
        }
        return e.getModifierState('NumLock');
    }
}
