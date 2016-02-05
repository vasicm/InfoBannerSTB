#include "view.h"

#include <stdio.h>
#include <directfb.h>
#include <stdint.h>
#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <signal.h>
#include <time.h>
#include "timer.h"

/* helper macro for error checking */
#define DFBCHECK(x...)                                      \
{                                                           \
DFBResult err = x;                                          \
                                                            \
if (err != DFB_OK)                                          \
  {                                                         \
    fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );  \
    DirectFBErrorFatal( #x, err );                          \
  }                                                         \
}

// #define dbg printf("\nfunction:%s(%d)\n",__FUNCTION__,__LINE__)


static IDirectFBSurface *primary = NULL;
IDirectFB *dfbInterface = NULL;
static int screenWidth = 0;
static int screenHeight = 0;
DFBSurfaceDescription surfaceDesc;

static int infoBannerXCor;
static int infoBannerYCor;
static int infoBannerWidth;
static int infoBannerHeight;

static int imgHeight;
static int imgWidth;
static int padding = 50;

static int infoBannerShowFlag = 0;
DFBRegion infoBannerRegion;
DFBRegion volumeRegion;

static Timer infoBannerTimer;
static Timer volumeGraphTimer;

void initView(int argc, char *argv[]) 
{
	DFBResult err;

	/* initialize DirectFB */
    
	DFBCHECK(DirectFBInit(&argc, &argv));
    /* fetch the DirectFB interface */
	DFBCHECK(DirectFBCreate(&dfbInterface));
    /* tell the DirectFB to take the full screen for this application */
	DFBCHECK(dfbInterface->SetCooperativeLevel(dfbInterface, DFSCL_FULLSCREEN));
	 
    /* create primary surface with double buffering enabled */
	surfaceDesc.flags = DSDESC_CAPS;
	surfaceDesc.caps = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
	DFBCHECK (dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &primary));
    
    /* fetch the screen size */
    DFBCHECK (primary->GetSize(primary, &screenWidth, &screenHeight));

    infoBannerXCor = screenWidth / 10;
	infoBannerYCor = screenHeight * 7 / 10;
	infoBannerWidth = screenWidth * 16 / 20;
	infoBannerHeight = screenHeight * 2 / 10;

	infoBannerRegion.x1 = infoBannerXCor;
	infoBannerRegion.y1 = infoBannerYCor;
	infoBannerRegion.x2 = infoBannerXCor + infoBannerWidth;
	infoBannerRegion.y2 = infoBannerYCor + infoBannerHeight;

	volumeRegion.x1 = padding;
	volumeRegion.y1 = padding;

	initTimer(&volumeGraphTimer, hiddeVolumeGraph, 3);
	initTimer(&infoBannerTimer, hiddeInfoBanner, 3);
}

void clearScreen()
{
	/* clear the screen before drawing anything (draw black full screen rectangle)*/
    DFBCHECK(primary->SetColor(primary, 0x00, 0x00, 0x00, 0x00));
	DFBCHECK(primary->FillRectangle(primary, 0, 0, screenWidth, screenHeight));

	/* switch between the displayed and the work buffer (update the display) */
	DFBCHECK(primary->Flip(primary, NULL, 0));
}

void hiddeInfoBanner()
{
	infoBannerShowFlag = 0;
	primary->Flip(primary,
		/*region to be updated, NULL for the whole surface*/NULL,
		/*flip flags*/0);
	DFBCHECK(primary->SetColor(primary, 0x00, 0x00, 0x00, 0x00));
	DFBCHECK(primary->FillRectangle(
		primary, 
		infoBannerXCor, 
		infoBannerYCor, 
		infoBannerWidth, 
		infoBannerHeight
		)
	);
	DFBCHECK(primary->Flip(primary, &infoBannerRegion, 0));
}

int isInfoBannerShowM()
{
	return infoBannerShowFlag;
}

void showInfoBanner(int channelNumber, int aPID, int vPID, int txt)
{

	infoBannerShowFlag = 1;
	primary->Flip(primary,
		/*region to be updated, NULL for the whole surface*/NULL,
		/*flip flags*/0);
	DFBCHECK(primary->SetColor(primary, 0x00, 0xff, 0x99, 0xff));
	DFBCHECK(primary->FillRectangle(
		primary, 
		infoBannerXCor, 
		infoBannerYCor, 
		infoBannerWidth, 
		infoBannerHeight
		)
	);

	IDirectFBFont *fontInterface = NULL;
	DFBFontDescription fontDesc;
	
    /* specify the height of the font by raising the appropriate flag and setting the height value */
	fontDesc.flags = DFDESC_HEIGHT;
	fontDesc.height = infoBannerHeight * 3 / 12;
	//fontDesc.height = 48;

    /* create the font and set the created font for primary surface text drawing */
	DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSerif-BoldOblique.ttf", &fontDesc, &fontInterface));
	DFBCHECK(primary->SetFont(primary, fontInterface));
    
    /* draw the text */	
	DFBCHECK(primary->SetColor(primary, 0xff, 0xff, 0xff, 0xff));

	char textToDraw[20];

	sprintf(textToDraw,"Channel %d",channelNumber);
	DFBCHECK(primary->DrawString(primary,
		textToDraw,
		-1,
		(infoBannerXCor + infoBannerWidth / 20),
		(infoBannerYCor + infoBannerHeight / 3),
		DSTF_LEFT)
	);

	fontDesc.height = infoBannerHeight / 6;
	//fontDesc.height = 20;

	DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSerif-BoldOblique.ttf", &fontDesc, &fontInterface));
	DFBCHECK(primary->SetFont(primary, fontInterface));

	sprintf(textToDraw,"Audio PID %d",aPID);
	DFBCHECK(primary->DrawString(primary,
		textToDraw,
		-1,
		(infoBannerXCor + infoBannerWidth / 10),
		(infoBannerYCor + infoBannerHeight * 2 / 3),
		DSTF_LEFT)
	);

	sprintf(textToDraw,"Video PID %d",vPID);
	DFBCHECK(primary->DrawString(primary,
		textToDraw,
		-1,
		(infoBannerXCor + infoBannerWidth / 10),
		(infoBannerYCor + infoBannerHeight * 11 / 12),
		DSTF_LEFT)
	);

	if(txt != 0) {
		sprintf(textToDraw,"TXT");
		DFBCHECK(primary->DrawString(primary,
			textToDraw,
			-1,
			(infoBannerXCor + infoBannerWidth * 16 / 20),
			(infoBannerYCor + infoBannerHeight / 3),
			DSTF_LEFT)
		);
	}
    primary->Flip(primary, &infoBannerRegion, 0);
	fontInterface->Release(fontInterface);

	startTimer(&infoBannerTimer);
}

void showVolumeGraph(int volume)
{
	if (volume < 0 || volume > 10) {
		return;
	}
	IDirectFBImageProvider *provider;
	IDirectFBSurface *volumeSurface = NULL;

	char filename[15];
	sprintf(filename,"volume_%d.png",volume);
    /* create the image provider for the specified file */
	DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, filename, &provider));
    /* get surface descriptor for the surface where the image will be rendered */
	DFBCHECK(provider->GetSurfaceDescription(provider, &surfaceDesc));
    /* create the surface for the image */
	DFBCHECK(dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &volumeSurface));
    /* render the image to the surface */
	DFBCHECK(provider->RenderTo(provider, volumeSurface, NULL));

    /* cleanup the provider after rendering the image to the surface */
	provider->Release(provider);

    /* fetch the logo size and add (blit) it to the screen */
	DFBCHECK(volumeSurface->GetSize(volumeSurface, &imgWidth, &imgHeight));

	volumeRegion.x2 = padding + imgWidth;
	volumeRegion.y2 = padding + imgHeight;

	DFBCHECK(primary->Blit(primary,
		/*source surface*/ volumeSurface,
		/*source region, NULL to blit the whole surface*/ NULL,
		/*destination x coordinate of the upper left corner of the image*/padding,
		/*destination y coordinate of the upper left corner of the image*/padding));

	DFBCHECK(primary->Flip(primary, &volumeRegion, 0));
	startTimer(&volumeGraphTimer);

}

void hiddeVolumeGraph()
{
	DFBCHECK(primary->SetColor(primary, 0x00, 0x00, 0x00, 0x00));
	DFBCHECK(primary->FillRectangle(
		primary, 
		padding, 
		padding, 
		imgWidth, 
		imgHeight
		)
	);
	DFBCHECK(primary->Flip(primary, &volumeRegion, 0));
}

void deintiView()
{
	removeTimer(&volumeGraphTimer);
	removeTimer(&infoBannerTimer);
}