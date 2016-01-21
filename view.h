#ifndef HEADER_VIEW
#define HEADER_VIEW

void initView(int argc, char *argv[]);

void showInfoBanner(int channelNumber, int aPID, int vPID, int txt);
void showVolumeGraph(int volume);
void hiddeInfoBanner();
void hiddeVolumeGraph();
void clearScreen();

int isInfoBannerShowM();

void deintiView();

#endif