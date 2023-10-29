#ifndef LETTERS_H
#define LETTERS_H

#define LETTER_HEIGHT 5
#define LETTER_WIDTH 5
#define LETTER_BIT_COUNT = (LETTER_HEIGHT * LETTER_WIDTH)
#define SPACEBAR 1

/*
Letters are constructed such the the LSB is the top left pixel and the MSB is the bottom right
every 5 bits (LETTER_WIDTH) the pixels will wrap to the next row
there are 5 rows in total (LETTER_HEIGHT)

<-LSB       Letter 'A' shown
0 1 0 0 0   first 5 bits
1 0 1 0 0
1 0 1 0 0
1 1 1 0 0
1 0 1 0 0   last 5 bits
MSB->
*/

const int asciiFont[128] = {
    0,        // NUL (Null)
    0,        // SOH (Start of Header)
    0,        // STX (Start of Text)
    0,        // ETX (End of Text)
    0,        // EOT (End of Transmission)
    0,        // ENQ (Enquiry)
    0,        // ACK (Acknowledgment)
    0,        // BEL (Bell)
    0,        // BS (Backspace)
    0,        // HT (Horizontal Tab)
    0,        // LF (Line Feed)
    0,        // VT (Vertical Tab)
    0,        // FF (Form Feed)
    0,        // CR (Carriage Return)
    0,        // SO (Shift Out)
    0,        // SI (Shift In)
    0,        // DLE (Data Link Escape)
    0,        // DC1 (Device Control 1)
    0,        // DC2 (Device Control 2)
    0,        // DC3 (Device Control 3)
    0,        // DC4 (Device Control 4)
    0,        // NAK (Negative Acknowledgment)
    0,        // SYN (Synchronous Idle)
    0,        // ETB (End of Transmission Block)
    0,        // CAN (Cancel)
    0,        // EM (End of Medium)
    0,        // SUB (Substitute)
    0,        // ESC (Escape)
    0,        // FS (File Separator)
    0,        // GS (Group Separator)
    0,        // RS (Record Separator)
    0,        // US (Unit Separator)
    0,        // Space
    1049633,  // !
    165,      // "
    11512810, // #
    62,       // $
    9507104,  // %
    0,        // &
    33,       // '
    2130978,  // (
    1116225,  // )
    5189,     // *
    72768,    // +
    1114112,  // ,
    3072,     // -
    1048576,  // .
    1083456,  // /
    7509159,  // 0
    7407714,  // 1
    7380103,  // 2
    7477383,  // 3
    4332705,  // 4
    3279911,  // 5
    7511079,  // 6
    2164871,  // 7
    7511207,  // 8
    4332711,  // 9
    32800,    // :
    1081376,  // ;
    4260932,  // <
    98400,    // =
    1118273,  // >
    2099335,  // ?
    33218079, // @
    5477538,  // A
    3312803,  // B
    6325286,  // C
    3314851,  // D
    7375911,  // E
    1084455,  // F
    6456358,  // G
    5414053,  // H
    7407687,  // I
    2265223,  // J
    5409957,  // K
    7373857,  // L
    18405233, // M
    9745769,  // N
    7509159,  // O
    1088679,  // P
    3314855,  // Q
    5409959,  // R
    7478311,  // S
    2164807,  // T
    7509157,  // U
    2266277,  // V
    11191857, // W
    5408933,  // X
    2164901,  // Y
    7374983,  // Z
    3179555,  // [
    2163744,  // backslash
    3213379,  // ]
    162,      // ^
    7340032,  // _
    65,       // `
    5477538,  // a
    3312803,  // b
    6325286,  // c
    3314851,  // d
    7375911,  // e
    1084455,  // f
    6456358,  // g
    5414053,  // h
    7407687,  // i
    2265223,  // j
    5409957,  // k
    7373857,  // l
    18405233, // m
    9745769,  // n
    7509159,  // o
    1088679,  // p
    3314855,  // q
    5409959,  // r
    7478311,  // s
    2164807,  // t
    7509157,  // u
    2266277,  // v
    11191857, // w
    5408933,  // x
    2164901,  // y
    7374983,  // z
    6360134,  // {
    1082401,  // |
    3217475,  // }
    170,      // ~
    0};       // Delete

#endif