#include <map>

namespace WiFiSettingsLanguage {

struct Texts {
  const __FlashStringHelper *title, *portal_wpa, *portal_password, *wait, *bye,
      *error_fs, *button_save, *button_restart, *scanning_short, *scanning_long,
      *rescan, *dot1x, *ssid, *wifi_password, *timezone ,*language;
  const char *init;
};

#if !defined LANGUAGE_EN && !defined LANGUAGE_NL && !defined LANGUAGE_DE &&    \
    !defined LANGUAGE_TW
#define LANGUAGE_ALL
#endif

std::map<const String, const String> languages {
// Ordered alphabetically
#if defined LANGUAGE_DE || defined LANGUAGE_ALL
  {"de", "Deutsch"},
#endif
#if defined LANGUAGE_EN || defined LANGUAGE_ALL
      {"en", "English"},
#endif
#if defined LANGUAGE_NL || defined LANGUAGE_ALL
      {"nl", "Nederlands"},
#endif
#if defined LANGUAGE_TW || defined LANGUAGE_ALL
      {"tw", "中文"},
#endif
};

bool available(const String &language) {
  return languages.count(language) == 1;
}

bool multiple() { return languages.size() > 1; }

bool select(Texts &T, String &language) {
  if (!available(language)) {
    if (available("tw"))
      language = "tw";
    else
      language = languages.begin()->first;
  }

#if defined LANGUAGE_EN || defined LANGUAGE_ALL
  if (language == "en") {
    T.title = F("Configuration");
    T.portal_wpa = F("Protect the configuration portal with a WiFi password");
    T.portal_password = F("WiFi password for the configuration portal");
    T.init = "default";
    T.wait = F("Wait for it...");
    T.bye = F("Bye!");
    T.error_fs = F("Error while writing to flash filesystem.");
    T.button_save = F("Save");
    T.button_restart = F("Restart device");
    T.scanning_short = F("Scanning...");
    T.scanning_long = F("Scanning for WiFi networks...");
    T.rescan = F("rescan");
    T.dot1x = F("(won't work: 802.1x is not supported)");
    T.ssid = F("WiFi network name (SSID)");
    T.wifi_password = F("WiFi password");
    T.timezone = F("Time zone") ;
    T.language = F("Language");
    return true;
  }
#endif

#if defined LANGUAGE_TW || defined LANGUAGE_ALL
  if (language == "tw") {
    T.title = F("設定");
    T.portal_wpa = F("使用WiFi密碼保護");
    T.portal_password = F("配置的WiFi密碼");
    T.init = "默認";
    T.wait = F("等待連接中...");
    T.bye = F("Bye!");
    T.error_fs = F("寫入Flash文件系統時出錯.");
    T.button_save = F("儲存");
    T.button_restart = F("重新起動裝置");
    T.scanning_short = F("掃描中...");
    T.scanning_long = F("掃描WiFi網絡...");
    T.rescan = F("重新掃描");
    T.dot1x = F("(無法使用：不支持802.1x)");
    T.ssid = F("WiFi網絡名稱 (SSID)");
    T.wifi_password = F("WiFi 密碼");
    T.timezone = F("時區") ;
    T.language = F("語言");
    return true;
  }
#endif

#if defined LANGUAGE_NL || defined LANGUAGE_ALL
  if (language == "nl") {
    T.title = F("Configuratie");
    T.portal_wpa = F("Beveilig de configuratieportal met een WiFi-wachtwoord");
    T.portal_password = F("WiFi-wachtwoord voor de configuratieportal");
    T.init = "standaard";
    T.wait = F("Even wachten...");
    T.bye = F("Doei!");
    T.error_fs = F("Fout bij het schrijven naar het flash-bestandssysteem.");
    T.button_save = F("Opslaan");
    T.button_restart = F("Herstarten");
    T.scanning_short = F("Scant...");
    T.scanning_long = F("Zoeken naar WiFi-netwerken...");
    T.rescan = F("opnieuw scannen");
    T.dot1x = F("(werkt niet: 802.1x wordt niet ondersteund)");
    T.ssid = F("WiFi-netwerknaam (SSID)");
    T.wifi_password = F("WiFi-wachtwoord");
    T.timezone = F("Tijdzone") ;
    T.language = F("Taal");

    return true;
  }
#endif

#if defined LANGUAGE_DE || defined LANGUAGE_ALL
  if (language == "de") {
    T.title = F("Konfiguration");
    T.portal_wpa = F("Das Konfigurationsportal mit einem Passwort schützen");
    T.portal_password = F("Passwort für das Konfigurationsportal");
    T.init = "Standard";
    T.wait = F("Warten...");
    T.bye = F("Tschüss!");
    T.error_fs = F("Fehler beim Schreiben auf das Flash-Dateisystem");
    T.button_save = F("Speichern");
    T.button_restart = F("Gerät neustarten");
    T.scanning_short = F("Suchen...");
    T.scanning_long = F("Suche nach WiFi-Netzwerken...");
    T.rescan = F("Erneut suchen");
    T.dot1x = F("(nicht möglich: 802.1x nicht unterstützt)");
    T.ssid = F("WiFi Netzwerkname (SSID)");
    T.wifi_password = F("WiFi Passwort");
     T.timezone = F("Zeitzone") ;
    T.language = F("Sprache");
    return true;
  }
#endif

  return false;
}

} // namespace WiFiSettingsLanguage
