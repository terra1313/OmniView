#pragma once

#include "jasonhandler.hpp"

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

bool send_to_api(
  nlohmann::json const& config,
  std::string const&    file,
  std::string const&    vin,
  std::string const&    scantype) {
    CURL*    curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        std::string url = load_json<std::string>(config, "api", "url") + "?"
                        + load_json<std::string>(config, "api", "input1") + "=" + vin + "&"
                        + load_json<std::string>(config, "api", "input2") + "=" + scantype;

        // Hinzufügen der Daten als Formfelder
        curl_mime*     mime = curl_mime_init(curl);
        curl_mimepart* part;

        // Datei

        part = curl_mime_addpart(mime);
        curl_mime_name(part, "file");
        curl_mime_filedata(part, file.c_str());

        struct curl_slist* header_list = NULL;
        header_list                    = curl_slist_append(
          header_list,
          load_json<std::string>(config, "api", "header1").c_str());
        header_list = curl_slist_append(
          header_list,
          load_json<std::string>(config, "api", "header2").c_str());

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        // Senden der Anfrage

        res = curl_easy_perform(curl);

        // Überprüfung auf Fehler

        if(res != CURLE_OK)
            fmt::print("Fehler beim Hochladen der Datei:{}\n\r ", curl_easy_strerror(res));

        // Aufräumen
        curl_easy_cleanup(curl);
        curl_slist_free_all(header_list);
    } else {
    }

    // Aufräumen von cURL

    curl_global_cleanup();

    return false;
}