#include "../../libraries/bikka/video.h"
#include "../../libraries/common.h"
#include "../../libraries/bikka/low_level.h"
#include "../../libraries/bikka/memory.h"

size_t _TerminalRow;
size_t _TerminalColumn;
uint8_t _TerminalColor;


uint16_t *_TerminalBuffer;


void TERMINAL_INSTALL(void){
    /* Sets up and installs the terminal */
    _TerminalRow = 0;
    _TerminalColumn = 0;
    _TerminalColor = VGAEntryColor(COLOR_WHITE, COLOR_BLUE); /* Communist red, hehe, well it's blue now */
    _TerminalBuffer = (uint16_t*) VIDEO_ADDRESS; /* Point the buffer to our video address */
    for (size_t Y_Axis = 0; Y_Axis < _VGAHeight; Y_Axis++){  /* Y_axis */
        for (size_t X_Axis = 0; X_Axis < _VGAWidth; X_Axis++){ /* X_axis */
            const size_t Index = ((Y_Axis * _VGAWidth) + X_Axis);
            _TerminalBuffer[Index] = VGAEntry(' ', _TerminalColor); /* Clean the screen and set the color */
        }
    }
}

void TERMINAL_SET_COLOR(uint8_t Color){
    /* Just a function to set the global variable '_TerminalColor' */
    _TerminalColor = Color; /* Nice */
}

void TERMINAL_PUT_ENTRY_AT(char C, uint8_t Color, size_t X_Axis, size_t Y_Axis){
    /* Kinda the same as the above, this only sets the entry point for printing */
    const size_t Index = ((Y_Axis * _VGAWidth) + X_Axis);
    _TerminalBuffer[Index] = VGAEntry(C, Color);
}

void TERMINAL_PUT_CHAR(char Character){
    /* One of the 'core functions' used to print */

    if (Character == '\n'){ /* Enable support for newline */
        _TerminalColumn = 0; /* Set the column to 0, so that we start at the beginning of the new row */
        _TerminalRow++; /* Add 1 to the row so that we start fresh */
        Character = ' '; /* Make the character go away */
    }
    
    if (Character == '\t'){ /* Enable support for tab */
        _TerminalColumn = _TerminalColumn + 4; /* 4 spaces */
        Character = ' '; /* Make the character go away */
    }

    if (Character == '\r'){ /* Enable carriage return */
        _TerminalColumn = 0;
        Character = ' '; /* Go go and never come back */
    }

    int Offset = TERMINAL_GET_SCREEN_OFFSET(_TerminalColumn+1, _TerminalRow); /* Get where the cursor should be */
    TERMINAL_PUT_ENTRY_AT(Character, _TerminalColor, _TerminalColumn, _TerminalRow);

    if (++_TerminalColumn == _VGAWidth){
        _TerminalColumn = 0;
        if (++_TerminalRow == _VGAHeight){
            _TerminalRow = 0;
        }
    }
    Offset += 2;
    Offset = TERMINAL_SCROLLING(Offset);
    TERMINAL_SET_CURSOR(Offset); /* Set the cursor */
}

void TERMINAL_WRITE(const char *Data, size_t Size){
    /**/
    for (size_t i = 0; i < Size; i++){ TERMINAL_PUT_CHAR(Data[i]); }
}

void TERMINAL_WRITE_STRING(const char *Data){
    /**/
    TERMINAL_WRITE(Data, STR_LEN(Data));
}

/* Special print function */
void GeoPrint(const char *Data, int Col, int Row){
    /* Geo print prints out 'Data' at the location marked by 'Col' and 'Row' */
    _TerminalColumn = Col; /* Set the column to our desired value */
    _TerminalRow = Row; /* Set the row to the desired value */
    TERMINAL_WRITE_STRING(Data); /* Print the string */
    _TerminalColumn = 0; 
    _TerminalRow++;
}

int TERMINAL_GET_SCREEN_OFFSET(int Col, int Row){ return ((Row * _VGAWidth) + Col) * 2; }

void TERMINAL_SET_CURSOR(int Offset){
    Offset /= 2;
    PORT_BYTE_OUT(REG_SCREEN_CTRL, 14);
    PORT_BYTE_OUT(REG_SCREEN_DATA, (unsigned char)(Offset >> 8));
    PORT_BYTE_OUT(REG_SCREEN_CTRL, 15);
    PORT_BYTE_OUT(REG_SCREEN_DATA, Offset);
}

int TERMINAL_GET_CURSOR(void){
    /* Locates where the cursor is */
    PORT_BYTE_OUT(REG_SCREEN_CTRL, 14);
    int Offset = PORT_BYTE_IN(REG_SCREEN_DATA) << 8;
    PORT_BYTE_OUT(REG_SCREEN_CTRL, 15);
    Offset += PORT_BYTE_IN(REG_SCREEN_DATA);
    return (Offset * 2);
}

int TERMINAL_SCROLLING(int Offset){
    /* Scrolls the screen if necessary */
    int CursorOffset = Offset;
    int i;
    if (CursorOffset < (_VGAHeight*_VGAWidth*2)){ return CursorOffset; }
    for (i = 1; i < _VGAHeight; i++){ MEM_CPY_2(TERMINAL_GET_SCREEN_OFFSET(0, i) + VIDEO_ADDRESS, TERMINAL_GET_SCREEN_OFFSET(0,i-1) + VIDEO_ADDRESS, _VGAWidth * 2); }

    char *LastLine = TERMINAL_GET_SCREEN_OFFSET(0, _VGAHeight-1) + VIDEO_ADDRESS;
    for (i = 0; i < _VGAWidth * 2; i++){ LastLine[i] = 0; }
    CursorOffset -= 2 * _VGAWidth;
    return CursorOffset;
}

void TERMINAL_BACKSPACE(void){
    /* Removes two spaces from the video memory*/
    int Offset = TERMINAL_GET_CURSOR()-2;
    TERMINAL_PUT_ENTRY_AT(' ', COLOR_BLACK, _TerminalColumn, _TerminalRow);
}


/* Functions for when you're in user mode */
void USERMODE_TERMINAL_INSTALL(enum _VGA_COLOR_ Color){
    /* Sets up and installs the terminal */
    _TerminalRow = 0;
    _TerminalColumn = 0;
    _TerminalColor = VGAEntryColor(COLOR_WHITE, Color);
    _TerminalBuffer = (uint16_t*) VIDEO_ADDRESS; /* Point the buffer to our video address */
    for (size_t Y_Axis = 0; Y_Axis < _VGAHeight; Y_Axis++){  /* Y_axis */
        for (size_t X_Axis = 0; X_Axis < _VGAWidth; X_Axis++){ /* X_axis */
            const size_t Index = ((Y_Axis * _VGAWidth) + X_Axis);
            _TerminalBuffer[Index] = VGAEntry(' ', _TerminalColor); /* Clean the screen and set the color */
        }
    }
}