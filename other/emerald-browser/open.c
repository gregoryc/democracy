// micro open.c; sudo musl-gcc -static -O3 -s open.c -o open

#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
  // size_t argv_1_len, i;
  for (int a = 1; a < argc; ++a) {
    size_t argv_1_len = strlen(argv[a]);
    size_t i = 0;
    while (i < argv_1_len) {
      if (argv[a][i] == '+')
        argv[a][i] = 'p';
      if (argv[a][i] == ' ')
        argv[a][i] = '+';
      ++i;
    }
    static char query1[] = "curl -sL 'https://www.google.com/search?q=";

    static const char query2[] =
        "&btnK=Google+Search&sxsrf=AOaemvK8_qzJx1JC2rYSvD0FXC-RGo5obQ%"
        "3A1634838300584&source=hp&ei=HKdxYYeWIYPDmAWS8IjQCQ&iflsig=ALs-"
        "wAMAAAAAYXG1LJix9FrVBMD9VvsWIgMA20RyRE7o' \\"
        "-H 'authority: www.google.com' \\"
        "-H 'cache-control: max-age=0' \\"
        "-H 'sec-ch-ua: \"Chromium\";v=\"94\", \"Google Chrome\";v=\"94\", "
        "\";Not A Brand\";v=\"99\"' \\"
        "-H 'sec-ch-ua-mobile: ?0' \\"
        "-H 'sec-ch-ua-platform: \"Linux\"' \\"
        "-H 'upgrade-insecure-requests: 1' \\"
        "-H 'user-agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
        "(KHTML, like Gecko) Chrome/94.0.4606.81 Safari/537.36' \\"
        "-H 'accept: "
        "text/html,application/xhtml+xml,application/xml;q=0.9,image/"
        "avif,image/webp,image/apng,*/*;q=0.8,application/"
        "signed-exchange;v=b3;q=0.9' \\"
        "-H 'x-client-data: "
        "CIS2yQEIpLbJAQipncoBCKmCywEI6vLLAQjv8ssBCJ75ywEI3ITMAQjnhMwBCLWFzAEI24"
        "XMAQj/hcwBCICGzAE=' \\"
        "-H 'sec-fetch-site: same-origin' \\"
        "-H 'sec-fetch-mode: navigate' \\"
        "-H 'sec-fetch-user: ?1' \\"
        "-H 'sec-fetch-dest: document' \\"
        "-H 'referer: https://www.google.com/' \\"
        "-H 'accept-language: en-GB,en-US;q=0.9,en;q=0.8' \\"
        "-H 'cookie: OTZ=6204887_72_76_104100_72_446760; "
        "SID=DAiFVqmLaDVo34R0Ivt5_"
        "BspZ7s6hfkPkaoIyh3YHcRB5AcpUVhwburMgkQwQ9CJoRBR2Q.; "
        "__Secure-1PSID=DAiFVqmLaDVo34R0Ivt5_"
        "BspZ7s6hfkPkaoIyh3YHcRB5AcpGO0efWAN-1hoyRhkM2Id1A.; "
        "__Secure-3PSID=DAiFVqmLaDVo34R0Ivt5_"
        "BspZ7s6hfkPkaoIyh3YHcRB5Acp8QHHuLT3pB7YSRXqcLQZng.; "
        "HSID=AKVTidlHdJgc4nune; SSID=ATumLTJyjZP7Ee2dz; "
        "APISID=UJMT6p0dHVFxpD07/A4oLvgb5RhBKBpJd3; "
        "SAPISID=xKW5_w6fZMnoHw6B/AsjziB6RkQ1dja7G8; "
        "__Secure-1PAPISID=xKW5_w6fZMnoHw6B/AsjziB6RkQ1dja7G8; "
        "__Secure-3PAPISID=xKW5_w6fZMnoHw6B/AsjziB6RkQ1dja7G8; "
        "OGPC=19022622-1:; SEARCH_SAMESITE=CgQI6ZMB; 1P_JAR=2021-10-21-17; "
        "NID=511=WlwS-m60Q8k1MlhA3r7I1LuTJ7Wk0KxJmXowelI4o4g3W1Le31WzZfpY2-"
        "uN2j41ew7h4ZsugzZGhipfR62qGxNwqHG1DwY0dXk5H_"
        "GIjCbLHDB9VKN11GrzKRMoxKRHOuF-napP8LNSuBMJNkCaV9l-"
        "c4BkYKu6j2gTCsJzqtBZMp0DAiU4rZHbVYaaEQXgKzl9I93hJlT0pdzWAUknHf5yIeEr27"
        "0Xa04tSZ8DOP3jyLsRI469KfxL78krw1EDzb9nnALxHeIaeRtkdQCpfVct4A; "
        "DV=c_T1HawivqVIANEH2apbUwahWE0_"
        "yldavBJY24mGNAEAADASVXgWDq5OiwEAALy2H6usCMoVaQAAAA; "
        "SIDCC="
        "AJi4QfHdkFTnVxFMSXGET6e3oUlek8C2AbwXYj7p3wK553fywKHBzBnGDkCfOzm0L6tKRl"
        "4A5bE; "
        "__Secure-3PSIDCC=AJi4QfEh6GQl_xhFa1OinFVTrEMBWCTd_NJ5g9GKrS1vJ_"
        "5tIgKCPgaOvhvfF_T7Y-VWyroSMw' \\"
        "--compressed | get_all_links | fgrep -v google.com | sort -uR | grep "
        "-P ^ht|head -n 1|xargs  printf 'open %s\\n'|tee  "
        "/tmp/emerald-browser-fifo";

    char query[sizeof(query1) - 1 + argv_1_len + sizeof(query2) + 1 - 1];
    memcpy(query, query1, sizeof(query1) - 1);
    memcpy(query + sizeof(query1) - 1, argv[a], argv_1_len);
    memcpy(query + sizeof(query1) - 1 + argv_1_len, query2,
           sizeof(query2) - 1 + 1);

    int fork_status = 0;
    // pu
    if (argc > 1)
      fork_status = fork();

    if (fork_status == 0) {
      execl("/bin/sh", "sh", "-c", query, (char *)NULL);
    }
  }
}
