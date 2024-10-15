# AnimeWatch
AnimeWatch is a lightweight program for watching anime. Created only for educational purposes.

## How it works
AnimeWatch bases on libcurl, libxml and Qt6. When user is looking for a title, he is actually sumbitting request like 'https://animego.org/search/anime?q={title}' to the AnimeGO. Then the result of request is parsed with libxml and user receives episodes, dubs and links to episode.

## Build
```
qmake6 main.pro
make
```

