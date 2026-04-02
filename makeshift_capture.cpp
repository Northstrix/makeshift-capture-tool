#define _USE_MATH_DEFINES
#ifndef BOOST_ALL_NO_LIB
#define BOOST_ALL_NO_LIB
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <math.h>
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <commctrl.h>
#include <objbase.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;

#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using std::min;
using std::max;
using std::string;

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#include "nuklear.h"

#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"advapi32.lib")

#define APP_VERSION  "v1.0"
#define APP_NAME     "Makeshift Capture Tool"
#define APP_CLASS    "MakeshiftCaptureToolWindow"
#define IDR_LOGO     100

/* ==================================================================
   LOCALIZATION
   To add a language:
     1. Increment LANG_COUNT
     2. Add { "code", "Display Name" } to g_langs[]
     3. Add one more column to LANG_TABLE[]
   ================================================================== */
#define LANG_COUNT 10
struct LangMeta { const char *code; const char *displayName; };
static const LangMeta g_langs[LANG_COUNT] = {
    { "en", "English" },
    { "es", "Español" },
    { "pt",  "Português" },
    { "it", "Italiano" },
    { "ja", "日本語" },
    { "yue", "廣東話" },
    { "hi", "हिन्दी" },
    { "pl", "Polski" },
    { "tr", "Türkçe" },
    { "vi", "Tiếng Việt" }
};
enum StrID {
    /* tabs */
    S_TAB_CAPTURE, S_TAB_SHOT, S_TAB_SETTINGS, S_TAB_ABOUT,
    /* capture mode names (button labels AND tray items) */
    S_MODE_RECT, S_MODE_FULL, S_MODE_FIXED,
    /* actions */
    S_ACT_SHOT,
    /* hotkey section labels (in Screenshot tab) */
    S_HK_LABEL_RECT, S_HK_LABEL_FULL, S_HK_LABEL_FIXED,
    S_HK_PRESS_TO_SET, S_HK_CURRENT, S_HK_CLEAR, S_HK_NONE,
    /* settings field names */
    S_SET_OUTDIR, S_SET_SHOT_PFX, S_SET_TIMESTAMP,
    S_SET_TOP, S_SET_TRAY, S_SET_NOTIF, S_SET_LANG, S_SET_SAVE,
    S_FIXED_XYWH,
    /* status */
    S_STATUS, S_READY, S_CANCELLED, S_SHOT_SAVED, S_SHOT_FAIL,
    /* selection overlay */
    S_SEL_DRAG,
    /* tray */
    S_TRAY_OPEN, S_TRAY_EXIT,
    S_TRAY_RECT, S_TRAY_FULL, S_TRAY_FIXED,
    S_OPEN_FOLDER,
    /* about */
    S_ABOUT_DESC, S_ABOUT_GITHUB,
    S_ABOUT_TPLIBS, S_ABOUT_LICENSE,
    S_ABOUT_MADEBY, S_ABOUT_USING, S_ABOUT_AND,
    STR_COUNT
};

static const char *LANG_TABLE[LANG_COUNT][STR_COUNT] = {
    /* ---- English ---- */
    {
        /* tabs */           "Capture","Screenshot","Settings","About",
        /* modes */          "Rectangular Capture","Full Screen","Preset Area",
        /* action */         "Screenshot",
        /* hk labels */      "Rectangular Capture","Full Screen","Preset Area",
        /* hk ui */          "Press any key combination...","Current:","Clear","None",
        /* settings */       "Output folder","File prefix","Timestamp filenames",
                            "Always on top","Minimize to tray","Notifications",
                            "Language","Save",
        /* fixed region */   "Preset region  X, Y, Width, Height",
        /* status */         "STATUS","Ready.","Cancelled.","Saved: ","Capture failed!",
        /* sel overlay */    "Drag to select   |   ESC to cancel",
        /* tray */           "Open","Exit",
                            "Rectangular Capture","Full Screen","Preset Area",
        /* folder */         "Open Output Folder",
        /* about */          "Screenshot capture tool for Windows 10/11",
                            "GitHub",
                            "Third-party Licenses","License",
        /* credits */        "Made by","using","and"
    },
    /* ---- Español (LATAM) ---- */
    {
        /* tabs */           "Captura","Captura de Pantalla","Configuración","Acerca de",
        /* modes */          "Captura Rectangular","Pantalla Completa","Área Preestablecida",
        /* action */         "Capturar Pantalla",
        /* hk labels */      "Captura Rectangular","Pantalla Completa","Área Preestablecida",
        /* hk ui */          "Presiona cualquier combinación de teclas...","Actual:","Limpiar","Ninguna",
        /* settings */       "Carpeta de salida","Prefijo de archivo","Incluir marca de tiempo",
                                "Siempre arriba","Minimizar a bandeja","Notificaciones",
                                "Idioma","Guardar",
        /* fixed region */   "Región preestablecida X, Y, Ancho, Alto",
        /* status */         "ESTADO","Listo.","Cancelado.","Guardado: ","¡Error al capturar!",
        /* sel overlay */    "Arrastra para seleccionar | ESC para cancelar",
        /* tray */           "Abrir","Salir",
                                "Captura Rectangular","Pantalla Completa","Área Preestablecida",
        /* folder */         "Abrir Carpeta de Salida",
        /* about */          "Herramienta de captura de pantalla para Windows 10/11",
                                "GitHub",
                                "Licencias de terceros","Licencia",
        /* credits */        "Creado por","usando","y"
    },
    /* ---- Português (Brasil) ---- */
    {
        /* tabs */           "Captura","Screenshot","Configurações","Sobre",
        /* modes */          "Captura Retangular","Tela Cheia","Área Fixa",
        /* action */         "Screenshot",
        /* hk labels */      "Captura Retangular","Tela Cheia","Área Fixa",
        /* hk ui */          "Pressione qualquer combinação de teclas...","Atual:","Limpar","Nenhuma",
        /* settings */       "Pasta de saída","Prefixo do arquivo","Incluir timestamp",
                                "Sempre no topo","Minimizar na bandeja","Notificações",
                                "Idioma","Salvar",
        /* fixed region */   "Região fixa X, Y, Largura, Altura",
        /* status */         "STATUS","Pronto.","Cancelado.","Salvo: ","Falha na captura!",
        /* sel overlay */    "Arraste para selecionar | ESC para cancelar",
        /* tray */           "Abrir","Sair",
                                "Captura Retangular","Tela Cheia","Área Fixa",
        /* folder */         "Abrir Pasta de Saída",
        /* about */          "Ferramenta de captura de tela para Windows 10/11",
                                "GitHub",
                                "Licenças de terceiros","Licença",
        /* credits */        "Feito por","usando","e"
    },
    /* ---- Italiano ---- */
    {
        /* tabs */           "Cattura","Screenshot","Impostazioni","Info",
        /* modes */          "Cattura Rettangolare","Schermo Intero","Area Preimpostata",
        /* action */         "Screenshot",
        /* hk labels */      "Cattura Rettangolare","Schermo Intero","Area Preimpostata",
        /* hk ui */          "Premi qualsiasi combinazione di tasti...","Attuale:","Cancella","Nessuna",
        /* settings */       "Cartella output","Prefisso file","Timestamp sui nomi",
                             "Sempre in primo piano","Minimizza in tray","Notifiche",
                             "Lingua","Salva",
        /* fixed region */   "Regione fissa X, Y, Larghezza, Altezza",
        /* status */         "STATO","Pronto.","Annullato.","Salvato: ","Cattura fallita!",
        /* sel overlay */    "Trascina per selezionare | ESC per annullare",
        /* tray */           "Apri","Esci",
                             "Cattura Rettangolare","Schermo Intero","Area Preimpostata",
        /* folder */         "Apri Cartella Output",
        /* about */          "Strumento di cattura screenshot per Windows 10/11",
                             "GitHub",
                             "Licenze terze parti","Licenza",
        /* credits */        "Fatto da","usando","e"
    },
    /* ---- 日本語 ---- */
    {
        /* tabs */           "キャプチャ","スクリーンショット","設定","情報",
        /* modes */          "矩形キャプチャ","全画面","固定領域",
        /* action */         "スクリーンショット",
        /* hk labels */      "矩形キャプチャ","全画面","固定領域",
        /* hk ui */          "任意のキー組み合わせを押してください...","現在:","クリア","なし",
        /* settings */       "出力フォルダ","ファイルプレフィックス","タイムスタンプ",
                             "常に最前面","トレイに最小化","通知",
                             "言語","保存",
        /* fixed region */   "固定領域 X, Y, 幅, 高さ",
        /* status */         "状態","準備完了。","キャンセル。","保存: ","キャプチャ失敗！",
        /* sel overlay */    "ドラッグして選択 | ESCでキャンセル",
        /* tray */           "開く","終了",
                             "矩形キャプチャ","全画面","固定領域",
        /* folder */         "出力フォルダを開く",
        /* about */          "Windows 10/11用スクリーンショットツール",
                             "GitHub",
                             "サードパーティライセンス","ライセンス",
        /* credits */        "作成","使用:","と"
    },
    /* ---- 廣東話 ---- */
    {
        /* tabs */           "截圖","屏幕截圖","設定","關於",
        /* modes */          "矩形截圖","全螢幕","預設區域",
        /* action */         "截圖",
        /* hk labels */      "矩形截圖","全螢幕","預設區域",
        /* hk ui */          "按下任何組合鍵...","目前:","清除","無",
        /* settings */       "輸出資料夾","檔案前綴","加上時間戳",
                             "永遠置頂","縮至系統匣","通知",
                             "語言","儲存",
        /* fixed region */   "預設區域 X, Y, 闊度, 高度",
        /* status */         "狀態","已就緒。","已取消。","已儲存: ","截圖失敗！",
        /* sel overlay */    "拖曳選擇 | ESC 取消",
        /* tray */           "開啟","退出",
                             "矩形截圖","全螢幕","預設區域",
        /* folder */         "開啟輸出資料夾",
        /* about */          "Windows 10/11 截圖工具",
                             "GitHub",
                             "第三方授權","授權",
        /* credits */        "製作","使用","及"
    },
    /* ---- हिन्दी ---- */
    {
        /* tabs */           "कैप्चर","स्क्रीनशॉट","सेटिंग्स","के बारे में",
        /* modes */          "आयताकार कैप्चर","पूर्ण स्क्रीन","निर्धारित क्षेत्र",
        /* action */         "स्क्रीनशॉट",
        /* hk labels */      "आयताकार कैप्चर","पूर्ण स्क्रीन","निर्धारित क्षेत्र",
        /* hk ui */          "किसी भी कुंजी संयोजन को दबाएं...","वर्तमान:","साफ़ करें","कोई नहीं",
        /* settings */       "आउटपुट फ़ोल्डर","फ़ाइल उपसर्ग","समय-चिह्न नाम",
                           "हमेशा ऊपर","ट्रे में कम करें","सूचनाएँ",
                           "भाषा","सहेजें",
        /* fixed region */   "निर्धारित क्षेत्र X, Y, चौड़ाई, ऊंचाई",
        /* status */         "स्थिति","तैयार।","रद्द।","सहेजा गया: ","कैप्चर विफल!",
        /* sel overlay */    "चुनने के लिए ड्रैग करें | ESC रद्द करने के लिए",
        /* tray */           "खोलें","निकास",
                           "आयताकार कैप्चर","पूर्ण स्क्रीन","निर्धारित क्षेत्र",
        /* folder */         "आउटपुट फ़ोल्डर खोलें",
        /* about */          "Windows 10/11 के लिए स्क्रीनशॉट टूल",
                           "GitHub",
                           "थर्ड-पार्टी लाइसेंस","लाइसेंस",
        /* credits */        "द्वारा बनाया गया","उपयोग","और"
    },
    /* ---- Polski ---- */
    {
        /* tabs */           "Zrzut","Screenshot","Ustawienia","O programie",
        /* modes */          "Zrzut prostokątny","Pełny ekran","Stały obszar",
        /* action */         "Screenshot",
        /* hk labels */      "Zrzut prostokątny","Pełny ekran","Stały obszar",
        /* hk ui */          "Naciśnij dowolną kombinację klawiszy...","Aktualny:","Wyczyść","Brak",
        /* settings */       "Folder wyjściowy","Prefiks pliku","Znacznik czasu",
                             "Zawsze na wierzchu","Minimalizuj do zasobnika","Powiadomienia",
                             "Język","Zapisz",
        /* fixed region */   "Stały obszar X, Y, Szerokość, Wysokość",
        /* status */         "STATUS","Gotowy.","Anulowano.","Zapisano: ","Błąd zrzutu!",
        /* sel overlay */    "Przeciągnij aby wybrać | ESC anuluj",
        /* tray */           "Otwórz","Wyjdź",
                             "Zrzut prostokątny","Pełny ekran","Stały obszar",
        /* folder */         "Otwórz folder wyjściowy",
        /* about */          "Narzędzie do zrzutów ekranu dla Windows 10/11",
                             "GitHub",
                             "Licencje zewnętrzne","Licencja",
        /* credits */        "Stworzone przez","używając","i"
    },
    /* ---- Türkçe ---- */
    {
        /* tabs */           "Ekran Görüntüsü","Yakala","Ayarlar","Hakkında",
        /* modes */          "Dikdörtgen Yakalama","Tam Ekran","Sabit Alan",
        /* action */         "Ekran Görüntüsü",
        /* hk labels */      "Dikdörtgen Yakalama","Tam Ekran","Sabit Alan",
        /* hk ui */          "Herhangi bir tuş kombinasyonuna basın...","Mevcut:","Temizle","Yok",
        /* settings */       "Çıktı klasörü","Dosya öneki","Zaman damgası",
                             "Her zaman üstte","Tepsi'ye küçült","Bildirimler",
                             "Dil","Kaydet",
        /* fixed region */   "Sabit bölge X, Y, Genişlik, Yükseklik",
        /* status */         "DURUM","Hazır.","İptal edildi.","Kaydedildi: ","Yakalama başarısız!",
        /* sel overlay */    "Seçmek için sürükle | ESC iptal et",
        /* tray */           "Aç","Çıkış",
                             "Dikdörtgen Yakalama","Tam Ekran","Sabit Alan",
        /* folder */         "Çıktı Klasörünü Aç",
        /* about */          "Windows 10/11 için ekran görüntüsü aracı",
                             "GitHub",
                             "Üçüncü taraf lisanslar","Lisans",
        /* credits */        "Tarafından yapıldı","kullanarak","ve"
    },
    /* ---- Tiếng Việt ---- */
    {
        /* tabs */           "Chụp","Chụp màn hình","Cài đặt","Giới thiệu",
        /* modes */          "Chụp hình chữ nhật","Toàn màn hình","Vùng cố định",
        /* action */         "Chụp màn hình",
        /* hk labels */      "Chụp hình chữ nhật","Toàn màn hình","Vùng cố định",
        /* hk ui */          "Nhấn bất kỳ tổ hợp phím nào...","Hiện tại:","Xóa","Không có",
        /* settings */       "Thư mục đầu ra","Tiền tố tệp","Dấu thời gian",
                             "Luôn ở trên cùng","Thu nhỏ vào khay","Thông báo",
                             "Ngôn ngữ","Lưu",
        /* fixed region */   "Vùng cố định X, Y, Chiều rộng, Chiều cao",
        /* status */         "TRẠNG THÁI","Sẵn sàng.","Đã hủy.","Đã lưu: ","Chụp thất bại!",
        /* sel overlay */    "Kéo để chọn | ESC để hủy",
        /* tray */           "Mở","Thoát",
                             "Chụp hình chữ nhật","Toàn màn hình","Vùng cố định",
        /* folder */         "Mở Thư mục Đầu ra",
        /* about */          "Công cụ chụp màn hình cho Windows 10/11",
                             "GitHub",
                             "Giấy phép bên thứ ba","Giấy phép",
        /* credits */        "Tạo bởi","sử dụng","và"
    },
};
static int g_lang = 0;
#define T(id) (LANG_TABLE[g_lang][(id)])

/* ==================================================================
   COLORS
   ================================================================== */
#define C_BG     nk_rgb( 10,  9, 16)
#define C_SURF   nk_rgb( 18, 16, 27)
#define C_CARD   nk_rgb( 26, 23, 40)
#define C_CARD2  nk_rgb( 35, 31, 52)
#define C_BORDER nk_rgb( 52, 45, 78)
#define C_PUR    nk_rgb(139, 92,246)
#define C_PUR_L  nk_rgb(167,139,250)
#define C_PUR_D  nk_rgb( 72, 44,130)
#define C_RED    nk_rgb(239, 68, 68)
#define C_GREEN  nk_rgb( 52,211,153)
#define C_AMBER  nk_rgb(251,191, 36)
#define C_TEXT   nk_rgb(238,234,255)
#define C_MUTED  nk_rgb(130,118,165)
#define C_DIM    nk_rgb( 68, 60, 95)

/* ==================================================================
   GDI FONT  (Onda Sfasata pattern)
   ================================================================== */
typedef struct nk_gdi_font { struct nk_user_font nk; HFONT handle; } nk_gdi_font;

static float gdi_w(nk_handle h, float, const char *s, int len) {
    SIZE z={0}; HDC dc=GetDC(NULL);
    nk_gdi_font *f=(nk_gdi_font*)h.ptr; SelectObject(dc,f->handle);
    int wn=MultiByteToWideChar(CP_UTF8,0,s,len,NULL,0);
    wchar_t *wb=(wchar_t*)malloc(sizeof(wchar_t)*(wn+1));
    MultiByteToWideChar(CP_UTF8,0,s,len,wb,wn); wb[wn]=0;
    GetTextExtentPoint32W(dc,wb,wn,&z); free(wb); ReleaseDC(NULL,dc);
    return (float)z.cx+2.f;
}
static void DrawTextUTF8(HDC hdc, const char *t, int len, RECT *r, UINT fl, HFONT font) {
    if(!t||!len)return; SelectObject(hdc,font);
    int wn=MultiByteToWideChar(CP_UTF8,0,t,len,NULL,0);
    wchar_t *wb=(wchar_t*)malloc(sizeof(wchar_t)*(wn+1));
    MultiByteToWideChar(CP_UTF8,0,t,len,wb,wn); wb[wn]=0;
    DrawTextW(hdc,wb,wn,r,fl|DT_NOCLIP|DT_NOPREFIX); free(wb);
}
static nk_gdi_font make_font(int px, int w, const char *face) {
    nk_gdi_font f={0};
    f.handle=CreateFontA(-px,0,0,0,w,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,face);
    f.nk.height=(float)px; f.nk.width=gdi_w; return f;
}
static nk_gdi_font g_fXs, g_fSm, g_fMd, g_fTitle;
static void patch_fonts(){
    g_fXs.nk.userdata=nk_handle_ptr(&g_fXs); g_fSm.nk.userdata=nk_handle_ptr(&g_fSm);
    g_fMd.nk.userdata=nk_handle_ptr(&g_fMd); g_fTitle.nk.userdata=nk_handle_ptr(&g_fTitle);
}

/* ==================================================================
   HOTKEY
   A hotkey is any combination of: modifier keys + one trigger key.
   The trigger is caught via WH_KEYBOARD_LL so it sees:
     - Standard keyboard keys (VK_*)
     - Numpad keys (VK_NUMPAD0..9, VK_MULTIPLY, etc.)
     - F1..F24
     - OEM / media / browser keys
     - Mouse X buttons (VK_XBUTTON1/2) via RAWMOUSE (registered separately)
     - HID gamepad buttons mapped to VK codes by driver (e.g. XInput,
       Joy2Key, reWASD) appear as regular VK codes in the LL hook.
   The label field is filled by vk_to_str and shown in the UI.
   ================================================================== */
struct HkDef {
    int  vk;          /* 0 = unset */
    int  mod;         /* MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_WIN bitfield */
    char label[128];  /* human-readable, e.g. "Ctrl+Shift+F8" */
};

static void vk_to_str(int vk, int mod, char *out, int sz){
    if(!vk){ strncpy_s(out,sz,"",_TRUNCATE); return; }
    char b[128]={};
    if(mod&MOD_WIN)     strcat_s(b,"Win+");
    if(mod&MOD_CONTROL) strcat_s(b,"Ctrl+");
    if(mod&MOD_ALT)     strcat_s(b,"Alt+");
    if(mod&MOD_SHIFT)   strcat_s(b,"Shift+");
    struct{int v;const char*n;}named[]={
        {VK_F1,"F1"},{VK_F2,"F2"},{VK_F3,"F3"},{VK_F4,"F4"},
        {VK_F5,"F5"},{VK_F6,"F6"},{VK_F7,"F7"},{VK_F8,"F8"},
        {VK_F9,"F9"},{VK_F10,"F10"},{VK_F11,"F11"},{VK_F12,"F12"},
        {VK_F13,"F13"},{VK_F14,"F14"},{VK_F15,"F15"},{VK_F16,"F16"},
        {VK_F17,"F17"},{VK_F18,"F18"},{VK_F19,"F19"},{VK_F20,"F20"},
        {VK_F21,"F21"},{VK_F22,"F22"},{VK_F23,"F23"},{VK_F24,"F24"},
        {VK_NUMPAD0,"Num0"},{VK_NUMPAD1,"Num1"},{VK_NUMPAD2,"Num2"},
        {VK_NUMPAD3,"Num3"},{VK_NUMPAD4,"Num4"},{VK_NUMPAD5,"Num5"},
        {VK_NUMPAD6,"Num6"},{VK_NUMPAD7,"Num7"},{VK_NUMPAD8,"Num8"},
        {VK_NUMPAD9,"Num9"},{VK_MULTIPLY,"Num*"},{VK_ADD,"Num+"},
        {VK_SUBTRACT,"Num-"},{VK_DECIMAL,"Num."},{VK_DIVIDE,"Num/"},
        {VK_PRIOR,"PgUp"},{VK_NEXT,"PgDn"},{VK_HOME,"Home"},{VK_END,"End"},
        {VK_INSERT,"Ins"},{VK_DELETE,"Del"},{VK_RETURN,"Enter"},
        {VK_SPACE,"Space"},{VK_TAB,"Tab"},{VK_ESCAPE,"Esc"},
        {VK_PAUSE,"Pause"},{VK_SCROLL,"ScrollLock"},{VK_SNAPSHOT,"PrtSc"},
        {VK_LEFT,"Left"},{VK_RIGHT,"Right"},{VK_UP,"Up"},{VK_DOWN,"Down"},
        {VK_XBUTTON1,"Mouse4"},{VK_XBUTTON2,"Mouse5"},
        {VK_VOLUME_UP,"Vol+"},{VK_VOLUME_DOWN,"Vol-"},{VK_VOLUME_MUTE,"Mute"},
        {VK_MEDIA_NEXT_TRACK,"NextTrack"},{VK_MEDIA_PREV_TRACK,"PrevTrack"},
        {VK_MEDIA_PLAY_PAUSE,"PlayPause"},
        {0,NULL}
    };
    bool found=false;
    for(int i=0;named[i].n;i++) if(named[i].v==vk){strcat_s(b,named[i].n);found=true;break;}
    if(!found){
        if(vk>='A'&&vk<='Z'){char t[4]={(char)vk,0};strcat_s(b,t);}
        else if(vk>='0'&&vk<='9'){char t[4]={(char)vk,0};strcat_s(b,t);}
        else{char t[12];sprintf_s(t,"VK_%02X",vk);strcat_s(b,t);}
    }
    strncpy_s(out,sz,b,_TRUNCATE);
}

/* ==================================================================
   SETTINGS
   ================================================================== */
#define HK_RECT  0
#define HK_FULL  1
#define HK_FIXED 2
#define HK_COUNT 3

struct AppSettings {
    HkDef hk[HK_COUNT];
    int   shotFx,shotFy,shotFw,shotFh;
    char  outputDir[MAX_PATH];
    char  shotPfx[64];
    int   useTimestamp;
    int   alwaysOnTop, minimizeToTray, enableNotif;
    int   winW, winH;  /* -1 = maximized */
    int   language;
};
static AppSettings g_cfg;

static void get_exe_dir(char *b,size_t s){
    GetModuleFileNameA(NULL,b,(DWORD)s);
    for(int i=(int)strlen(b)-1;i>=0;--i)if(b[i]=='\\'||b[i]=='/'){b[i]=0;break;}
}
static string ini_path(){char d[MAX_PATH];get_exe_dir(d,sizeof(d));return string(d)+"\\makeshift_capture.ini";}
static string def_outdir(){char d[MAX_PATH];get_exe_dir(d,sizeof(d));return string(d)+"\\Captures";}

static void settings_defaults(AppSettings &s){
    memset(s.hk,0,sizeof(s.hk));
    /* Only Rectangular Capture gets a default hotkey (F8) */
    s.hk[HK_RECT].vk=VK_F8;
    vk_to_str(VK_F8,0,s.hk[HK_RECT].label,sizeof(s.hk[HK_RECT].label));
    s.shotFx=0;s.shotFy=0;s.shotFw=800;s.shotFh=600;
    strcpy_s(s.outputDir,def_outdir().c_str());
    strcpy_s(s.shotPfx,"screenshot");
    s.useTimestamp=1;s.alwaysOnTop=0;s.minimizeToTray=1;s.enableNotif=1;
    s.winW=600;s.winH=540;s.language=0;
}
static void ini_load(AppSettings &s){
    settings_defaults(s);
    FILE *f=nullptr; if(fopen_s(&f,ini_path().c_str(),"r")||!f)return;
    char line[1024],sec[64]="";
    while(fgets(line,sizeof(line),f)){
        int L=(int)strlen(line);
        while(L>0&&(line[L-1]=='\r'||line[L-1]=='\n'))line[--L]=0;
        if(!L||line[0]==';')continue;
        if(line[0]=='['){char*e=strchr(line+1,']');if(e){*e=0;strcpy_s(sec,line+1);}continue;}
        char*eq=strchr(line,'=');if(!eq)continue;
        *eq=0;const char*val=eq+1;char k[128];strcpy_s(k,line);
        int kl=(int)strlen(k);while(kl>0&&k[kl-1]==' ')k[--kl]=0;
        while(*val==' ')++val;
#define _I(sc,ky,fld) if(!strcmp(sec,sc)&&!strcmp(k,ky))s.fld=atoi(val);
#define _S(sc,ky,buf) if(!strcmp(sec,sc)&&!strcmp(k,ky))strcpy_s(s.buf,val);
        for(int i=0;i<HK_COUNT;i++){
            char kv[16],km[16];sprintf_s(kv,"hk%dvk",i);sprintf_s(km,"hk%dmod",i);
            if(!strcmp(sec,"hk")&&!strcmp(k,kv))s.hk[i].vk=atoi(val);
            if(!strcmp(sec,"hk")&&!strcmp(k,km))s.hk[i].mod=atoi(val);
        }
        _I("shot","fx",shotFx)_I("shot","fy",shotFy)_I("shot","fw",shotFw)_I("shot","fh",shotFh)
        _S("out","dir",outputDir)_S("out","sp",shotPfx)_I("out","ts",useTimestamp)
        _I("ui","top",alwaysOnTop)_I("ui","tray",minimizeToTray)
        _I("ui","notif",enableNotif)_I("ui","w",winW)_I("ui","h",winH)_I("ui","lang",language)
#undef _I
#undef _S
    }
    fclose(f);
    for(int i=0;i<HK_COUNT;i++)
        vk_to_str(s.hk[i].vk,s.hk[i].mod,s.hk[i].label,sizeof(s.hk[i].label));
}
static void ini_save(const AppSettings &s){
    FILE *f=nullptr;if(fopen_s(&f,ini_path().c_str(),"w")||!f)return;
    fprintf(f,"[hk]\n");
    for(int i=0;i<HK_COUNT;i++)fprintf(f,"hk%dvk=%d\nhk%dmod=%d\n",i,s.hk[i].vk,i,s.hk[i].mod);
    fprintf(f,"[shot]\nfx=%d\nfy=%d\nfw=%d\nfh=%d\n",s.shotFx,s.shotFy,s.shotFw,s.shotFh);
    fprintf(f,"[out]\ndir=%s\nsp=%s\nts=%d\n",s.outputDir,s.shotPfx,s.useTimestamp);
    fprintf(f,"[ui]\ntop=%d\ntray=%d\nnotif=%d\nw=%d\nh=%d\nlang=%d\n",
        s.alwaysOnTop,s.minimizeToTray,s.enableNotif,s.winW,s.winH,s.language);
    fclose(f);
}

/* ==================================================================
   HOTKEY REGISTRATION
   ================================================================== */
#define HK_BASE 0xC001
static HWND g_hwnd=NULL;

static void register_hotkeys(){
    for(int i=0;i<HK_COUNT;i++){
        UnregisterHotKey(g_hwnd,HK_BASE+i);
        if(g_cfg.hk[i].vk)
            RegisterHotKey(g_hwnd,HK_BASE+i,g_cfg.hk[i].mod,g_cfg.hk[i].vk);
    }
}

/* ==================================================================
   LOGO (embedded resource)
   ================================================================== */
static HBITMAP g_logoBmp=NULL;
static int     g_logoW=0, g_logoH=0;
struct BlitReq{bool valid;int x,y,w,h;HBITMAP hbm;int sw,sh;};
static BlitReq g_logoBlit={};

static void load_logo(){
    HRSRC res=FindResourceA(NULL,MAKEINTRESOURCEA(IDR_LOGO),"PNG");if(!res)return;
    HGLOBAL hg=LoadResource(NULL,res);if(!hg)return;
    DWORD sz=SizeofResource(NULL,res);void*data=LockResource(hg);if(!data||!sz)return;
    HGLOBAL hmem=GlobalAlloc(GMEM_MOVEABLE,sz);
    void*p=GlobalLock(hmem);memcpy(p,data,sz);GlobalUnlock(hmem);
    IStream*stream=NULL;CreateStreamOnHGlobal(hmem,TRUE,&stream);
    if(stream){
        Bitmap*bmp=Bitmap::FromStream(stream);
        if(bmp&&bmp->GetLastStatus()==Ok){
            g_logoW=(int)bmp->GetWidth();g_logoH=(int)bmp->GetHeight();
            bmp->GetHBITMAP(Color(0,0,0,0),&g_logoBmp);
        }
        delete bmp;stream->Release();
    }
}
static HICON make_icon(){
    if(g_logoBmp){
        HDC s=GetDC(NULL);HDC sdc=CreateCompatibleDC(s);HDC ddc=CreateCompatibleDC(s);
        HBITMAP d16=CreateCompatibleBitmap(s,16,16);
        SelectObject(sdc,g_logoBmp);SelectObject(ddc,d16);
        SetStretchBltMode(ddc,HALFTONE);
        StretchBlt(ddc,0,0,16,16,sdc,0,0,g_logoW,g_logoH,SRCCOPY);
        DeleteDC(sdc);DeleteDC(ddc);ReleaseDC(NULL,s);
        ICONINFO ii={TRUE,0,0,d16,d16};HICON ico=CreateIconIndirect(&ii);DeleteObject(d16);
        if(ico)return ico;
    }
    /* fallback: draw camera icon */
    HDC dc=CreateCompatibleDC(NULL);HBITMAP bm=CreateBitmap(16,16,1,32,NULL);
    HBITMAP old=(HBITMAP)SelectObject(dc,bm);
    RECT r={0,0,16,16};HBRUSH bg=CreateSolidBrush(RGB(10,9,16));FillRect(dc,&r,bg);DeleteObject(bg);
    HBRUSH ac=CreateSolidBrush(RGB(139,92,246));
    RECT body={2,5,14,12};FillRect(dc,&body,ac);
    RECT lens={5,7,10,11};HBRUSH wh=CreateSolidBrush(RGB(238,234,255));FillRect(dc,&lens,wh);DeleteObject(wh);
    RECT pupil={6,8,9,10};FillRect(dc,&pupil,ac);
    RECT hump={5,3,9,5};FillRect(dc,&hump,ac);DeleteObject(ac);
    SelectObject(dc,old);DeleteDC(dc);
    ICONINFO ii={TRUE,0,0,bm,bm};HICON ico=CreateIconIndirect(&ii);DeleteObject(bm);
    return ico?ico:LoadIcon(NULL,IDI_APPLICATION);
}

/* ==================================================================
   TRAY
   ================================================================== */
#define WM_TRAYICON  (WM_USER+1)
#define ID_TRAY_OPEN      5001
#define ID_TRAY_RECT      5002
#define ID_TRAY_FULL      5003
#define ID_TRAY_FIXED     5004
#define ID_TRAY_EXIT      5099

static NOTIFYICONDATA g_nid={};
static bool g_trayVis=false;

static void tray_add(){
    if(g_trayVis)return;
    g_nid.cbSize=sizeof(g_nid);g_nid.hWnd=g_hwnd;g_nid.uID=1;
    g_nid.uFlags=NIF_ICON|NIF_TIP|NIF_MESSAGE;g_nid.uCallbackMessage=WM_TRAYICON;
    g_nid.hIcon=make_icon();strcpy_s(g_nid.szTip,APP_NAME);
    Shell_NotifyIconA(NIM_ADD,&g_nid);g_trayVis=true;
}
static void tray_remove(){if(!g_trayVis)return;Shell_NotifyIconA(NIM_DELETE,&g_nid);g_trayVis=false;}
static void show_toast(const char*title,const char*msg){
    if(!g_cfg.enableNotif||!g_trayVis)return;
    NOTIFYICONDATA n=g_nid;n.uFlags|=NIF_INFO;n.dwInfoFlags=NIIF_INFO;n.uTimeout=3000;
    strcpy_s(n.szInfoTitle,title);strcpy_s(n.szInfo,msg);Shell_NotifyIconA(NIM_MODIFY,&n);
}
static void tray_menu(){
    HMENU m=CreatePopupMenu();
    AppendMenuA(m,MF_STRING,ID_TRAY_OPEN, T(S_TRAY_OPEN));
    AppendMenuA(m,MF_SEPARATOR,0,NULL);
    AppendMenuA(m,MF_STRING,ID_TRAY_RECT, T(S_TRAY_RECT));
    AppendMenuA(m,MF_STRING,ID_TRAY_FULL, T(S_TRAY_FULL));
    AppendMenuA(m,MF_STRING,ID_TRAY_FIXED,T(S_TRAY_FIXED));
    AppendMenuA(m,MF_SEPARATOR,0,NULL);
    AppendMenuA(m,MF_STRING,ID_TRAY_EXIT, T(S_TRAY_EXIT));
    POINT pt;GetCursorPos(&pt);SetForegroundWindow(g_hwnd);
    TrackPopupMenu(m,TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,0,g_hwnd,NULL);
    DestroyMenu(m);
}

/* ==================================================================
   WINDOW HIDE FOR SCREENSHOT
   ================================================================== */
static bool g_windowVisible=true;

static void hide_for_capture(){
    ShowWindow(g_hwnd,SW_HIDE);
    g_windowVisible=false;
    /* DwmFlush: wait for compositor to actually remove window */
    typedef HRESULT(WINAPI*PFN)();
    static PFN pfn=NULL;
    if(!pfn){HMODULE h=LoadLibraryA("dwmapi.dll");if(h)pfn=(PFN)GetProcAddress(h,"DwmFlush");}
    if(pfn)pfn();
    Sleep(200);  /* extra margin for slow/integrated GPUs */
}
static void show_after_capture(){
    ShowWindow(g_hwnd,SW_SHOWNOACTIVATE);
    g_windowVisible=true;
}

/* ==================================================================
   RECT SELECTION OVERLAY
   ================================================================== */
struct SelState{POINT a,b;bool drag,done,cancelled;};
static SelState g_sel;

static LRESULT CALLBACK SelProc(HWND hw,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
    case WM_PAINT:{
        PAINTSTRUCT ps;HDC hdc=BeginPaint(hw,&ps);
        RECT rc;GetClientRect(hw,&rc);
        HBRUSH ov=CreateSolidBrush(RGB(8,6,14));FillRect(hdc,&rc,ov);DeleteObject(ov);
        if(g_sel.drag){
            int x1=min(g_sel.a.x,g_sel.b.x),y1=min(g_sel.a.y,g_sel.b.y);
            int x2=max(g_sel.a.x,g_sel.b.x),y2=max(g_sel.a.y,g_sel.b.y);
            RECT sr={x1,y1,x2,y2};
            HBRUSH cl=CreateSolidBrush(RGB(55,35,100));FillRect(hdc,&sr,cl);DeleteObject(cl);
            HPEN pn=CreatePen(PS_SOLID,2,RGB(139,92,246));
            SelectObject(hdc,pn);SelectObject(hdc,GetStockObject(NULL_BRUSH));
            Rectangle(hdc,x1,y1,x2,y2);DeleteObject(pn);
            char lb[64];sprintf_s(lb,"%dx%d  (%d,%d)",x2-x1,y2-y1,x1,y1);
            HFONT fn=CreateFontA(-14,0,0,0,FW_SEMIBOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Segoe UI");
            SelectObject(hdc,fn);SetTextColor(hdc,RGB(200,170,255));SetBkMode(hdc,TRANSPARENT);
            RECT lr={x1+6,y1+4,x1+300,y1+22};
            DrawTextA(hdc,lb,-1,&lr,DT_LEFT|DT_TOP|DT_SINGLELINE);DeleteObject(fn);
        }
        HFONT fn=CreateFontA(-15,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Segoe UI");
        SelectObject(hdc,fn);SetTextColor(hdc,RGB(180,150,240));SetBkMode(hdc,TRANSPARENT);
        RECT ir={0,0,rc.right,36};
        DrawTextA(hdc,T(S_SEL_DRAG),-1,&ir,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        DeleteObject(fn);EndPaint(hw,&ps);return 0;}
    case WM_LBUTTONDOWN:
        g_sel.a={GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
        g_sel.b=g_sel.a;g_sel.drag=true;g_sel.done=false;g_sel.cancelled=false;
        SetCapture(hw);return 0;
    case WM_MOUSEMOVE:
        if(g_sel.drag){g_sel.b={GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};InvalidateRect(hw,NULL,TRUE);}return 0;
    case WM_LBUTTONUP:
        if(g_sel.drag){
            g_sel.b={GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
            ReleaseCapture();g_sel.drag=false;g_sel.done=true;DestroyWindow(hw);
        }return 0;
    case WM_KEYDOWN:
        if(wp==VK_ESCAPE){g_sel.cancelled=true;g_sel.done=false;DestroyWindow(hw);}return 0;
    case WM_DESTROY:PostQuitMessage(0);return 0;
    }
    return DefWindowProc(hw,msg,wp,lp);
}
static bool rect_select(RECT &out){
    WNDCLASSA wc={0};wc.lpfnWndProc=SelProc;wc.hInstance=GetModuleHandle(NULL);
    wc.hCursor=LoadCursor(NULL,IDC_CROSS);wc.lpszClassName="MCT_Sel";RegisterClassA(&wc);
    g_sel={};
    int sw=GetSystemMetrics(SM_CXVIRTUALSCREEN),sh=GetSystemMetrics(SM_CYVIRTUALSCREEN);
    int sx=GetSystemMetrics(SM_XVIRTUALSCREEN),sy=GetSystemMetrics(SM_YVIRTUALSCREEN);
    HWND sel=CreateWindowExA(WS_EX_TOPMOST|WS_EX_LAYERED|WS_EX_TOOLWINDOW,
        "MCT_Sel",NULL,WS_POPUP|WS_VISIBLE,sx,sy,sw,sh,NULL,NULL,GetModuleHandle(NULL),NULL);
    if(!sel)return false;
    SetLayeredWindowAttributes(sel,0,170,LWA_ALPHA);SetForegroundWindow(sel);
    MSG msg;while(GetMessage(&msg,NULL,0,0)){TranslateMessage(&msg);DispatchMessage(&msg);}
    if(!g_sel.done)return false;
    out.left=min(g_sel.a.x,g_sel.b.x);out.top=min(g_sel.a.y,g_sel.b.y);
    out.right=max(g_sel.a.x,g_sel.b.x);out.bottom=max(g_sel.a.y,g_sel.b.y);
    return (out.right-out.left)>=8&&(out.bottom-out.top)>=8;
}

/* ==================================================================
   CAPTURE HELPERS
   ================================================================== */
static RECT fullscreen_rect(){
    RECT r;
    r.left=GetSystemMetrics(SM_XVIRTUALSCREEN);r.top=GetSystemMetrics(SM_YVIRTUALSCREEN);
    r.right=r.left+GetSystemMetrics(SM_CXVIRTUALSCREEN);
    r.bottom=r.top+GetSystemMetrics(SM_CYVIRTUALSCREEN);return r;
}
static HBITMAP capture_hbm(RECT r){
    int w=r.right-r.left,h=r.bottom-r.top;if(w<=0||h<=0)return NULL;
    HDC scr=GetDC(NULL);HDC mem=CreateCompatibleDC(scr);
    HBITMAP bm=CreateCompatibleBitmap(scr,w,h);
    HBITMAP old=(HBITMAP)SelectObject(mem,bm);
    BitBlt(mem,0,0,w,h,scr,r.left,r.top,SRCCOPY|CAPTUREBLT);
    SelectObject(mem,old);DeleteDC(mem);ReleaseDC(NULL,scr);return bm;
}
static CLSID s_pngClsid;static bool s_pngSet=false;
static CLSID get_png_clsid(){
    if(s_pngSet)return s_pngClsid;
    UINT n=0,sz=0;GetImageEncodersSize(&n,&sz);
    std::vector<BYTE>buf(sz);ImageCodecInfo*ci=(ImageCodecInfo*)buf.data();GetImageEncoders(n,sz,ci);
    for(UINT i=0;i<n;i++)if(!wcscmp(ci[i].MimeType,L"image/png")){s_pngClsid=ci[i].Clsid;s_pngSet=true;break;}
    return s_pngClsid;
}
static bool save_png(HBITMAP hbm,const char*path){
    Bitmap bmp(hbm,NULL);CLSID c=get_png_clsid();
    int wn=MultiByteToWideChar(CP_UTF8,0,path,-1,NULL,0);
    std::vector<wchar_t>wp(wn);MultiByteToWideChar(CP_UTF8,0,path,-1,wp.data(),wn);
    return bmp.Save(wp.data(),&c,NULL)==Ok;
}
static string ts_str(){
    SYSTEMTIME t;GetLocalTime(&t);char b[32];
    sprintf_s(b,"%04d%02d%02d_%02d%02d%02d",t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond);
    return b;
}
static string build_path(const char*dir,const char*pfx,const char*ext){
    try{bfs::create_directories(dir);}catch(...){}
    char b[MAX_PATH];
    if(g_cfg.useTimestamp)sprintf_s(b,"%s\\%s_%s.%s",dir,pfx,ts_str().c_str(),ext);
    else for(int n=1;n<99999;n++){sprintf_s(b,"%s\\%s_%04d.%s",dir,pfx,n,ext);if(!bfs::exists(b))break;}
    return b;
}

/* status */
static char g_statusMsg[256]="";static char g_statusCol[8]="muted";
static void set_status(const char*m,const char*c){strcpy_s(g_statusMsg,m);strcpy_s(g_statusCol,c);}
static struct nk_color status_col(const char*c){
    if(!strcmp(c,"green"))return C_GREEN;if(!strcmp(c,"red"))return C_RED;
    if(!strcmp(c,"amber"))return C_AMBER;if(!strcmp(c,"accent"))return C_PUR_L;
    return C_MUTED;
}

/* mode: 0=rect  1=full  2=fixed
   fromTray: don't restore window after capture */
static void do_screenshot(int mode,bool fromTray){
    hide_for_capture();
    RECT r={};bool ok=false;
    switch(mode){
    case 0: ok=rect_select(r); break;
    case 1: r=fullscreen_rect();ok=true; break;
    case 2: r={g_cfg.shotFx,g_cfg.shotFy,
               g_cfg.shotFx+max(1,g_cfg.shotFw),
               g_cfg.shotFy+max(1,g_cfg.shotFh)};ok=true; break;
    }
    if(!ok){
        set_status(T(S_CANCELLED),"muted");
        if(!fromTray)show_after_capture();
        return;
    }
    string path=build_path(g_cfg.outputDir,g_cfg.shotPfx,"png");
    HBITMAP bm=capture_hbm(r);
    bool saved=bm&&save_png(bm,path.c_str());
    if(bm)DeleteObject(bm);
    if(!fromTray)show_after_capture();
    if(saved){
        bfs::path p(path);char b[300];
        sprintf_s(b,"%s%s",T(S_SHOT_SAVED),p.filename().string().c_str());
        set_status(b,"green");show_toast(APP_NAME,b);
    }else set_status(T(S_SHOT_FAIL),"red");
}

/* ==================================================================
   HOTKEY CAPTURE (low-level hook)
   WH_KEYBOARD_LL catches ALL keys: keyboard, numpad, F13-F24, media,
   and VK codes emitted by gamepad/HID drivers (XInput, Joy2Key, etc.)
   Mouse X buttons are caught via Raw Input.
   ================================================================== */
static int   g_listenIdx=-1;
static HHOOK g_listenHook=NULL;

static LRESULT CALLBACK HkHook(int code,WPARAM wp,LPARAM lp){
    if(code>=0&&wp==WM_KEYDOWN){
        KBDLLHOOKSTRUCT*kb=(KBDLLHOOKSTRUCT*)lp;
        int vk=(int)kb->vkCode;
        /* skip bare modifiers */
        if(vk!=VK_SHIFT&&vk!=VK_LSHIFT&&vk!=VK_RSHIFT&&
           vk!=VK_CONTROL&&vk!=VK_LCONTROL&&vk!=VK_RCONTROL&&
           vk!=VK_MENU&&vk!=VK_LMENU&&vk!=VK_RMENU&&
           vk!=VK_LWIN&&vk!=VK_RWIN){
            int mod=0;
            if(GetAsyncKeyState(VK_CONTROL)&0x8000)mod|=MOD_CONTROL;
            if(GetAsyncKeyState(VK_MENU)   &0x8000)mod|=MOD_ALT;
            if(GetAsyncKeyState(VK_SHIFT)  &0x8000)mod|=MOD_SHIFT;
            if(GetAsyncKeyState(VK_LWIN)   &0x8000||
               GetAsyncKeyState(VK_RWIN)   &0x8000)mod|=MOD_WIN;
            UnhookWindowsHookEx(g_listenHook);g_listenHook=NULL;
            PostMessageA(g_hwnd,WM_USER+10,(WPARAM)vk,(LPARAM)mod);
            return 1;
        }
    }
    return CallNextHookEx(g_listenHook,code,wp,lp);
}
static void hk_listen_start(int idx){
    if(g_listenHook){UnhookWindowsHookEx(g_listenHook);g_listenHook=NULL;}
    g_listenIdx=idx;
    g_listenHook=SetWindowsHookExA(WH_KEYBOARD_LL,HkHook,GetModuleHandle(NULL),0);
}
static void hk_listen_cancel(){
    if(g_listenHook){UnhookWindowsHookEx(g_listenHook);g_listenHook=NULL;}
    g_listenIdx=-1;
}
static void hk_apply(int idx,int vk,int mod){
    g_cfg.hk[idx].vk=vk;g_cfg.hk[idx].mod=mod;
    vk_to_str(vk,mod,g_cfg.hk[idx].label,sizeof(g_cfg.hk[idx].label));
    register_hotkeys();g_listenIdx=-1;
}

/* ==================================================================
   NUKLEAR THEME
   ================================================================== */
static struct nk_context g_nk;
static void apply_theme(){
    struct nk_color t[NK_COLOR_COUNT];
    for(int i=0;i<NK_COLOR_COUNT;i++)t[i]=nk_rgba(26,23,40,255);
    t[NK_COLOR_WINDOW]=C_BG;t[NK_COLOR_HEADER]=C_SURF;t[NK_COLOR_BORDER]=C_BORDER;
    t[NK_COLOR_TEXT]=C_TEXT;t[NK_COLOR_BUTTON]=C_CARD;t[NK_COLOR_BUTTON_HOVER]=C_CARD2;
    t[NK_COLOR_BUTTON_ACTIVE]=C_PUR_D;t[NK_COLOR_TOGGLE]=C_CARD;t[NK_COLOR_TOGGLE_HOVER]=C_CARD2;
    t[NK_COLOR_TOGGLE_CURSOR]=C_PUR;t[NK_COLOR_SELECT]=C_CARD;t[NK_COLOR_SELECT_ACTIVE]=C_PUR_D;
    t[NK_COLOR_SLIDER]=C_CARD;t[NK_COLOR_SLIDER_CURSOR]=C_PUR;
    t[NK_COLOR_SLIDER_CURSOR_HOVER]=C_PUR_L;t[NK_COLOR_SLIDER_CURSOR_ACTIVE]=nk_rgb(109,40,217);
    t[NK_COLOR_PROPERTY]=C_SURF;t[NK_COLOR_EDIT]=nk_rgb(14,12,22);t[NK_COLOR_EDIT_CURSOR]=C_PUR_L;
    t[NK_COLOR_COMBO]=C_CARD;t[NK_COLOR_CHART]=C_SURF;t[NK_COLOR_CHART_COLOR]=C_PUR;
    t[NK_COLOR_CHART_COLOR_HIGHLIGHT]=C_RED;t[NK_COLOR_SCROLLBAR]=C_SURF;
    t[NK_COLOR_SCROLLBAR_CURSOR]=C_BORDER;t[NK_COLOR_SCROLLBAR_CURSOR_HOVER]=C_PUR_D;
    t[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE]=C_PUR;t[NK_COLOR_TAB_HEADER]=C_SURF;
    nk_style_from_table(&g_nk,t);
    g_nk.style.window.padding=nk_vec2(12,8);g_nk.style.window.spacing=nk_vec2(5,4);
    g_nk.style.button.rounding=4;g_nk.style.button.border=1.f;
    g_nk.style.button.border_color=C_BORDER;g_nk.style.button.padding=nk_vec2(6,4);
    g_nk.style.edit.border=1.f;g_nk.style.edit.border_color=C_BORDER;
    g_nk.style.edit.rounding=4;g_nk.style.edit.padding=nk_vec2(6,3);g_nk.style.edit.cursor_size=2;
    g_nk.style.checkbox.border=1;g_nk.style.checkbox.border_color=C_BORDER;
    g_nk.style.checkbox.padding=nk_vec2(2,2);
}

/* ==================================================================
   UI HELPERS
   ================================================================== */
static void section_label(const char*txt){
    nk_layout_row_dynamic(&g_nk,5,1);nk_spacing(&g_nk,1);
    nk_layout_row_dynamic(&g_nk,16,1);
    nk_style_push_font(&g_nk,&g_fXs.nk);
    g_nk.style.text.color=C_PUR_L;nk_label(&g_nk,txt,NK_TEXT_LEFT);
    g_nk.style.text.color=C_TEXT;nk_style_pop_font(&g_nk);
    nk_layout_row_dynamic(&g_nk,1,1);
    {struct nk_rect r;nk_widget(&r,&g_nk);nk_fill_rect(nk_window_get_canvas(&g_nk),r,0,C_BORDER);}
    nk_layout_row_dynamic(&g_nk,4,1);nk_spacing(&g_nk,1);
}
static bool action_btn(const char*lbl){
    struct nk_style_button sb=g_nk.style.button;
    g_nk.style.button.normal.data.color=C_PUR_D;
    g_nk.style.button.hover.data.color=nk_rgb(90,55,160);
    g_nk.style.button.border_color=C_PUR;g_nk.style.text.color=C_TEXT;
    nk_style_push_font(&g_nk,&g_fMd.nk);
    bool r=nk_button_label(&g_nk,lbl)!=0;
    nk_style_pop_font(&g_nk);g_nk.style.button=sb;g_nk.style.text.color=C_TEXT;return r;
}
static void row_str(const char*lbl,float lw,char*buf,int sz){
    float ww=nk_window_get_width(&g_nk)-24.f;
    nk_layout_row_begin(&g_nk,NK_STATIC,26,2);
    nk_layout_row_push(&g_nk,lw);nk_style_push_font(&g_nk,&g_fXs.nk);
    g_nk.style.text.color=C_MUTED;nk_label(&g_nk,lbl,NK_TEXT_LEFT);g_nk.style.text.color=C_TEXT;
    nk_layout_row_push(&g_nk,ww-lw-4);
    nk_edit_string_zero_terminated(&g_nk,NK_EDIT_FIELD,buf,sz,nk_filter_default);
    nk_style_pop_font(&g_nk);nk_layout_row_end(&g_nk);
}
static void row_xywh(char*bx,char*by,char*bw,char*bh){
    float ww=nk_window_get_width(&g_nk)-24.f;float fw=(ww-4*22)/4.f;
    nk_layout_row_begin(&g_nk,NK_STATIC,26,8);nk_style_push_font(&g_nk,&g_fXs.nk);
    const char*lb[]={"X","Y","W","H"};char*bu[]={bx,by,bw,bh};
    for(int i=0;i<4;i++){
        nk_layout_row_push(&g_nk,22);g_nk.style.text.color=C_MUTED;
        nk_label(&g_nk,lb[i],NK_TEXT_CENTERED);g_nk.style.text.color=C_TEXT;
        nk_layout_row_push(&g_nk,fw);
        nk_edit_string_zero_terminated(&g_nk,NK_EDIT_FIELD,bu[i],10,nk_filter_decimal);
    }
    nk_style_pop_font(&g_nk);nk_layout_row_end(&g_nk);
}
static void save_btn(){
    nk_layout_row_dynamic(&g_nk,10,1);nk_spacing(&g_nk,1);
    nk_layout_row_dynamic(&g_nk,34,1);
    struct nk_style_button sb=g_nk.style.button;
    g_nk.style.button.normal.data.color=C_PUR_D;
    g_nk.style.button.hover.data.color=nk_rgb(90,55,160);
    g_nk.style.button.border_color=C_PUR;nk_style_push_font(&g_nk,&g_fSm.nk);
    if(nk_button_label(&g_nk,T(S_SET_SAVE))){
        ini_save(g_cfg);register_hotkeys();
        SetWindowPos(g_hwnd,g_cfg.alwaysOnTop?HWND_TOPMOST:HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
        set_status(T(S_READY),"green");
    }
    nk_style_pop_font(&g_nk);g_nk.style.button=sb;
}

/* edit buffers */
static char eb_fx[12],eb_fy[12],eb_fw[12],eb_fh[12];
static char eb_dir[MAX_PATH],eb_pfx[64];
static void sync_edits(){
    sprintf_s(eb_fx,"%d",g_cfg.shotFx);sprintf_s(eb_fy,"%d",g_cfg.shotFy);
    sprintf_s(eb_fw,"%d",g_cfg.shotFw);sprintf_s(eb_fh,"%d",g_cfg.shotFh);
    strcpy_s(eb_dir,g_cfg.outputDir);strcpy_s(eb_pfx,g_cfg.shotPfx);
}
static void flush_edits(){
    g_cfg.shotFx=max(0,atoi(eb_fx));g_cfg.shotFy=max(0,atoi(eb_fy));
    g_cfg.shotFw=max(1,atoi(eb_fw));g_cfg.shotFh=max(1,atoi(eb_fh));
    strcpy_s(g_cfg.outputDir,eb_dir);strcpy_s(g_cfg.shotPfx,eb_pfx);
}

/* ==================================================================
   TABS
   ================================================================== */
enum TabID{T_CAPTURE=0,T_SHOT,T_SETTINGS,T_ABOUT,T_COUNT};
static int g_tab=T_CAPTURE;
static struct nk_scroll g_tabScroll[T_COUNT]={};

/* ----  Capture tab  ---- */
static void tab_capture(){
    float ww=nk_window_get_width(&g_nk)-24.f;
    section_label(T(S_TAB_SHOT));

    /* 3 mode buttons – each full width, stacked vertically like GitHub button in About tab */
    nk_layout_row_dynamic(&g_nk,36,1);
    if(action_btn(T(S_MODE_RECT))) do_screenshot(0,false);
    nk_layout_row_dynamic(&g_nk,36,1);
    if(action_btn(T(S_MODE_FULL))) do_screenshot(1,false);
    nk_layout_row_dynamic(&g_nk,36,1);
    if(action_btn(T(S_MODE_FIXED)))do_screenshot(2,false);

    /* Status */
    section_label(T(S_STATUS));
    struct nk_color dc=status_col(g_statusCol);
    nk_layout_row_begin(&g_nk,NK_STATIC,22,2);
    nk_layout_row_push(&g_nk,14);
    {struct nk_rect dr;nk_widget(&dr,&g_nk);
     nk_fill_circle(nk_window_get_canvas(&g_nk),nk_rect(dr.x,dr.y+5,10,10),dc);}
    nk_layout_row_push(&g_nk,ww-18);
    nk_style_push_font(&g_nk,&g_fXs.nk);g_nk.style.text.color=dc;
    nk_label(&g_nk,g_statusMsg,NK_TEXT_LEFT);g_nk.style.text.color=C_TEXT;nk_style_pop_font(&g_nk);
    nk_layout_row_end(&g_nk);

    /* Open folder */
    nk_layout_row_dynamic(&g_nk,8,1);nk_spacing(&g_nk,1);
    nk_layout_row_dynamic(&g_nk,28,1);
    {struct nk_style_button sb=g_nk.style.button;
     g_nk.style.button.normal.data.color=C_CARD;g_nk.style.button.border_color=C_BORDER;
     g_nk.style.text.color=C_MUTED;nk_style_push_font(&g_nk,&g_fXs.nk);
     if(nk_button_label(&g_nk,T(S_OPEN_FOLDER))){
         try{bfs::create_directories(g_cfg.outputDir);}catch(...){}
         ShellExecuteA(NULL,"open",g_cfg.outputDir,NULL,NULL,SW_SHOWNORMAL);}
     nk_style_pop_font(&g_nk);g_nk.style.button=sb;g_nk.style.text.color=C_TEXT;}
}

/* Returns pixel width of a UTF-8 string rendered in g_fXs (13px Segoe UI) */
static float tw_xs(const char*s){
    if(!s||!*s)return 0.f;
    return g_fXs.nk.width(g_fXs.nk.userdata,13.f,s,(int)strlen(s))+2.f;
}

/* ----  One hotkey block  ----
   Vertical layout matching the GitHub button style - full width rows, nothing crops:
     Row 1 (16px): mode name label (muted, left)
     Row 2 (28px): current key value (purple/dim, left)   [full width]
     Row 3 (28px): [Set key... / Cancel]                  [full width]
     Row 4 (24px, only if set): [Clear]                   [full width, muted]
     Gap
*/
static void hk_row(int idx, const char *seclabel){
    bool listening=(g_listenIdx==idx);

    /* Row 1: mode name */
    nk_layout_row_dynamic(&g_nk,16,1);
    nk_style_push_font(&g_nk,&g_fXs.nk);
    g_nk.style.text.color=C_MUTED;
    nk_label(&g_nk,seclabel,NK_TEXT_LEFT);
    g_nk.style.text.color=C_TEXT;
    nk_style_pop_font(&g_nk);

    /* Row 2: current key display */
    nk_layout_row_dynamic(&g_nk,26,1);
    nk_style_push_font(&g_nk,&g_fXs.nk);
    if(listening){
        g_nk.style.text.color=C_AMBER;
        nk_label(&g_nk,T(S_HK_PRESS_TO_SET),NK_TEXT_LEFT);
    }else{
        g_nk.style.text.color=g_cfg.hk[idx].vk?C_PUR_L:C_DIM;
        nk_label(&g_nk,g_cfg.hk[idx].vk?g_cfg.hk[idx].label:T(S_HK_NONE),NK_TEXT_LEFT);
    }
    g_nk.style.text.color=C_TEXT;
    nk_style_pop_font(&g_nk);

    /* Row 3: Set / Cancel button (full width) */
    nk_layout_row_dynamic(&g_nk,28,1);
    if(listening){
        struct nk_style_button sb=g_nk.style.button;
        g_nk.style.button.normal.data.color=nk_rgb(55,20,20);
        g_nk.style.button.border_color=C_RED;
        g_nk.style.text.color=C_RED;
        nk_style_push_font(&g_nk,&g_fXs.nk);
        if(nk_button_label(&g_nk,"Cancel"))hk_listen_cancel();
        nk_style_pop_font(&g_nk);
        g_nk.style.button=sb;g_nk.style.text.color=C_TEXT;
    }else{
        struct nk_style_button sb=g_nk.style.button;
        g_nk.style.button.normal.data.color=C_CARD2;
        g_nk.style.button.border_color=C_BORDER;
        g_nk.style.text.color=C_MUTED;
        nk_style_push_font(&g_nk,&g_fXs.nk);
        if(nk_button_label(&g_nk,"Set key..."))hk_listen_start(idx);
        nk_style_pop_font(&g_nk);
        g_nk.style.button=sb;g_nk.style.text.color=C_TEXT;
    }

    /* Row 4: Clear (only if key assigned) */
    if(!listening&&g_cfg.hk[idx].vk){
        nk_layout_row_dynamic(&g_nk,22,1);
        struct nk_style_button sb=g_nk.style.button;
        g_nk.style.button.normal.data.color=C_CARD;
        g_nk.style.button.border_color=C_BORDER;
        g_nk.style.text.color=C_DIM;
        nk_style_push_font(&g_nk,&g_fXs.nk);
        if(nk_button_label(&g_nk,T(S_HK_CLEAR))){
            g_cfg.hk[idx].vk=0;g_cfg.hk[idx].mod=0;
            g_cfg.hk[idx].label[0]=0;register_hotkeys();}
        nk_style_pop_font(&g_nk);
        g_nk.style.button=sb;g_nk.style.text.color=C_TEXT;
    }

    /* gap */
    nk_layout_row_dynamic(&g_nk,8,1);nk_spacing(&g_nk,1);
}

/* ----  Screenshot settings tab  ---- */
static void tab_shot(){
    hk_row(HK_RECT, T(S_HK_LABEL_RECT));
    hk_row(HK_FULL, T(S_HK_LABEL_FULL));
    hk_row(HK_FIXED,T(S_HK_LABEL_FIXED));

    section_label(T(S_FIXED_XYWH));
    row_xywh(eb_fx,eb_fy,eb_fw,eb_fh);

    section_label(T(S_SET_SHOT_PFX));
    row_str(T(S_SET_SHOT_PFX),120,eb_pfx,64);
    save_btn();
}

/* ----  Settings tab  ---- */
static void tab_settings(){
    float ww=nk_window_get_width(&g_nk)-24.f;

    section_label(T(S_SET_OUTDIR));
    row_str("",4,eb_dir,MAX_PATH);
    nk_style_push_font(&g_nk,&g_fXs.nk);g_nk.style.text.color=C_DIM;
    nk_layout_row_dynamic(&g_nk,16,1);
    nk_label(&g_nk,"  Default: Captures\\ next to .exe",NK_TEXT_LEFT);
    g_nk.style.text.color=C_TEXT;nk_style_pop_font(&g_nk);

    section_label(T(S_SET_LANG));
    nk_layout_row_dynamic(&g_nk,26,1);nk_style_push_font(&g_nk,&g_fXs.nk);
    const char*lnames[LANG_COUNT];
    for(int i=0;i<LANG_COUNT;i++)lnames[i]=g_langs[i].displayName;
    int nl=nk_combo(&g_nk,lnames,LANG_COUNT,g_cfg.language,22,nk_vec2(200,137));
    if(nl!=g_cfg.language){g_cfg.language=nl;g_lang=nl;}
    nk_style_pop_font(&g_nk);

    section_label("DISPLAY");
    /* Each checkbox on its own row, left-aligned, fixed checkbox+label column */
    nk_style_push_font(&g_nk,&g_fXs.nk);
    nk_layout_row_begin(&g_nk,NK_STATIC,26,1);
    nk_layout_row_push(&g_nk,ww);nk_checkbox_label(&g_nk,T(S_SET_TOP),  &g_cfg.alwaysOnTop);
    nk_layout_row_end(&g_nk);
    nk_layout_row_begin(&g_nk,NK_STATIC,26,1);
    nk_layout_row_push(&g_nk,ww);nk_checkbox_label(&g_nk,T(S_SET_TRAY), &g_cfg.minimizeToTray);
    nk_layout_row_end(&g_nk);
    nk_layout_row_begin(&g_nk,NK_STATIC,26,1);
    nk_layout_row_push(&g_nk,ww);nk_checkbox_label(&g_nk,T(S_SET_NOTIF),    &g_cfg.enableNotif);
    nk_layout_row_end(&g_nk);
    nk_layout_row_begin(&g_nk,NK_STATIC,26,1);
    nk_layout_row_push(&g_nk,ww);nk_checkbox_label(&g_nk,T(S_SET_TIMESTAMP),&g_cfg.useTimestamp);
    nk_layout_row_end(&g_nk);
    nk_style_pop_font(&g_nk);
    save_btn();
}

/* ==================================================================
   ABOUT TAB
   Hero card contains:
     Line 1: app name (title font)
     Line 2: description (muted)
     Line 3: "Made by AUTHOR using TOOL and TOOL" with inline hyperlinks
             Plain words drawn with nk_draw_text; link words backed by
             invisible zero-border buttons exactly overlapping the text.
   Licenses: two separate auto-height scrollable sections:
     "Third-party Licenses"  and  "License"
   ================================================================== */

static struct nk_scroll g_tp_scroll={0,0};
static struct nk_scroll g_ap_scroll={0,0};

static void tab_about(){
    float ww=nk_window_get_width(&g_nk)-24.f;

    /* ---- measure "Made by ..." segments ---- */
    const char *mb=T(S_ABOUT_MADEBY),*us=T(S_ABOUT_USING),*an=T(S_ABOUT_AND);
    const char *au="Maxim Bortnikov",*t1="Claude Sonnet 4.6",*t2="Perplexity";
    float wMB=tw_xs(mb),wAU=tw_xs(au),wUS=tw_xs(us);
    float wT1=tw_xs(t1),wAN=tw_xs(an),wT2=tw_xs(t2);
    /* total width with 4px gaps between each segment */
    float creditW=wMB+4+wAU+4+wUS+4+wT1+4+wAN+4+wT2;
    /* hero card height: title(22) + gap(4) + desc(16) + gap(4) + credits(16) + padding(14) */
    float cardH=22+4+16+4+16+14.f;

    /* ---- Hero card ---- */
    nk_layout_row_dynamic(&g_nk,(int)cardH,1);
    struct nk_rect b;nk_widget(&b,&g_nk);
    struct nk_command_buffer*cb=nk_window_get_canvas(&g_nk);
    nk_fill_rect(cb,b,8,C_CARD);nk_stroke_rect(cb,b,8,1,C_BORDER);
    nk_fill_rect(cb,nk_rect(b.x+1,b.y+1,5,b.h-2),4,C_PUR);
    /* App name */
    nk_draw_text(cb,nk_rect(b.x+16,b.y+8,b.w-20,22),
        APP_NAME,(int)strlen(APP_NAME),&g_fTitle.nk,C_CARD,C_TEXT);
    /* Description */
    nk_draw_text(cb,nk_rect(b.x+16,b.y+34,b.w-20,16),
        T(S_ABOUT_DESC),(int)strlen(T(S_ABOUT_DESC)),&g_fXs.nk,C_CARD,C_MUTED);
    /* "Made by ..." drawn inline – x auto-centered or left-aligned */
    float cx=b.x+16.f;
    float cy=b.y+cardH-20.f;  /* bottom of card, 20px up */
    /* Draw plain words */
    nk_draw_text(cb,nk_rect(cx,cy,wMB,16),mb,(int)strlen(mb),&g_fXs.nk,C_CARD,C_MUTED);cx+=wMB+4;
    nk_draw_text(cb,nk_rect(cx,cy,wAU,16),au,(int)strlen(au),&g_fXs.nk,C_CARD,C_PUR_L);cx+=wAU+4;
    nk_draw_text(cb,nk_rect(cx,cy,wUS,16),us,(int)strlen(us),&g_fXs.nk,C_CARD,C_MUTED);cx+=wUS+4;
    nk_draw_text(cb,nk_rect(cx,cy,wT1,16),t1,(int)strlen(t1),&g_fXs.nk,C_CARD,C_PUR_L);cx+=wT1+4;
    nk_draw_text(cb,nk_rect(cx,cy,wAN,16),an,(int)strlen(an),&g_fXs.nk,C_CARD,C_MUTED);cx+=wAN+4;
    nk_draw_text(cb,nk_rect(cx,cy,wT2,16),t2,(int)strlen(t2),&g_fXs.nk,C_CARD,C_PUR_L);
    /* Hover underlines */
    auto check_hover=[&](float lx,float ly,float lw,float lh)->bool{
        POINT pt;GetCursorPos(&pt);
        POINT cl=pt;ScreenToClient(g_hwnd,&cl);
        return (cl.x>=lx&&cl.x<=lx+lw&&cl.y>=ly&&cl.y<=ly+lh);
    };
    float ux=b.x+16+wMB+4; /* start of author */
    if(check_hover(ux,cy,wAU,16))
        nk_stroke_line(cb,ux,cy+15.f,ux+wAU,cy+15.f,1.f,C_PUR_L);
    ux+=wAU+4+wUS+4;        /* Claude */
    if(check_hover(ux,cy,wT1,16))
        nk_stroke_line(cb,ux,cy+15.f,ux+wT1,cy+15.f,1.f,C_PUR_L);
    ux+=wT1+4+wAN+4;        /* Perplexity */
    if(check_hover(ux,cy,wT2,16))
        nk_stroke_line(cb,ux,cy+15.f,ux+wT2,cy+15.f,1.f,C_PUR_L);

    /* ---- Invisible clickable hit areas overlapping the hero card credit line ----
       We use nk_layout_space to position 3 transparent buttons at pixel-exact
       positions matching where the drawn text is inside the card.
       The card ended at "b.y + cardH". The credit line is at cy = b.y+cardH-20.
       We emit a 16px-tall layout_space row directly after the card widget.
       Since the card was consumed by nk_widget, the next row starts right below.
       We reach UP by using a negative y offset inside nk_layout_space.
    */
    nk_layout_space_begin(&g_nk,NK_STATIC,16,3);
    {
        /* Positions relative to the space origin (which starts at b.y+cardH).
           The credit line is 20px above the bottom of the card, so
           dy = -20 relative to the bottom = -(cardH - (cardH-20)) = -20 */
        float dy=-20.f; /* move up into the card */
        float lx0=16+wMB+4;                          /* author start x (relative to card left=b.x) */
        float lx1=lx0+wAU+4+wUS+4;                   /* Claude start */
        float lx2=lx1+wT1+4+wAN+4;                   /* Perplexity start */
        /* nk_layout_space coords are relative to group content origin */

        nk_layout_space_push(&g_nk,nk_rect(lx0, dy, wAU, 16));
        {struct nk_style_button sbb=g_nk.style.button;
         g_nk.style.button.normal.data.color=nk_rgba(0,0,0,0);
         g_nk.style.button.hover.data.color=nk_rgba(0,0,0,0);
         g_nk.style.button.active.data.color=nk_rgba(0,0,0,0);
         g_nk.style.button.border=0;g_nk.style.button.padding=nk_vec2(0,0);
         g_nk.style.text.color=nk_rgba(0,0,0,0);
         nk_style_push_font(&g_nk,&g_fXs.nk);
         if(nk_button_label(&g_nk," "))
             ShellExecuteA(NULL,"open","https://maxim-bortnikov.netlify.app/",NULL,NULL,SW_SHOWNORMAL);
         nk_style_pop_font(&g_nk);g_nk.style.button=sbb;g_nk.style.text.color=C_TEXT;}

        nk_layout_space_push(&g_nk,nk_rect(lx1, dy, wT1, 16));
        {struct nk_style_button sbb=g_nk.style.button;
         g_nk.style.button.normal.data.color=nk_rgba(0,0,0,0);
         g_nk.style.button.hover.data.color=nk_rgba(0,0,0,0);
         g_nk.style.button.active.data.color=nk_rgba(0,0,0,0);
         g_nk.style.button.border=0;g_nk.style.button.padding=nk_vec2(0,0);
         g_nk.style.text.color=nk_rgba(0,0,0,0);
         nk_style_push_font(&g_nk,&g_fXs.nk);
         if(nk_button_label(&g_nk," "))
             ShellExecuteA(NULL,"open","https://claude.ai",NULL,NULL,SW_SHOWNORMAL);
         nk_style_pop_font(&g_nk);g_nk.style.button=sbb;g_nk.style.text.color=C_TEXT;}

        nk_layout_space_push(&g_nk,nk_rect(lx2, dy, wT2, 16));
        {struct nk_style_button sbb=g_nk.style.button;
         g_nk.style.button.normal.data.color=nk_rgba(0,0,0,0);
         g_nk.style.button.hover.data.color=nk_rgba(0,0,0,0);
         g_nk.style.button.active.data.color=nk_rgba(0,0,0,0);
         g_nk.style.button.border=0;g_nk.style.button.padding=nk_vec2(0,0);
         g_nk.style.text.color=nk_rgba(0,0,0,0);
         nk_style_push_font(&g_nk,&g_fXs.nk);
         if(nk_button_label(&g_nk," "))
             ShellExecuteA(NULL,"open","https://perplexity.ai",NULL,NULL,SW_SHOWNORMAL);
         nk_style_pop_font(&g_nk);g_nk.style.button=sbb;g_nk.style.text.color=C_TEXT;}
    }
    nk_layout_space_end(&g_nk);

    /* ---- separator ---- */
    nk_layout_row_dynamic(&g_nk,6,1);nk_spacing(&g_nk,1);
    nk_layout_row_dynamic(&g_nk,1,1);
    {struct nk_rect r;nk_widget(&r,&g_nk);nk_fill_rect(cb,r,0,C_BORDER);}
    nk_layout_row_dynamic(&g_nk,4,1);nk_spacing(&g_nk,1);

    /* ---- Info rows: "Label: Value" compact left-aligned ---- */
    auto info=[&](const char*k,const char*v){
        char kl[128]; sprintf_s(kl,"%s: ",k);
        float klw=tw_xs(kl)+2.f;
        float valw=ww-klw-4.f;
        nk_layout_row_begin(&g_nk,NK_STATIC,18,2);
        nk_layout_row_push(&g_nk,klw);
        nk_style_push_font(&g_nk,&g_fXs.nk);
        g_nk.style.text.color=C_MUTED;nk_label(&g_nk,kl,NK_TEXT_LEFT);
        nk_layout_row_push(&g_nk,valw);
        g_nk.style.text.color=C_PUR_L;nk_label(&g_nk,v,NK_TEXT_LEFT);
        g_nk.style.text.color=C_TEXT;nk_style_pop_font(&g_nk);nk_layout_row_end(&g_nk);
    };
    info("Version",     APP_VERSION);
    info("Platform",    "Windows 10 / 11  (Win32 API)");
    info("UI",          "Nuklear  (MIT / Unlicense)");
    info("Filesystem",  "Boost.Filesystem  (BSL-1.0)");
    info("Images",      "GDI+  (Windows SDK)");
    info("Config",      "makeshift_capture.ini");

    nk_layout_row_dynamic(&g_nk,6,1);nk_spacing(&g_nk,1);

    /* GitHub button */
    nk_layout_row_dynamic(&g_nk,28,1);
    {struct nk_style_button sb=g_nk.style.button;
     g_nk.style.button.normal.data.color=C_PUR_D;g_nk.style.button.border_color=C_PUR;
     g_nk.style.text.color=C_TEXT;nk_style_push_font(&g_nk,&g_fXs.nk);
     if(nk_button_label(&g_nk,T(S_ABOUT_GITHUB)))
         ShellExecuteA(NULL,"open","https://github.com/Northstrix/makeshift-capture-tool",NULL,NULL,SW_SHOWNORMAL);
     nk_style_pop_font(&g_nk);g_nk.style.button=sb;g_nk.style.text.color=C_TEXT;}

    nk_layout_row_dynamic(&g_nk,4,1);nk_spacing(&g_nk,1);

    /* ---- Third-party Licenses (auto-height) ---- */
    section_label(T(S_ABOUT_TPLIBS));
    static const char TPLIC[]=
        "------------------------------------------------------------------------------\n"
        "Nuklear is available under 2 licenses\n"
        "------------------------------------------------------------------------------\n"
        "ALTERNATIVE A - MIT License\n"
        "Copyright (c) 2017 Micha Mettke\n"
        "Permission is hereby granted, free of charge, to any person obtaining a copy of\n"
        "this software and associated documentation files (the \"Software\"), to deal in\n"
        "the Software without restriction, including without limitation the rights to\n"
        "use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies\n"
        "of the Software, and to permit persons to whom the Software is furnished to do\n"
        "so, subject to the following conditions:\n"
        "The above copyright notice and this permission notice shall be included in all\n"
        "copies or substantial portions of the Software.\n"
        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
        "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
        "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
        "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
        "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
        "SOFTWARE.\n"
        "------------------------------------------------------------------------------\n"
        "ALTERNATIVE B - Public Domain (www.unlicense.org)\n"
        "This is free and unencumbered software released into the public domain.\n"
        "Anyone is free to copy, modify, publish, use, compile, sell, or distribute this\n"
        "software, either in source code form or as a compiled binary, for any purpose,\n"
        "commercial or non-commercial, and by any means.\n"
        "In jurisdictions that recognize copyright laws, the author or authors of this\n"
        "software dedicate any and all copyright interest in the software to the public\n"
        "domain. We make this dedication for the benefit of the public at large and to\n"
        "the detriment of our heirs and successors. We intend this dedication to be an\n"
        "overt act of relinquishment in perpetuity of all present and future rights to\n"
        "this software under copyright law.\n"
        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
        "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
        "AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN\n"
        "ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION\n"
        "WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n"
        "-----------------------------------------------------------------------------\n"
        "\n"
        "Boost Software License - Version 1.0 - August 17th, 2003\n"
        "\n"
        "Permission is hereby granted, free of charge, to any person or organization\n"
        "obtaining a copy of the software and accompanying documentation covered by\n"
        "this license (the \"Software\") to use, reproduce, display, distribute,\n"
        "execute, and transmit the Software, and to prepare derivative works of the\n"
        "Software, and to permit third-parties to whom the Software is furnished to\n"
        "do so, all subject to the following:\n"
        "\n"
        "The copyright notices in the Software and this entire statement, including\n"
        "the above license grant, this restriction and the following disclaimer,\n"
        "must be included in all copies of the Software, in whole or in part, and\n"
        "all derivative works of the Software, unless such copies or derivative\n"
        "works are solely in the form of machine-executable object code generated by\n"
        "a source language processor.\n"
        "\n"
        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
        "FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT\n"
        "SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE\n"
        "FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,\n"
        "ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER\n"
        "DEALINGS IN THE SOFTWARE.\n"
        "\n"
        "Microsoft GDI+ (Windows SDK)\n"
        "Copyright © Microsoft Corporation. All rights reserved.\n"
        "Use of GDI+ is subject to the Windows SDK License Agreement.\n";
    {int lc=0;for(const char*p=TPLIC;*p;p++)if(*p=='\n')lc++;
     float ah=(float)(lc+1)*17.f+10.f;
     nk_layout_row_dynamic(&g_nk,(int)ah,1);
     if(nk_group_scrolled_begin(&g_nk,&g_tp_scroll,"TpLic",NK_WINDOW_BORDER)){
         nk_layout_row_dynamic(&g_nk,16,1);nk_style_push_font(&g_nk,&g_fXs.nk);
         const char*p=TPLIC;
         while(*p){const char*nl=strchr(p,'\n');int len=nl?(int)(nl-p):(int)strlen(p);
             char lb[200]={};strncpy_s(lb,p,min(len,199));
             g_nk.style.text.color=C_MUTED;nk_label(&g_nk,lb,NK_TEXT_LEFT);
             p+=(nl?(len+1):len);if(!nl)break;}
         g_nk.style.text.color=C_TEXT;nk_style_pop_font(&g_nk);
         nk_group_scrolled_end(&g_nk);}}

    /* ---- App License (auto-height) ---- */
    nk_layout_row_dynamic(&g_nk,4,1);nk_spacing(&g_nk,1);
    section_label(T(S_ABOUT_LICENSE));
    static const char APPLIC[]=
        "Makeshift Capture Tool\n"
        "Copyright (c) 2026 Maxim Bortnikov\n"
        "MIT License\n"
        "\n"
        "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
        "of this software and associated documentation files (the \"Software\"), to deal\n"
        "in the Software without restriction, including without limitation the rights\n"
        "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
        "copies of the Software, and to permit persons to whom the Software is\n"
        "furnished to do so, subject to the following conditions:\n"
        "\n"
        "The above copyright notice and this permission notice shall be included in all\n"
        "copies or substantial portions of the Software.\n"
        "\n"
        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
        "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
        "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
        "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
        "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
        "SOFTWARE.\n";
    {int lc=0;for(const char*p=APPLIC;*p;p++)if(*p=='\n')lc++;
     float ah=(float)(lc+1)*17.f+10.f;
     nk_layout_row_dynamic(&g_nk,(int)ah,1);
     if(nk_group_scrolled_begin(&g_nk,&g_ap_scroll,"ApLic",NK_WINDOW_BORDER)){
         nk_layout_row_dynamic(&g_nk,16,1);nk_style_push_font(&g_nk,&g_fXs.nk);
         const char*p=APPLIC;
         while(*p){const char*nl=strchr(p,'\n');int len=nl?(int)(nl-p):(int)strlen(p);
             char lb[200]={};strncpy_s(lb,p,min(len,199));
             g_nk.style.text.color=C_MUTED;nk_label(&g_nk,lb,NK_TEXT_LEFT);
             p+=(nl?(len+1):len);if(!nl)break;}
         g_nk.style.text.color=C_TEXT;nk_style_pop_font(&g_nk);
         nk_group_scrolled_end(&g_nk);}}

    nk_layout_row_dynamic(&g_nk,10,1);nk_spacing(&g_nk,1);
}

/* ==================================================================
   FULL FRAME
   ================================================================== */
static void draw_frame(float cw,float ch){
    if(!nk_begin(&g_nk,"Main",nk_rect(0,0,cw,ch),NK_WINDOW_NO_SCROLLBAR)){nk_end(&g_nk);return;}

    /* Title bar */
    nk_layout_row_dynamic(&g_nk,44,1);
    {struct nk_rect tb;nk_widget(&tb,&g_nk);
     struct nk_command_buffer*cb=nk_window_get_canvas(&g_nk);
     nk_fill_rect(cb,tb,0,C_SURF);nk_fill_rect(cb,nk_rect(tb.x,tb.y,4,tb.h),0,C_PUR);
     nk_draw_text(cb,nk_rect(tb.x+16,tb.y+7,tb.w-24,26),
         APP_NAME,(int)strlen(APP_NAME),&g_fTitle.nk,C_SURF,C_TEXT);}

    /* Tab bar */
    const char*tlbls[]={T(S_TAB_CAPTURE),T(S_TAB_SHOT),T(S_TAB_SETTINGS),T(S_TAB_ABOUT)};
    float tw=(nk_window_get_width(&g_nk)-24)/(float)T_COUNT;
    nk_layout_row_begin(&g_nk,NK_STATIC,30,T_COUNT);
    for(int i=0;i<T_COUNT;i++){
        nk_layout_row_push(&g_nk,tw);
        struct nk_style_button sb=g_nk.style.button;
        g_nk.style.button.rounding=0;g_nk.style.button.border=0;
        if(i==g_tab){g_nk.style.button.normal.data.color=C_CARD;g_nk.style.button.hover.data.color=C_CARD;g_nk.style.button.active.data.color=C_CARD;g_nk.style.text.color=C_TEXT;}
        else{g_nk.style.button.normal.data.color=C_SURF;g_nk.style.button.hover.data.color=C_CARD;g_nk.style.button.active.data.color=C_CARD;g_nk.style.text.color=C_MUTED;}
        nk_style_push_font(&g_nk,&g_fXs.nk);
        if(nk_button_label(&g_nk,tlbls[i]))g_tab=i;
        nk_style_pop_font(&g_nk);g_nk.style.button=sb;g_nk.style.text.color=C_TEXT;
    }
    nk_layout_row_end(&g_nk);
    nk_layout_row_dynamic(&g_nk,2,1);
    {struct nk_rect lr;nk_widget(&lr,&g_nk);
     nk_fill_rect(nk_window_get_canvas(&g_nk),nk_rect(lr.x+g_tab*tw,lr.y,tw,2),0,C_PUR);}

    /* Scrollable content */
    float ch2=ch-44-32-2;
    nk_layout_row_dynamic(&g_nk,(int)ch2,1);
    if(nk_group_scrolled_begin(&g_nk,&g_tabScroll[g_tab],"TC",NK_WINDOW_BORDER)){
        switch(g_tab){
        case T_CAPTURE:  tab_capture();  break;
        case T_SHOT:     tab_shot();     break;
        case T_SETTINGS: tab_settings(); break;
        case T_ABOUT:    tab_about();    break;
        }
        nk_group_scrolled_end(&g_nk);
    }
    nk_end(&g_nk);
}

/* ==================================================================
   GDI RENDER  (Onda Sfasata: DT_CENTER|DT_VCENTER|DT_SINGLELINE)
   ================================================================== */
static void nk_gdi_render(HDC hdc,RECT*rc){
    g_logoBlit.valid=false;
    HDC mDC=CreateCompatibleDC(hdc);
    HBITMAP mBM=CreateCompatibleBitmap(hdc,rc->right,rc->bottom);
    SelectObject(mDC,mBM);
    HBRUSH bg=CreateSolidBrush(RGB(10,9,16));FillRect(mDC,rc,bg);DeleteObject(bg);

    const struct nk_command*cmd;
    nk_foreach(cmd,&g_nk){
        switch(cmd->type){
        case NK_COMMAND_RECT_FILLED:{
            const struct nk_command_rect_filled*r=(const struct nk_command_rect_filled*)cmd;
            if(r->rounding>0){HBRUSH br=CreateSolidBrush(RGB(r->color.r,r->color.g,r->color.b));
                SelectObject(mDC,br);SelectObject(mDC,GetStockObject(NULL_PEN));
                RoundRect(mDC,(int)r->x,(int)r->y,(int)(r->x+r->w),(int)(r->y+r->h),(int)r->rounding*2,(int)r->rounding*2);DeleteObject(br);}
            else{HBRUSH br=CreateSolidBrush(RGB(r->color.r,r->color.g,r->color.b));
                RECT gr={(int)r->x,(int)r->y,(int)(r->x+r->w),(int)(r->y+r->h)};FillRect(mDC,&gr,br);DeleteObject(br);}
        }break;
        case NK_COMMAND_RECT:{
            const struct nk_command_rect*r=(const struct nk_command_rect*)cmd;
            HPEN p=CreatePen(PS_SOLID,r->line_thickness,RGB(r->color.r,r->color.g,r->color.b));
            SelectObject(mDC,p);SelectObject(mDC,GetStockObject(NULL_BRUSH));
            if(r->rounding>0)RoundRect(mDC,(int)r->x,(int)r->y,(int)(r->x+r->w),(int)(r->y+r->h),(int)r->rounding*2,(int)r->rounding*2);
            else Rectangle(mDC,(int)r->x,(int)r->y,(int)(r->x+r->w),(int)(r->y+r->h));
            DeleteObject(p);}break;
        case NK_COMMAND_CIRCLE_FILLED:{
            const struct nk_command_circle_filled*c=(const struct nk_command_circle_filled*)cmd;
            HBRUSH br=CreateSolidBrush(RGB(c->color.r,c->color.g,c->color.b));
            SelectObject(mDC,br);SelectObject(mDC,GetStockObject(NULL_PEN));
            Ellipse(mDC,(int)c->x,(int)c->y,(int)(c->x+c->w),(int)(c->y+c->h));DeleteObject(br);}break;
        case NK_COMMAND_LINE:{
            const struct nk_command_line*l=(const struct nk_command_line*)cmd;
            HPEN p=CreatePen(PS_SOLID,l->line_thickness,RGB(l->color.r,l->color.g,l->color.b));
            SelectObject(mDC,p);MoveToEx(mDC,(int)l->begin.x,(int)l->begin.y,NULL);
            LineTo(mDC,(int)l->end.x,(int)l->end.y);DeleteObject(p);}break;
        case NK_COMMAND_TEXT:{
            const struct nk_command_text*t=(const struct nk_command_text*)cmd;
            SetBkMode(mDC,TRANSPARENT);SetTextColor(mDC,RGB(t->foreground.r,t->foreground.g,t->foreground.b));
            RECT tr={(int)t->x,(int)t->y,(int)(t->x+t->w),(int)(t->y+t->h)};
            UINT al=DT_CENTER|DT_VCENTER|DT_SINGLELINE;if(t->h>50)al=DT_LEFT|DT_WORDBREAK;
            DrawTextUTF8(mDC,t->string,t->length,&tr,al,((nk_gdi_font*)t->font->userdata.ptr)->handle);}break;
        case NK_COMMAND_SCISSOR:{
            const struct nk_command_scissor*s=(const struct nk_command_scissor*)cmd;
            HRGN rg=CreateRectRgn((int)s->x,(int)s->y,(int)(s->x+s->w),(int)(s->y+s->h));
            SelectClipRgn(mDC,rg);DeleteObject(rg);}break;
        default:break;
        }
    }
    /* Logo blit */
    if(g_logoBlit.valid&&g_logoBlit.hbm){
        HDC src=CreateCompatibleDC(mDC);HBITMAP old2=(HBITMAP)SelectObject(src,g_logoBlit.hbm);
        SetStretchBltMode(mDC,HALFTONE);
        StretchBlt(mDC,g_logoBlit.x,g_logoBlit.y,g_logoBlit.w,g_logoBlit.h,
                   src,0,0,g_logoBlit.sw,g_logoBlit.sh,SRCCOPY);
        SelectObject(src,old2);DeleteDC(src);g_logoBlit.valid=false;
    }
    BitBlt(hdc,0,0,rc->right,rc->bottom,mDC,0,0,SRCCOPY);
    DeleteObject(mBM);DeleteDC(mDC);
}

/* ==================================================================
   WINDOW PROCEDURE
   ================================================================== */
static HDC g_hdc=NULL;

static LRESULT CALLBACK WndProc(HWND hw,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
    case WM_GETMINMAXINFO:{MINMAXINFO*mm=(MINMAXINFO*)lp;mm->ptMinTrackSize={400,400};return 0;}

    case WM_SIZE:
        if(wp==SIZE_MAXIMIZED){
            g_cfg.winW=-1;g_cfg.winH=-1;ini_save(g_cfg);
        }else if(wp==SIZE_RESTORED){
            RECT r;GetWindowRect(hw,&r);
            g_cfg.winW=r.right-r.left;g_cfg.winH=r.bottom-r.top;ini_save(g_cfg);
        }else if(wp==SIZE_MINIMIZED&&g_cfg.minimizeToTray){
            ShowWindow(hw,SW_HIDE);g_windowVisible=false;
        }
        return 0;

    case WM_CLOSE:{
        WINDOWPLACEMENT wpl={sizeof(wpl)};GetWindowPlacement(hw,&wpl);
        if(wpl.showCmd==SW_SHOWMAXIMIZED){g_cfg.winW=-1;g_cfg.winH=-1;}
        else{RECT r;GetWindowRect(hw,&r);g_cfg.winW=r.right-r.left;g_cfg.winH=r.bottom-r.top;}
        flush_edits();ini_save(g_cfg);
        if(g_cfg.minimizeToTray){ShowWindow(hw,SW_HIDE);g_windowVisible=false;}
        else DestroyWindow(hw);
        return 0;}

    case WM_DESTROY:
        hk_listen_cancel();tray_remove();
        for(int i=0;i<HK_COUNT;i++)UnregisterHotKey(hw,HK_BASE+i);
        flush_edits();ini_save(g_cfg);
        if(g_logoBmp)DeleteObject(g_logoBmp);
        PostQuitMessage(0);return 0;

    case WM_USER+10:
        if(g_listenIdx>=0)hk_apply(g_listenIdx,(int)wp,(int)lp);
        return 0;

    case WM_HOTKEY:{
        int idx=(int)(wp-HK_BASE);
        if(idx>=0&&idx<HK_COUNT)do_screenshot(idx,false);
        return 0;}

    case WM_TRAYICON:
        if(lp==WM_RBUTTONUP||lp==WM_CONTEXTMENU)tray_menu();
        else if(lp==WM_LBUTTONDBLCLK){ShowWindow(hw,SW_RESTORE);SetForegroundWindow(hw);g_windowVisible=true;}
        return 0;

    case WM_COMMAND:
        switch(LOWORD(wp)){
        case ID_TRAY_OPEN:  ShowWindow(hw,SW_RESTORE);SetForegroundWindow(hw);g_windowVisible=true;break;
        case ID_TRAY_RECT:  do_screenshot(0,true);break;
        case ID_TRAY_FULL:  do_screenshot(1,true);break;
        case ID_TRAY_FIXED: do_screenshot(2,true);break;
        case ID_TRAY_EXIT:  DestroyWindow(hw);break;
        }
        return 0;

    case WM_LBUTTONDOWN:nk_input_button(&g_nk,NK_BUTTON_LEFT, GET_X_LPARAM(lp),GET_Y_LPARAM(lp),1);return 0;
    case WM_LBUTTONUP:  nk_input_button(&g_nk,NK_BUTTON_LEFT, GET_X_LPARAM(lp),GET_Y_LPARAM(lp),0);return 0;
    case WM_RBUTTONDOWN:nk_input_button(&g_nk,NK_BUTTON_RIGHT,GET_X_LPARAM(lp),GET_Y_LPARAM(lp),1);return 0;
    case WM_RBUTTONUP:  nk_input_button(&g_nk,NK_BUTTON_RIGHT,GET_X_LPARAM(lp),GET_Y_LPARAM(lp),0);return 0;
    case WM_MOUSEMOVE:  nk_input_motion(&g_nk,GET_X_LPARAM(lp),GET_Y_LPARAM(lp));return 0;
    case WM_MOUSEWHEEL:
        nk_input_scroll(&g_nk,nk_vec2(0,(float)(GET_WHEEL_DELTA_WPARAM(wp)/WHEEL_DELTA)));return 0;
    case WM_CHAR:nk_input_char(&g_nk,(char)wp);return 0;
    case WM_KEYDOWN:case WM_KEYUP:{
        int dn=(msg==WM_KEYDOWN);
        if(wp==VK_SHIFT) nk_input_key(&g_nk,NK_KEY_SHIFT,dn);
        if(wp==VK_DELETE)nk_input_key(&g_nk,NK_KEY_DEL,dn);
        if(wp==VK_RETURN)nk_input_key(&g_nk,NK_KEY_ENTER,dn);
        if(wp==VK_TAB)   nk_input_key(&g_nk,NK_KEY_TAB,dn);
        if(wp==VK_BACK)  nk_input_key(&g_nk,NK_KEY_BACKSPACE,dn);
        if(wp==VK_LEFT)  nk_input_key(&g_nk,NK_KEY_LEFT,dn);
        if(wp==VK_RIGHT) nk_input_key(&g_nk,NK_KEY_RIGHT,dn);
        if(wp==VK_UP)    nk_input_key(&g_nk,NK_KEY_UP,dn);
        if(wp==VK_DOWN)  nk_input_key(&g_nk,NK_KEY_DOWN,dn);
        if(wp==VK_HOME)  nk_input_key(&g_nk,NK_KEY_TEXT_START,dn);
        if(wp==VK_END)   nk_input_key(&g_nk,NK_KEY_TEXT_END,dn);
        if(GetKeyState(VK_CONTROL)&0x8000){
            if(wp=='A')nk_input_key(&g_nk,NK_KEY_TEXT_SELECT_ALL,dn);
            if(wp=='C')nk_input_key(&g_nk,NK_KEY_COPY,dn);
            if(wp=='V')nk_input_key(&g_nk,NK_KEY_PASTE,dn);
            if(wp=='X')nk_input_key(&g_nk,NK_KEY_CUT,dn);
            if(wp=='Z')nk_input_key(&g_nk,NK_KEY_TEXT_UNDO,dn);
        }
        return 0;}
    case WM_ERASEBKGND:return 1;
    }
    return DefWindowProc(hw,msg,wp,lp);
}

/* ==================================================================
   WINMAIN
   ================================================================== */
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR,int){
    CoInitialize(NULL);SetProcessDPIAware();
    GdiplusStartupInput gsi;ULONG_PTR gdipTok;GdiplusStartup(&gdipTok,&gsi,NULL);
    srand((unsigned)time(NULL));

    load_logo();
    ini_load(g_cfg);
    g_lang=g_cfg.language;
    try{bfs::create_directories(g_cfg.outputDir);}catch(...){}

    WNDCLASSA wc={0};
    wc.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;wc.lpfnWndProc=WndProc;
    wc.hInstance=hInst;wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName=APP_CLASS;wc.hIcon=make_icon();
    RegisterClassA(&wc);

    int W=(g_cfg.winW>0)?g_cfg.winW:600;
    int H=(g_cfg.winH>0)?g_cfg.winH:540;
    g_hwnd=CreateWindowExA(
        g_cfg.alwaysOnTop?WS_EX_TOPMOST:0,
        APP_CLASS,APP_NAME,WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,CW_USEDEFAULT,W,H,NULL,NULL,hInst,NULL);

    g_hdc=GetDC(g_hwnd);

    g_fXs   =make_font(13,FW_NORMAL,  "Segoe UI");
    g_fSm   =make_font(15,FW_NORMAL,  "Segoe UI");
    g_fMd   =make_font(17,FW_SEMIBOLD,"Segoe UI");
    g_fTitle=make_font(19,FW_BOLD,    "Segoe UI Semibold");
    patch_fonts();

    nk_init_default(&g_nk,&g_fSm.nk);
    apply_theme();sync_edits();register_hotkeys();tray_add();
    set_status(T(S_READY),"muted");

    if(g_cfg.winW==-1&&g_cfg.winH==-1)ShowWindow(g_hwnd,SW_MAXIMIZE);
    else ShowWindow(g_hwnd,SW_SHOW);
    UpdateWindow(g_hwnd);

    while(1){
        MSG msg;
        nk_input_begin(&g_nk);
        while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            if(msg.message==WM_QUIT)goto cleanup;
            TranslateMessage(&msg);DispatchMessage(&msg);
        }
        nk_input_end(&g_nk);
        if(!g_windowVisible){Sleep(30);continue;}
        RECT rc;GetClientRect(g_hwnd,&rc);
        float cw=(float)rc.right,ch=(float)rc.bottom;
        if(cw<1||ch<1){Sleep(16);continue;}
        draw_frame(cw,ch);
        nk_gdi_render(g_hdc,&rc);
        nk_clear(&g_nk);
        Sleep(16);
    }
cleanup:
    GdiplusShutdown(gdipTok);
    DeleteObject(g_fXs.handle);DeleteObject(g_fSm.handle);
    DeleteObject(g_fMd.handle);DeleteObject(g_fTitle.handle);
    nk_free(&g_nk);ReleaseDC(g_hwnd,g_hdc);CoUninitialize();
    return 0;
}
