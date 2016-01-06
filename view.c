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
	DFBCHECK(primary->SetColor(primary, 0x00, 0x00, 0x00, 0x00));
	DFBCHECK(primary->FillRectangle(
		primary, 
		infoBannerXCor, 
		infoBannerYCor, 
		infoBannerWidth, 
		infoBannerHeight
		)
	);
	DFBCHECK(primary->Flip(primary, NULL, 0));
}

void showInfoBanner(int channelNumber, int aPID, int vPID, int txt)
{

	DFBCHECK(primary->SetColor(primary, 0x00, 0xff, 0x00, 0xff));
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
	fontDesc.height = 48;

    /* create the font and set the created font for primary surface text drawing */
	DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
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
	fontDesc.height = 20;

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

	sprintf(textToDraw,"TXT");
	DFBCHECK(primary->DrawString(primary,
		textToDraw,
		-1,
		(infoBannerXCor + infoBannerWidth * 16 / 20),
		(infoBannerYCor + infoBannerHeight / 3),
		DSTF_LEFT)
	);

    primary->Flip(primary,
		/*region to be updated, NULL for the whole surface*/NULL,
		/*flip flags*/0);
	fontInterface->Release(fontInterface);
}

void showVolumeGraph(int volume)
{
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
	DFBCHECK(primary->Blit(primary,
		/*source surface*/ volumeSurface,
		/*source region, NULL to blit the whole surface*/ NULL,
		/*destination x coordinate of the upper left corner of the image*/padding,
		/*destination y coordinate of the upper left corner of the image*/(imgHeight + padding));

	DFBCHECK(primary->Flip(primary,
		/*region to be updated, NULL for the whole surface*/NULL,
		/*flip flags*/0)
	);
}

void hiddeVolumeGraph()
{
	DFBCHECK(primary->SetColor(primary, 0x00, 0x00, 0x00, 0x00));
	DFBCHECK(primary->FillRectangle(
		primary, 
		padding, 
		(imgHeight + padding), 
		imgWidth, 
		imgHeight
		)
	);
	DFBCHECK(primary->Flip(primary, NULL, 0));
}

void deintiView()
{

}