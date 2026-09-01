#include "proxies/winmm_proxy.hpp"
#include "proxies/proxy_helper.hpp"

namespace {

HMODULE g_hRealWinmm = nullptr;

void InitProxy() noexcept {
    if (!g_hRealWinmm) {
        g_hRealWinmm = ler::proxies::ProxyHelper::LoadSystemLibrary(L"winmm.dll");
    }
}

#define FORWARD_CALL(ret_type, name, args_decl, args_pass) \
    typedef ret_type (WINAPI *pfn_##name) args_decl; \
    static pfn_##name Orig_##name = nullptr; \
    extern "C" ret_type WINAPI Proxy_##name args_decl { \
        InitProxy(); \
        if (!Orig_##name && g_hRealWinmm) { \
            Orig_##name = reinterpret_cast<pfn_##name>(ler::proxies::ProxyHelper::GetProc(g_hRealWinmm, #name)); \
        } \
        if (Orig_##name) return Orig_##name args_pass; \
        return (ret_type)0; \
    }

FORWARD_CALL(BOOL, PlaySound, (LPCSTR pszSound, HMODULE hmod, DWORD fdwSound), (pszSound, hmod, fdwSound))
FORWARD_CALL(BOOL, PlaySoundA, (LPCSTR pszSound, HMODULE hmod, DWORD fdwSound), (pszSound, hmod, fdwSound))
FORWARD_CALL(BOOL, PlaySoundW, (LPCWSTR pszSound, HMODULE hmod, DWORD fdwSound), (pszSound, hmod, fdwSound))
FORWARD_CALL(BOOL, sndPlaySoundA, (LPCSTR pszSound, UINT fuSound), (pszSound, fuSound))
FORWARD_CALL(BOOL, sndPlaySoundW, (LPCWSTR pszSound, UINT fuSound), (pszSound, fuSound))

FORWARD_CALL(MMRESULT, timeBeginPeriod, (UINT uPeriod), (uPeriod))
FORWARD_CALL(MMRESULT, timeEndPeriod, (UINT uPeriod), (uPeriod))
FORWARD_CALL(MMRESULT, timeGetDevCaps, (LPTIMECAPS ptc, UINT cbtc), (ptc, cbtc))
FORWARD_CALL(MMRESULT, timeGetSystemTime, (LPMMTIME pmmt, UINT cbmmt), (pmmt, cbmmt))
FORWARD_CALL(DWORD, timeGetTime, (void), ())
FORWARD_CALL(MMRESULT, timeKillEvent, (UINT uTimerID), (uTimerID))
FORWARD_CALL(MMRESULT, timeSetEvent, (UINT uDelay, UINT uResolution, LPTIMECALLBACK fptc, DWORD_PTR dwUser, UINT fuEvent), (uDelay, uResolution, fptc, dwUser, fuEvent))

FORWARD_CALL(MMRESULT, waveOutOpen, (LPHWAVEOUT phwo, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen), (phwo, uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen))
FORWARD_CALL(MMRESULT, waveOutClose, (HWAVEOUT hwo), (hwo))
FORWARD_CALL(MMRESULT, waveOutPrepareHeader, (HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh), (hwo, pwh, cbwh))
FORWARD_CALL(MMRESULT, waveOutUnprepareHeader, (HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh), (hwo, pwh, cbwh))
FORWARD_CALL(MMRESULT, waveOutWrite, (HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh), (hwo, pwh, cbwh))
FORWARD_CALL(MMRESULT, waveOutPause, (HWAVEOUT hwo), (hwo))
FORWARD_CALL(MMRESULT, waveOutRestart, (HWAVEOUT hwo), (hwo))
FORWARD_CALL(MMRESULT, waveOutReset, (HWAVEOUT hwo), (hwo))
FORWARD_CALL(MMRESULT, waveOutBreakLoop, (HWAVEOUT hwo), (hwo))
FORWARD_CALL(MMRESULT, waveOutGetPosition, (HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt), (hwo, pmmt, cbmmt))
FORWARD_CALL(MMRESULT, waveOutGetPitch, (HWAVEOUT hwo, LPDWORD pdwPitch), (hwo, pdwPitch))
FORWARD_CALL(MMRESULT, waveOutSetPitch, (HWAVEOUT hwo, DWORD dwPitch), (hwo, dwPitch))
FORWARD_CALL(MMRESULT, waveOutGetPlaybackRate, (HWAVEOUT hwo, LPDWORD pdwRate), (hwo, pdwRate))
FORWARD_CALL(MMRESULT, waveOutSetPlaybackRate, (HWAVEOUT hwo, DWORD dwRate), (hwo, dwRate))
FORWARD_CALL(MMRESULT, waveOutGetVolume, (HWAVEOUT hwo, LPDWORD pdwVolume), (hwo, pdwVolume))
FORWARD_CALL(MMRESULT, waveOutSetVolume, (HWAVEOUT hwo, DWORD dwVolume), (hwo, dwVolume))
FORWARD_CALL(MMRESULT, waveOutGetErrorTextA, (MMRESULT mmrError, LPSTR pszText, UINT cchText), (mmrError, pszText, cchText))
FORWARD_CALL(MMRESULT, waveOutGetErrorTextW, (MMRESULT mmrError, LPWSTR pszText, UINT cchText), (mmrError, pszText, cchText))
FORWARD_CALL(UINT, waveOutGetNumDevs, (void), ())
FORWARD_CALL(MMRESULT, waveOutGetDevCapsA, (UINT_PTR uDeviceID, LPWAVEOUTCAPSA pwoc, UINT cbwoc), (uDeviceID, pwoc, cbwoc))
FORWARD_CALL(MMRESULT, waveOutGetDevCapsW, (UINT_PTR uDeviceID, LPWAVEOUTCAPSW pwoc, UINT cbwoc), (uDeviceID, pwoc, cbwoc))
FORWARD_CALL(MMRESULT, waveOutGetID, (HWAVEOUT hwo, LPUINT puDeviceID), (hwo, puDeviceID))
FORWARD_CALL(MMRESULT, waveOutMessage, (HWAVEOUT hwo, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2), (hwo, uMsg, dw1, dw2))

FORWARD_CALL(MMRESULT, waveInOpen, (LPHWAVEIN phwi, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen), (phwi, uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen))
FORWARD_CALL(MMRESULT, waveInClose, (HWAVEIN hwi), (hwi))
FORWARD_CALL(MMRESULT, waveInPrepareHeader, (HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh), (hwi, pwh, cbwh))
FORWARD_CALL(MMRESULT, waveInUnprepareHeader, (HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh), (hwi, pwh, cbwh))
FORWARD_CALL(MMRESULT, waveInAddBuffer, (HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh), (hwi, pwh, cbwh))
FORWARD_CALL(MMRESULT, waveInStart, (HWAVEIN hwi), (hwi))
FORWARD_CALL(MMRESULT, waveInStop, (HWAVEIN hwi), (hwi))
FORWARD_CALL(MMRESULT, waveInReset, (HWAVEIN hwi), (hwi))
FORWARD_CALL(MMRESULT, waveInGetPosition, (HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt), (hwi, pmmt, cbmmt))
FORWARD_CALL(MMRESULT, waveInGetErrorTextA, (MMRESULT mmrError, LPSTR pszText, UINT cchText), (mmrError, pszText, cchText))
FORWARD_CALL(MMRESULT, waveInGetErrorTextW, (MMRESULT mmrError, LPWSTR pszText, UINT cchText), (mmrError, pszText, cchText))
FORWARD_CALL(UINT, waveInGetNumDevs, (void), ())
FORWARD_CALL(MMRESULT, waveInGetDevCapsA, (UINT_PTR uDeviceID, LPWAVEINCAPSA pwic, UINT cbwic), (uDeviceID, pwic, cbwic))
FORWARD_CALL(MMRESULT, waveInGetDevCapsW, (UINT_PTR uDeviceID, LPWAVEOUTCAPSW pwic, UINT cbwic), (uDeviceID, pwic, cbwic))
FORWARD_CALL(MMRESULT, waveInGetID, (HWAVEIN hwi, LPUINT puDeviceID), (hwi, puDeviceID))
FORWARD_CALL(MMRESULT, waveInMessage, (HWAVEIN hwi, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2), (hwi, uMsg, dw1, dw2))

FORWARD_CALL(MMRESULT, midiOutOpen, (LPHMIDIOUT phmo, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen), (phmo, uDeviceID, dwCallback, dwInstance, fdwOpen))
FORWARD_CALL(MMRESULT, midiOutClose, (HMIDIOUT hmo), (hmo))
FORWARD_CALL(MMRESULT, midiOutPrepareHeader, (HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh), (hmo, pmh, cbmh))
FORWARD_CALL(MMRESULT, midiOutUnprepareHeader, (HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh), (hmo, pmh, cbmh))
FORWARD_CALL(MMRESULT, midiOutShortMsg, (HMIDIOUT hmo, DWORD dwMsg), (hmo, dwMsg))
FORWARD_CALL(MMRESULT, midiOutLongMsg, (HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh), (hmo, pmh, cbmh))
FORWARD_CALL(MMRESULT, midiOutReset, (HMIDIOUT hmo), (hmo))
FORWARD_CALL(MMRESULT, midiOutGetVolume, (HMIDIOUT hmo, LPDWORD pdwVolume), (hmo, pdwVolume))
FORWARD_CALL(MMRESULT, midiOutSetVolume, (HMIDIOUT hmo, DWORD dwVolume), (hmo, dwVolume))
FORWARD_CALL(MMRESULT, midiOutGetErrorTextA, (MMRESULT mmrError, LPSTR pszText, UINT cchText), (mmrError, pszText, cchText))
FORWARD_CALL(MMRESULT, midiOutGetErrorTextW, (MMRESULT mmrError, LPWSTR pszText, UINT cchText), (mmrError, pszText, cchText))
FORWARD_CALL(UINT, midiOutGetNumDevs, (void), ())
FORWARD_CALL(MMRESULT, midiOutGetDevCapsA, (UINT_PTR uDeviceID, LPMIDIOUTCAPSA pmoc, UINT cbmoc), (uDeviceID, pmoc, cbmoc))
FORWARD_CALL(MMRESULT, midiOutGetDevCapsW, (UINT_PTR uDeviceID, LPMIDIOUTCAPSW pmoc, UINT cbmoc), (uDeviceID, pmoc, cbmoc))
FORWARD_CALL(MMRESULT, midiOutGetID, (HMIDIOUT hmo, LPUINT puDeviceID), (hmo, puDeviceID))
FORWARD_CALL(MMRESULT, midiOutMessage, (HMIDIOUT hmo, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2), (hmo, uMsg, dw1, dw2))

FORWARD_CALL(MCIERROR, mciSendCommandA, (MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2), (mciId, uMsg, dwParam1, dwParam2))
FORWARD_CALL(MCIERROR, mciSendCommandW, (MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2), (mciId, uMsg, dwParam1, dwParam2))
FORWARD_CALL(MCIERROR, mciSendStringA, (LPCSTR lpstrCommand, LPSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback), (lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback))
FORWARD_CALL(MCIERROR, mciSendStringW, (LPCWSTR lpstrCommand, LPWSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback), (lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback))
FORWARD_CALL(BOOL, mciGetErrorStringA, (MCIERROR mcierr, LPSTR pszText, UINT cchText), (mcierr, pszText, cchText))
FORWARD_CALL(BOOL, mciGetErrorStringW, (MCIERROR mcierr, LPWSTR pszText, UINT cchText), (mcierr, pszText, cchText))
FORWARD_CALL(MCIDEVICEID, mciGetDeviceIDA, (LPCSTR pszDevice), (pszDevice))
FORWARD_CALL(MCIDEVICEID, mciGetDeviceIDW, (LPCWSTR pszDevice), (pszDevice))
FORWARD_CALL(BOOL, mciExecute, (LPCSTR pszCommand), (pszCommand))

FORWARD_CALL(HMMIO, mmioOpenA, (LPSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen), (pszFileName, pmmioinfo, fdwOpen))
FORWARD_CALL(HMMIO, mmioOpenW, (LPWSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen), (pszFileName, pmmioinfo, fdwOpen))
FORWARD_CALL(MMRESULT, mmioClose, (HMMIO hmmio, UINT fuClose), (hmmio, fuClose))
FORWARD_CALL(LONG, mmioRead, (HMMIO hmmio, HPSTR pch, LONG cch), (hmmio, pch, cch))
FORWARD_CALL(LONG, mmioWrite, (HMMIO hmmio, const char *pch, LONG cch), (hmmio, pch, cch))
FORWARD_CALL(LONG, mmioSeek, (HMMIO hmmio, LONG lOffset, int iOrigin), (hmmio, lOffset, iOrigin))
FORWARD_CALL(MMRESULT, mmioGetInfo, (HMMIO hmmio, LPMMIOINFO pmmioinfo, UINT fuInfo), (hmmio, pmmioinfo, fuInfo))
FORWARD_CALL(MMRESULT, mmioSetInfo, (HMMIO hmmio, LPCMMIOINFO pmmioinfo, UINT fuInfo), (hmmio, pmmioinfo, fuInfo))
FORWARD_CALL(MMRESULT, mmioSetBuffer, (HMMIO hmmio, LPSTR pchBuffer, LONG cchBuffer, UINT fuBuffer), (hmmio, pchBuffer, cchBuffer, fuBuffer))
FORWARD_CALL(MMRESULT, mmioFlush, (HMMIO hmmio, UINT fuFlush), (hmmio, fuFlush))
FORWARD_CALL(MMRESULT, mmioAdvance, (HMMIO hmmio, LPMMIOINFO pmmioinfo, UINT fuAdvance), (hmmio, pmmioinfo, fuAdvance))
FORWARD_CALL(MMRESULT, mmioAscend, (HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuAscend), (hmmio, pmmcki, fuAscend))
FORWARD_CALL(MMRESULT, mmioDescend, (HMMIO hmmio, LPMMCKINFO pmmcki, const MMCKINFO *pmmckiParent, UINT fuDescend), (hmmio, pmmcki, pmmckiParent, fuDescend))
FORWARD_CALL(MMRESULT, mmioCreateChunk, (HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuCreate), (hmmio, pmmcki, fuCreate))
FORWARD_CALL(FOURCC, mmioStringToFOURCCA, (LPCSTR sz, UINT uFlags), (sz, uFlags))
FORWARD_CALL(FOURCC, mmioStringToFOURCCW, (LPCWSTR sz, UINT uFlags), (sz, uFlags))

FORWARD_CALL(MMRESULT, joyGetPos, (UINT uJoyID, LPJOYINFO pji), (uJoyID, pji))
FORWARD_CALL(MMRESULT, joyGetPosEx, (UINT uJoyID, LPJOYINFOEX pji), (uJoyID, pji))
FORWARD_CALL(UINT, joyGetNumDevs, (void), ())
FORWARD_CALL(MMRESULT, joyGetDevCapsA, (UINT_PTR uJoyID, LPJOYCAPSA pjc, UINT cbjc), (uJoyID, pjc, cbjc))
FORWARD_CALL(MMRESULT, joyGetDevCapsW, (UINT_PTR uJoyID, LPJOYCAPSW pjc, UINT cbjc), (uJoyID, pjc, cbjc))
FORWARD_CALL(MMRESULT, joyGetThreshold, (UINT uJoyID, LPUINT puThreshold), (uJoyID, puThreshold))
FORWARD_CALL(MMRESULT, joySetCapture, (HWND hwnd, UINT uJoyID, UINT uPeriod, BOOL fChanged), (hwnd, uJoyID, uPeriod, fChanged))
FORWARD_CALL(MMRESULT, joyReleaseCapture, (UINT uJoyID), (uJoyID))
FORWARD_CALL(MMRESULT, joySetThreshold, (UINT uJoyID, UINT uThreshold), (uJoyID, uThreshold))

FORWARD_CALL(UINT, auxGetNumDevs, (void), ())
FORWARD_CALL(MMRESULT, auxGetDevCapsA, (UINT_PTR uDeviceID, LPAUXCAPSA pac, UINT cbac), (uDeviceID, pac, cbac))
FORWARD_CALL(MMRESULT, auxGetDevCapsW, (UINT_PTR uDeviceID, LPAUXCAPSW pac, UINT cbac), (uDeviceID, pac, cbac))
FORWARD_CALL(MMRESULT, auxGetVolume, (UINT uDeviceID, LPDWORD pdwVolume), (uDeviceID, pdwVolume))
FORWARD_CALL(MMRESULT, auxSetVolume, (UINT uDeviceID, DWORD dwVolume), (uDeviceID, dwVolume))
FORWARD_CALL(MMRESULT, auxOutMessage, (UINT uDeviceID, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2), (uDeviceID, uMsg, dw1, dw2))

FORWARD_CALL(MMRESULT, mixerOpen, (LPHMIXER phmx, UINT uMxId, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen), (phmx, uMxId, dwCallback, dwInstance, fdwOpen))
FORWARD_CALL(MMRESULT, mixerClose, (HMIXER hmx), (hmx))
FORWARD_CALL(MMRESULT, mixerGetDevCapsA, (UINT_PTR uMxId, LPMIXERCAPSA pmxcaps, UINT cbmxcaps), (uMxId, pmxcaps, cbmxcaps))
FORWARD_CALL(MMRESULT, mixerGetDevCapsW, (UINT_PTR uMxId, LPMIXERCAPSW pmxcaps, UINT cbmxcaps), (uMxId, pmxcaps, cbmxcaps))
FORWARD_CALL(UINT, mixerGetNumDevs, (void), ())
FORWARD_CALL(MMRESULT, mixerGetLineInfoA, (HMIXEROBJ hmxobj, LPMIXERLINEA pmxl, DWORD fdwInfo), (hmxobj, pmxl, fdwInfo))
FORWARD_CALL(MMRESULT, mixerGetLineInfoW, (HMIXEROBJ hmxobj, LPMIXERLINEW pmxl, DWORD fdwInfo), (hmxobj, pmxl, fdwInfo))
FORWARD_CALL(MMRESULT, mixerGetLineControlsA, (HMIXEROBJ hmxobj, LPMIXERLINECONTROLSA pmxlc, DWORD fdwControls), (hmxobj, pmxlc, fdwControls))
FORWARD_CALL(MMRESULT, mixerGetLineControlsW, (HMIXEROBJ hmxobj, LPMIXERLINECONTROLSW pmxlc, DWORD fdwControls), (hmxobj, pmxlc, fdwControls))
FORWARD_CALL(MMRESULT, mixerGetControlDetailsA, (HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails), (hmxobj, pmxcd, fdwDetails))
FORWARD_CALL(MMRESULT, mixerGetControlDetailsW, (HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails), (hmxobj, pmxcd, fdwDetails))
FORWARD_CALL(MMRESULT, mixerSetControlDetails, (HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails), (hmxobj, pmxcd, fdwDetails))
FORWARD_CALL(MMRESULT, mixerMessage, (HMIXER hmx, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2), (hmx, uMsg, dw1, dw2))

FORWARD_CALL(UINT, mmsystemGetVersion, (void), ())

} // namespace

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hinstDLL);
            InitProxy();
            ler::proxies::ProxyHelper::LoadLerRuntime(hinstDLL);
            break;
        case DLL_PROCESS_DETACH:
            if (g_hRealWinmm) {
                ::FreeLibrary(g_hRealWinmm);
                g_hRealWinmm = nullptr;
            }
            break;
    }
    return TRUE;
}
