/**
 * @file swkbd.h
 * @brief Wrapper for using the swkbd (software keyboard) LibraryApplet.
 * @author yellows8
 * @copyright libnx Authors
 */
#pragma once
#include "../types.h"
#include "../services/applet.h"

typedef enum {
    SwkbdTextCheckResult_OK      = 0,  ///< Success, valid string.
    SwkbdTextCheckResult_Bad     = 1,  ///< Failure, invalid string. Error message is displayed in a message-box, pressing OK will return to swkbd again.
    SwkbdTextCheckResult_Prompt  = 2,  ///< Failure, invalid string. Error message is displayed in a message-box, pressing Cancel will return to swkbd again, while pressing OK will continue as if the text was valid.
    SwkbdTextCheckResult_Silent  = 3,  ///< Failure, invalid string. With value 3 and above, swkbd will silently not accept the string, without displaying any error.
} SwkbdTextCheckResult;

typedef enum {
    SwkbdType_Normal = 0,  ///< Normal keyboard.
    SwkbdType_NumPad = 1,  ///< Number pad. The buttons at the bottom left/right are only available when they're set by \ref swkbdConfigSetLeftOptionalSymbolKey / \ref swkbdConfigSetRightOptionalSymbolKey.
    SwkbdType_QWERTY = 2,  ///< QWERTY (and variants) keyboard only.
} SwkbdType;

/// Bitmask for \ref SwkbdArgV0 keySetDisableBitmask. This disables keys on the keyboard when the corresponding bit(s) are set.
enum {
    SwkbdKeyDisableBitmask_Space        = BIT(1),  ///< Disable space-bar.
    SwkbdKeyDisableBitmask_At           = BIT(2),  ///< Disable '@'.
    SwkbdKeyDisableBitmask_Percent      = BIT(3),  ///< Disable '%'.
    SwkbdKeyDisableBitmask_ForwardSlash = BIT(4),  ///< Disable '/'.
    SwkbdKeyDisableBitmask_Backslash    = BIT(5),  ///< Disable '\'.
    SwkbdKeyDisableBitmask_Numbers      = BIT(6),  ///< Disable numbers.
    SwkbdKeyDisableBitmask_DownloadCode = BIT(7),  ///< Used for \ref swkbdConfigMakePresetDownloadCode.
    SwkbdKeyDisableBitmask_UserName     = BIT(8),  ///< Used for \ref swkbdConfigMakePresetUserName. Disables '@', '%', and '\'.
};

/// Value for \ref SwkbdArgV0 textDrawType. Only applies when stringLenMax is 1..32, otherwise swkbd will only use SwkbdTextDrawType_Box.
typedef enum {
    SwkbdTextDrawType_Line          = 0,  ///< The text will be displayed on a line. Also enables displaying the Header and Sub text.
    SwkbdTextDrawType_Box           = 1,  ///< The text will be displayed in a box.
    SwkbdTextDrawType_DownloadCode  = 2,  ///< Used by \ref swkbdConfigMakePresetDownloadCode on 5.0.0+. Enables using \ref SwkbdArgV7 unk_x3e0.
} SwkbdTextDrawType;

/// SwkbdInline Interactive input storage request ID.
typedef enum {
    SwkbdRequestCommand_Finalize = 0x4,
    SwkbdRequestCommand_SetUserWordInfo = 0x6,
    SwkbdRequestCommand_SetCustomizeDic = 0x7,
    SwkbdRequestCommand_Calc = 0xA,
} SwkbdRequestCommand;

/// SwkbdInline Interactive output storage reply ID.
typedef enum {
    SwkbdReplyType_FinishedInitialize    = 0x0,
    SwkbdReplyType_ChangedString         = 0x2,
    SwkbdReplyType_MovedCursor           = 0x3,
    SwkbdReplyType_MovedTab              = 0x4,
    SwkbdReplyType_DecidedEnter          = 0x5,
    SwkbdReplyType_DecidedCancel         = 0x6,
    SwkbdReplyType_ChangedStringUtf8     = 0x7,
    SwkbdReplyType_MovedCursorUtf8       = 0x8,
    SwkbdReplyType_DecidedEnterUtf8      = 0x9,
    SwkbdReplyType_UnsetCustomizeDic     = 0xA,
    SwkbdReplyType_ReleasedUserWordInfo  = 0xB,
} SwkbdReplyType;

/// SwkbdInline State
typedef enum {
    SwkbdState_Inactive       = 0x0,  ///< Default state from \ref swkbdInlineCreate, before a state is set by \ref swkbdInlineUpdate when a reply is received. Also indicates that the applet is no longer running.
    SwkbdState_Initialized    = 0x1,
    SwkbdState_Unknown2       = 0x2,
    SwkbdState_TextAvailable  = 0x3,  ///< Text is available since a ChangedString* reply was received.
    SwkbdState_Submitted      = 0x4,  ///< The user pressed the ok-button, submitting the text and closing the applet.
    SwkbdState_Unknown5       = 0x5,
} SwkbdState;

/// Value for \ref SwkbdInitializeArg mode. Controls the LibAppletMode when launching the applet.
typedef enum {
    SwkbdInlineMode_UserDisplay   = 0,  ///< LibAppletMode_Unknown3. This is the default. The user-process must handle displaying the swkbd gfx on the screen. Attempting to get the swkbd gfx data for this currently throws an error (unknown why), SwkbdInlineMode_AppletDisplay should be used instead.
    SwkbdInlineMode_AppletDisplay = 1,  ///< LibAppletMode_Background. The applet will handle displaying gfx on the screen.
} SwkbdInlineMode;

/// TextCheck callback set by \ref swkbdConfigSetTextCheckCallback, for validating the input string when the swkbd ok-button is pressed. This buffer contains an UTF-8 string. This callback should validate the input string, then return a \ref SwkbdTextCheckResult indicating success/failure. On failure, this function must write an error message to the tmp_string buffer, which will then be displayed by swkbd.
typedef SwkbdTextCheckResult (*SwkbdTextCheckCb)(char* tmp_string, size_t tmp_string_size);

/// Base swkbd arg struct.
typedef struct {
    SwkbdType type;                  ///< See \ref SwkbdType.
    u16 okButtonText[18/2];
    u16 leftButtonText;
    u16 rightButtonText;
    u8  dicFlag;                     ///< Enables dictionary usage when non-zero (including the system dictionary).
    u8  pad_x1b;
    u32 keySetDisableBitmask;        ///< See SwkbdKeyDisableBitmask_*.
    u32 initialCursorPos;            ///< Initial cursor position in the string: 0 = start, 1 = end.
    u16 headerText[130/2];
    u16 subText[258/2];
    u16 guideText[514/2];
    u16 pad_x3aa;
    u32 stringLenMax;                ///< When non-zero, specifies the max string length. When the input is too long, swkbd will stop accepting more input until text is deleted via the B button (Backspace). See also \ref SwkbdTextDrawType.
    u32 stringLenMaxExt;             ///< When non-zero, specifies the max string length. When the input is too long, swkbd will display an icon and disable the ok-button.
    u32 passwordFlag;                ///< Use password: 0 = disable, 1 = enable.
    SwkbdTextDrawType textDrawType;  ///< See \ref SwkbdTextDrawType.
    u16 returnButtonFlag;            ///< Controls whether the Return button is enabled, for newlines input. 0 = disabled, non-zero = enabled.
    u8  blurBackground;              ///< When enabled with value 1, the background is blurred.
    u8  pad_x3bf;
    u32 initialStringOffset;
    u32 initialStringSize;
    u32 userDicOffset;
    s32 userDicEntries;
    u8 textCheckFlag;
    u8 pad_x3d1[7];
    SwkbdTextCheckCb textCheckCb;  ///< This really doesn't belong in a struct sent to another process, but official sw does this.
} SwkbdArgV0;

/// Arg struct for version 0x30007+.
typedef struct {
    SwkbdArgV0 arg;
    u32 unk_x3e0[8];  ///< When set and enabled via \ref SwkbdTextDrawType, controls displayed text grouping (inserts spaces, without affecting output string).
} SwkbdArgV7;

typedef struct {
    SwkbdArgV7 arg;

    u8* workbuf;
    size_t workbuf_size;
    s32 max_dictwords;

    u32 version;
} SwkbdConfig;

/// User dictionary word.
typedef struct {
    u8 unk_x0[0x64];
} SwkbdDictWord;

/// Input data for SwkbdInline request SetCustomizeDic.
typedef struct {
    u8 unk_x0[0x70];
} SwkbdCustomizeDicInfo;

typedef struct {
    u32 unk_x0;
    u8 mode;            ///< See \ref SwkbdInlineMode.
    u8 unk_x5;          ///< Only set on 5.0.0+.
    u8 pad[2];
} SwkbdInitializeArg;

typedef struct {
    SwkbdType type;                  ///< See \ref SwkbdType.
    u16 okButtonText[9];
    u16 leftButtonText;
    u16 rightButtonText;
    u8 dicFlag;                      ///< Enables dictionary usage when non-zero (including the system dictionary).
    u8 unk_x1b;
    u32 keySetDisableBitmask;        ///< See SwkbdKeyDisableBitmask_*.
    s32 unk_x20;
    s32 unk_x24;
    u8 returnButtonFlag;             ///< Controls whether the Return button is enabled, for newlines input. 0 = disabled, non-zero = enabled.
    u16 unk_x29;
    u8 unk_x2b;
    u32 flags;                       ///< Bitmask 0x4: unknown.
    u8 unk_x30;
    u8 unk_x31[0x17];
} PACKED SwkbdAppearArg;

typedef struct {
    u32 unk_x0;
    u16 size;                    ///< Size of this struct.
    u8 unk_x6;
    u8 unk_x7;
    u64 flags;
    SwkbdInitializeArg initArg;  ///< Flags bitmask 0x1.
    float volume;                ///< Flags bitmask 0x2.
    s32 cursorPos;               ///< Flags bitmask 0x10.
    SwkbdAppearArg appearArg;
    u16 inputText[0x3f4/2];      ///< Flags bitmask 0x8.
    u8 utf8Mode;                 ///< Flags bitmask 0x20.
    u8 unk_x45d;
    u8 enableBackspace;          ///< Flags bitmask 0x8000. Only available with 5.0.0+.
    u8 unk_x45f[3];
    u8 keytopAsFloating;         ///< Flags bitmask 0x200.
    u8 footerScalable;           ///< Flags bitmask 0x100.
    u8 alphaEnabledInInputMode;  ///< Flags bitmask 0x100.
    u8 inputModeFadeType;        ///< Flags bitmask 0x100.
    u8 disableTouch;             ///< Flags bitmask 0x200.
    u8 disableUSBKeyboard;       ///< Flags bitmask 0x800.
    u8 unk_x468[5];
    u16 unk_x46d;
    u8 unk_x46f;
    float keytopScaleX;          ///< Flags bitmask 0x200.
    float keytopScaleY;          ///< Flags bitmask 0x200.
    float keytopTranslateX;      ///< Flags bitmask 0x200.
    float keytopTranslateY;      ///< Flags bitmask 0x200.
    float keytopBgAlpha;         ///< Flags bitmask 0x100.
    float footerBgAlpha;         ///< Flags bitmask 0x100.
    float balloonScale;          ///< Flags bitmask 0x200.
    float unk_x48c;
    u8 unk_x490[0xc];
    u8 seGroup;                  ////< Flags bitmask: enable=0x2000, disable=0x4000. Only available with 5.0.0+.
    u8 pad_x49d[3];
} PACKED SwkbdInlineCalcArg;

/// Struct data for SwkbdInline Interactive reply storage ChangedString*, at the end following the string.
typedef struct {
    u32 stringLen;          ///< String length in characters, without NUL-terminator.
    s32 dicStartCursorPos;  ///< Starting cursorPos for the current dictionary word in the current text string. -1 for none.
    s32 dicEndCursorPos;    ///< Ending cursorPos for the current dictionary word in the current text string. -1 for none.
    s32 cursorPos;          ///< Cursor position.
} SwkbdChangedStringArg;

/// Struct data for SwkbdInline Interactive reply storage MovedCursor*, at the end following the string.
typedef struct {
    u32 stringLen;  ///< String length in characters, without NUL-terminator.
    s32 cursorPos;  ///< Cursor position.
} SwkbdMovedCursorArg;

/// Struct data for SwkbdInline Interactive reply storage MovedTab*, at the end following the string.
typedef struct {
    u32 unk_x0;
    u32 unk_x4;
} SwkbdMovedTabArg;

/// Struct data for SwkbdInline Interactive reply storage DecidedEnter*, at the end following the string.
typedef struct {
    u32 stringLen;  ///< String length in characters, without NUL-terminator.
} SwkbdDecidedEnterArg;

/// This callback is used by \ref swkbdInlineUpdate when handling ChangedString* replies (text changed by the user or by \ref swkbdInlineSetInputText).
/// str is the UTF-8 string for the current text.
typedef void (*SwkbdChangedStringCb)(const char* str, SwkbdChangedStringArg* arg);

/// This callback is used by \ref swkbdInlineUpdate when handling MovedCursor* replies.
/// str is the UTF-8 string for the current text.
typedef void (*SwkbdMovedCursorCb)(const char* str, SwkbdMovedCursorArg* arg);

/// This callback is used by \ref swkbdInlineUpdate when handling MovedTab* replies.
/// str is the UTF-8 string for the current text.
typedef void (*SwkbdMovedTabCb)(const char* str, SwkbdMovedTabArg* arg);

/// This callback is used by \ref swkbdInlineUpdate when handling DecidedEnter* replies (when the final text was submitted via the button).
/// str is the UTF-8 string for the current text.
typedef void (*SwkbdDecidedEnterCb)(const char* str, SwkbdDecidedEnterArg* arg);

/// InlineKeyboard
typedef struct {
    u32 version;
    AppletHolder holder;
    SwkbdInlineCalcArg calcArg;
    bool directionalButtonAssignFlag;
    SwkbdState state;

    bool dicCustomInitialized;
    AppletStorage dicStorage;

    bool wordInfoInitialized;
    AppletStorage wordInfoStorage;

    u8* interactive_tmpbuf;
    size_t interactive_tmpbuf_size;
    char* interactive_strbuf;
    size_t interactive_strbuf_size;

    VoidFn finishedInitializeCb;
    SwkbdChangedStringCb changedStringCb;
    SwkbdMovedCursorCb movedCursorCb;
    SwkbdMovedTabCb movedTabCb;
    SwkbdDecidedEnterCb decidedEnterCb;
    VoidFn releasedUserWordInfoCb;
} SwkbdInline;

/**
 * @brief Creates a SwkbdConfig struct.
 * @param c SwkbdConfig struct.
 * @param max_dictwords Max \ref SwkbdDictWord entries, 0 for none.
 */
Result swkbdCreate(SwkbdConfig* c, s32 max_dictwords);

/**
 * @brief Closes a SwkbdConfig struct.
 * @param c SwkbdConfig struct.
 */
void swkbdClose(SwkbdConfig* c);

/**
 * @brief Clears the args in the SwkbdConfig struct and initializes it with the Default Preset.
 * @note Do not use this before \ref swkbdCreate.
 * @note Sets the following fields: type = \ref SwkbdType_QWERTY, initialCursorPos = 1, returnButtonFlag = 1, blurBackground = 1. Pre-5.0.0: textDrawType = SwkbdTextDrawType_Box.
 * @param c SwkbdConfig struct.
 */
void swkbdConfigMakePresetDefault(SwkbdConfig* c);

/**
 * @brief Clears the args in the SwkbdConfig struct and initializes it with the Password Preset.
 * @note Do not use this before \ref swkbdCreate.
 * @note Sets the following fields: type = \ref SwkbdType_QWERTY, initialCursorPos = 1, passwordFlag = 1, blurBackground = 1.
 * @param c SwkbdConfig struct.
 */
void swkbdConfigMakePresetPassword(SwkbdConfig* c);

/**
 * @brief Clears the args in the SwkbdConfig struct and initializes it with the UserName Preset.
 * @note Do not use this before \ref swkbdCreate.
 * @note Sets the following fields: type = \ref SwkbdType_Normal, keySetDisableBitmask = SwkbdKeyDisableBitmask_UserName, initialCursorPos = 1, blurBackground = 1.
 * @param c SwkbdConfig struct.
 */
void swkbdConfigMakePresetUserName(SwkbdConfig* c);

/**
 * @brief Clears the args in the SwkbdConfig struct and initializes it with the DownloadCode Preset.
 * @note Do not use this before \ref swkbdCreate.
 * @note Sets the following fields: type = \ref SwkbdType_Normal (\ref SwkbdType_QWERTY on 5.0.0+), keySetDisableBitmask = SwkbdKeyDisableBitmask_DownloadCode, initialCursorPos = 1, blurBackground = 1. 5.0.0+: stringLenMax = 16, stringLenMaxExt = 1, textDrawType = SwkbdTextDrawType_DownloadCode. unk_x3e0[0-2] = 0x3, 0x7, and 0xb.
 * @param c SwkbdConfig struct.
 */
void swkbdConfigMakePresetDownloadCode(SwkbdConfig* c);

/**
 * @brief Sets the Ok button text. The default is "".
 * @param c SwkbdConfig struct.
 * @param str UTF-8 input string.
 */
void swkbdConfigSetOkButtonText(SwkbdConfig* c, const char* str);

/**
 * @brief Sets the LeftOptionalSymbolKey, for \ref SwkbdType_NumPad. The default is "".
 * @param c SwkbdConfig struct.
 * @param str UTF-8 input string.
 */
void swkbdConfigSetLeftOptionalSymbolKey(SwkbdConfig* c, const char* str);

/**
 * @brief Sets the RightOptionalSymbolKey, for \ref SwkbdType_NumPad. The default is "".
 * @param c SwkbdConfig struct.
 * @param str UTF-8 input string.
 */
void swkbdConfigSetRightOptionalSymbolKey(SwkbdConfig* c, const char* str);

/**
 * @brief Sets the Header text. The default is "".
 * @note See \ref SwkbdArgV0 stringLenMax.
 * @param c SwkbdConfig struct.
 * @param str UTF-8 input string.
 */
void swkbdConfigSetHeaderText(SwkbdConfig* c, const char* str);

/**
 * @brief Sets the Sub text. The default is "".
 * @note See \ref SwkbdArgV0 stringLenMax.
 * @param c SwkbdConfig struct.
 * @param str UTF-8 input string.
 */
void swkbdConfigSetSubText(SwkbdConfig* c, const char* str);

/**
 * @brief Sets the Guide text. The default is "".
 * @note The swkbd applet only displays this when the current displayed cursor position is 0.
 * @param c SwkbdConfig struct.
 * @param str UTF-8 input string.
 */
void swkbdConfigSetGuideText(SwkbdConfig* c, const char* str);

/**
 * @brief Sets the Initial text. The default is "".
 * @param c SwkbdConfig struct.
 * @param str UTF-8 input string.
 */
void swkbdConfigSetInitialText(SwkbdConfig* c, const char* str);

/**
 * @brief Sets the user dictionary.
 * @param c SwkbdConfig struct.
 * @param input Input data.
 * @param entries Total entries in the buffer.
 */
void swkbdConfigSetDictionary(SwkbdConfig* c, const SwkbdDictWord *input, s32 entries);

/**
 * @brief Sets the TextCheck callback.
 * @param c SwkbdConfig struct.
 * @param cb \ref SwkbdTextCheckCb callback.
 */
void swkbdConfigSetTextCheckCallback(SwkbdConfig* c, SwkbdTextCheckCb cb);

/**
 * @brief Launch swkbd with the specified config. This will return once swkbd is finished running.
 * @note The string buffer is also used for the buffer passed to the \ref SwkbdTextCheckCb, when it's set. Hence, in that case this buffer should be large enough to handle TextCheck string input/output. The size passed to the callback is the same size passed here, -1.
 * @param c SwkbdConfig struct.
 * @param out_string UTF-8 Output string buffer.
 * @param out_string_size UTF-8 Output string buffer size, including NUL-terminator.
 */
Result swkbdShow(SwkbdConfig* c, char* out_string, size_t out_string_size);

/**
 * @brief Creates a SwkbdInline object. Only available on 2.0.0+.
 * @note This is essentially an asynchronous version of the regular swkbd.
 * @note This calls \ref swkbdInlineSetUtf8Mode internally with flag=true.
 * @param s SwkbdInline object.
 */
Result swkbdInlineCreate(SwkbdInline* s);

/**
 * @brief Closes a SwkbdInline object. If the applet is running, this will tell the applet to exit, then wait for the applet to exit + applet exit handling.
 * @param s SwkbdInline object.
 */
Result swkbdInlineClose(SwkbdInline* s);

/**
 * @brief Launches the applet with the SwkbdInline object.
 * @param s SwkbdInline object.
 */
Result swkbdInlineLaunch(SwkbdInline* s);

/**
 * @brief Handles updating SwkbdInline state, this should be called periodically.
 * @note Handles applet exit if needed, and also sends the \ref SwkbdInlineCalcArg to the applet if needed. Hence, this should be called at some point after writing to \ref SwkbdInlineCalcArg.
 * @note Handles applet Interactive storage output when needed.
 * @param s SwkbdInline object.
 * @param out_state Optional output \ref SwkbdState.
 */
Result swkbdInlineUpdate(SwkbdInline* s, SwkbdState* out_state);

/**
 * @brief Sets the FinishedInitialize callback, used by \ref swkbdInlineUpdate. The default is NULL for none.
 * @param s SwkbdInline object.
 * @param cb Callback
 */
void swkbdInlineSetFinishedInitializeCallback(SwkbdInline* s, VoidFn cb);

/**
 * @brief Sets the ChangedString callback, used by \ref swkbdInlineUpdate. The default is NULL for none.
 * @param s SwkbdInline object.
 * @param cb \ref SwkbdChangedStringCb Callback
 */
void swkbdInlineSetChangedStringCallback(SwkbdInline* s, SwkbdChangedStringCb cb);

/**
 * @brief Sets the MovedCursor callback, used by \ref swkbdInlineUpdate. The default is NULL for none.
 * @param s SwkbdInline object.
 * @param cb \ref SwkbdMovedCursorCb Callback
 */
void swkbdInlineSetMovedCursorCallback(SwkbdInline* s, SwkbdMovedCursorCb cb);

/**
 * @brief Sets the MovedTab callback, used by \ref swkbdInlineUpdate. The default is NULL for none.
 * @param s SwkbdInline object.
 * @param cb \ref SwkbdMovedTabCb Callback
 */
void swkbdInlineSetMovedTabCallback(SwkbdInline* s, SwkbdMovedTabCb cb);

/**
 * @brief Sets the DecidedEnter callback, used by \ref swkbdInlineUpdate. The default is NULL for none.
 * @param s SwkbdInline object.
 * @param cb \ref SwkbdDecidedEnterCb Callback
 */
void swkbdInlineSetDecidedEnterCallback(SwkbdInline* s, SwkbdDecidedEnterCb cb);

/**
 * @brief Sets the ReleasedUserWordInfo callback, used by \ref swkbdInlineUpdate. The default is NULL for none.
 * @param s SwkbdInline object.
 * @param cb Callback
 */
void swkbdInlineSetReleasedUserWordInfoCallback(SwkbdInline* s, VoidFn cb);

/**
 * @brief Appear the kbd and set \ref SwkbdAppearArg.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param arg Input SwkbdAppearArg.
 */
void swkbdInlineAppear(SwkbdInline* s, SwkbdAppearArg* arg);

/**
 * @brief Disappear the kbd.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 */
void swkbdInlineDisappear(SwkbdInline* s);

/**
 * @brief Creates a \ref SwkbdAppearArg which can then be passed to \ref swkbdInlineAppear. arg is initialized with the defaults, with type being set to the input type.
 * @param arg Output \ref SwkbdAppearArg.
 * @param type \ref SwkbdType type
 * @param str Input UTF-8 string for the Ok button text, this can be empty/NULL to use the default.
 */
void swkbdInlineMakeAppearArg(SwkbdAppearArg* arg, SwkbdType type);

/**
 * @brief Sets okButtonText for the specified SwkbdAppearArg, which was previously initialized with \ref swkbdInlineMakeAppearArg.
 * @param arg \ref SwkbdAppearArg
 * @param str Input UTF-8 string for the Ok button text, this can be empty/NULL to use the default.
 */
void swkbdInlineAppearArgSetOkButtonText(SwkbdAppearArg* arg,  const char* str);

/**
 * @brief Sets the LeftButtonText, for \ref SwkbdType_NumPad. The default is "". Equivalent to \ref swkbdConfigSetLeftOptionalSymbolKey.
 * @param arg \ref SwkbdAppearArg, previously initialized by \ref swkbdInlineMakeAppearArg.
 * @param str UTF-8 input string.
 */
void swkbdInlineAppearArgSetLeftButtonText(SwkbdAppearArg* arg, const char* str);

/**
 * @brief Sets the RightButtonText, for \ref SwkbdType_NumPad. The default is "". Equivalent to \ref sswkbdConfigSetRightOptionalSymbolKey.
 * @param arg \ref SwkbdAppearArg, previously initialized by \ref swkbdInlineMakeAppearArg.
 * @param str UTF-8 input string.
 */
void swkbdInlineAppearArgSetRightButtonText(SwkbdAppearArg* arg, const char* str);

/**
 * @brief Sets the audio volume.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param volume Volume
 */
void swkbdInlineSetVolume(SwkbdInline* s, float volume);

/**
 * @brief Sets the current input text string. Overrides the entire user input string if the user previously entered any text.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @note This will not affect the cursor position, see \ref swkbdInlineSetCursorPos for that.
 * @param s SwkbdInline object.
 * @param str UTF-8 input string.
 */
void swkbdInlineSetInputText(SwkbdInline* s, const char* str);

/**
 * @brief Sets the cursor character position in the string.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param pos Position
 */
void swkbdInlineSetCursorPos(SwkbdInline* s, s32 pos);

/**
 * @brief Sets the UserWordInfo.
 * @note Not avilable when \ref SwkbdState is above \ref SwkbdState_Initialized. Can't be used if this was already used previously.
 * @note The specified buffer must not be used after this, until \ref swkbdInlineClose is used.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards.
 * @note If input==NULL or total_entries==0, this will just call \ref swkbdInlineUnsetUserWordInfo internally.
 * @param s SwkbdInline object.
 * @param input Input data.
 * @param entries Total entries in the buffer.
 */
Result swkbdInlineSetUserWordInfo(SwkbdInline* s, const SwkbdDictWord *input, s32 entries);

/**
 * @brief Request UnsetUserWordInfo.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @note Not avilable when \ref SwkbdState is above \ref SwkbdState_Initialized.
 * @param s SwkbdInline object.
 */
Result swkbdInlineUnsetUserWordInfo(SwkbdInline* s);

/**
 * @brief Sets the utf8Mode.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @note Automatically used internally by \ref swkbdInlineCreate.
 * @param s SwkbdInline object.
 * @param flag Flag
 */
void swkbdInlineSetUtf8Mode(SwkbdInline* s, bool flag);

/**
 * @brief Sets the CustomizeDic.
 * @note Not avilable when \ref SwkbdState is above \ref SwkbdState_Initialized. Can't be used if this was already used previously.
 * @note The specified buffer must not be used after this, until \ref swkbdInlineClose is used. However, it will also become available once \ref swkbdInlineUpdate handles SwkbdReplyType_UnsetCustomizeDic internally.
 * @param s SwkbdInline object.
 * @param buffer 0x1000-byte aligned buffer.
 * @param size 0x1000-byte aligned buffer size.
 * @param info Input \ref SwkbdCustomizeDicInfo
 */
Result swkbdInlineSetCustomizeDic(SwkbdInline* s, void* buffer, size_t size, SwkbdCustomizeDicInfo *info);

/**
 * @brief Request UnsetCustomizeDic.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @note Not avilable when \ref SwkbdState is above \ref SwkbdState_Initialized.
 * @param s SwkbdInline object.
 */
void swkbdInlineUnsetCustomizeDic(SwkbdInline* s);

/**
 * @brief Sets InputModeFadeType.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param type Type
 */
void swkbdInlineSetInputModeFadeType(SwkbdInline* s, u8 type);

/**
 * @brief Sets AlphaEnabledInInputMode.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param flag Flag
 */
void swkbdInlineSetAlphaEnabledInInputMode(SwkbdInline* s, bool flag);

/**
 * @brief Sets KeytopBgAlpha.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param alpha Alpha, clamped to range 0.0f..1.0f.
 */
void swkbdInlineSetKeytopBgAlpha(SwkbdInline* s, float alpha);

/**
 * @brief Sets FooterBgAlpha.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param alpha Alpha, clamped to range 0.0f..1.0f.
 */
void swkbdInlineSetFooterBgAlpha(SwkbdInline* s, float alpha);

/**
 * @brief Sets gfx scaling. Configures KeytopScale* and BalloonScale based on the input value.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param scale Scale
 */
void swkbdInlineSetKeytopScale(SwkbdInline* s, float scale);

/**
 * @brief Sets gfx translation for the displayed swkbd image position.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param x X
 * @param y Y
 */
void swkbdInlineSetKeytopTranslate(SwkbdInline* s, float x, float y);

/**
 * @brief Sets KeytopAsFloating.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param flag Flag
 */
void swkbdInlineSetKeytopAsFloating(SwkbdInline* s, bool flag);

/**
 * @brief Sets FooterScalable.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param flag Flag
 */
void swkbdInlineSetFooterScalable(SwkbdInline* s, bool flag);

/**
 * @brief Sets whether touch is enabled. The default is enabled.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param flag Flag
 */
void swkbdInlineSetTouchFlag(SwkbdInline* s, bool flag);

/**
 * @brief Sets whether USB-keyboard is enabled. The default is enabled.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param flag Flag
 */
void swkbdInlineSetUSBKeyboardFlag(SwkbdInline* s, bool flag);

/**
 * @brief Sets whether DirectionalButtonAssign is enabled. The default is disabled. Only available on 4.0.0+.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param flag Flag
 */
void swkbdInlineSetDirectionalButtonAssignFlag(SwkbdInline* s, bool flag);

/**
 * @brief Sets whether the specified SeGroup (sound effect) is enabled. The default is enabled. Only available on 5.0.0+.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect. If called again with a different seGroup, \ref swkbdInlineUpdate must be called prior to calling this again.
 * @param s SwkbdInline object.
 * @param seGroup SeGroup
 * @param flag Flag
 */
void swkbdInlineSetSeGroup(SwkbdInline* s, u8 seGroup, bool flag);

/**
 * @brief Sets whether the backspace button is enabled. The default is enabled. Only available on 5.0.0+.
 * @note \ref swkbdInlineUpdate must be called at some point afterwards for this to take affect.
 * @param s SwkbdInline object.
 * @param flag Flag
 */
void swkbdInlineSetBackspaceFlag(SwkbdInline* s, bool flag);

