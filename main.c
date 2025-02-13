/*
 *      oneko  -  X11 猫
 */

#include "oneko.h"
#include "patchlevel.h"

#include "animal.h"

Animal* animals[20] = {0};
int animal_count = 0;
Animal* animal;

/*
 *      グローバル変数
 */

char* ClassName = "Oneko";           /* コマンド名称 */
char* ProgramName;                   /* コマンド名称 */

Display* D;                    /* ディスプレイ構造体 */
int     theScreen;                      /* スクリーン番号 */
unsigned int    theDepth;               /* デプス */
Window  theRoot;                        /* ルートウィンドウのＩＤ */
Window  theWindow;                      /* 猫ウィンドウのＩＤ */
char* WindowName = NULL;             /* 猫ウィンドウの名前 */
Window  theTarget = None;               /* 目標ウィンドウのＩＤ */
char* TargetName = NULL;             /* 目標ウィンドウの名前 */

unsigned int WindowWidth;            /* ルートウィンドウの幅 */
unsigned int WindowHeight;           /* ルートウィンドウの高さ */

XColor  theForegroundColor;             /* 色 (フォアグラウンド) */
XColor  theBackgroundColor;             /* 色 (バックグラウンド) */

int Synchronous = False;

/*
 *      いろいろな初期設定 (オプション、リソースで変えられるよ)
 */

/* Resource:    */
char* Foreground = NULL;             /*   foreground */
char* Background = NULL;             /*   background */
long    IntervalTime = 0L;              /*   time       */
double  NekoSpeed = 0.0;          /*   speed      */
int     IdleSpace = 0;                  /*   idle       */
int     NoShape = NOTDEFINED;           /*   noshape    */
int     ReverseVideo = NOTDEFINED;      /*   reverse    */
int     ToWindow = NOTDEFINED;          /*   towindow   */
int     ToFocus = NOTDEFINED;           /*   tofocus    */
int     XOffset=0,YOffset=0;            /* X and Y offsets for cat from mouse
                                           pointer. */
/*
 *      いろいろな状態変数
 */

Bool    DontMapped = True;

int     NekoTickCount;          /* 猫動作カウンタ */
int     NekoStateCount;         /* 猫同一状態カウンタ */
int     NekoState;              /* 猫の状態 */

int     MouseX;                 /* マウスＸ座標 */
int     MouseY;                 /* マウスＹ座標 */

int     PrevMouseX = 0;         /* 直前のマウスＸ座標 */
int     PrevMouseY = 0;         /* 直前のマウスＹ座標 */
Window  PrevTarget = None;      /* 直前の目標ウィンドウのＩＤ */

int     NekoX;                  /* 猫Ｘ座標 */
int     NekoY;                  /* 猫Ｙ座標 */

int     NekoMoveDx;             /* 猫移動距離Ｘ */
int     NekoMoveDy;             /* 猫移動距離Ｙ */

int     NekoLastX;              /* 猫最終描画Ｘ座標 */
int     NekoLastY;              /* 猫最終描画Ｙ座標 */
GC      NekoLastGC;             /* 猫最終描画 GC */
/* Variables used to set how quickly the program will chose to raise itself. */
/* Look at Interval(), Handle Visiblility Notify Event */
#define DEFAULT_RAISE_WAIT 16  /* About 2 seconds with default interval */
int     RaiseWindowDelay=0;
/*
 *      その他
 */

double  SinPiPer8Times3;        /* sin(３π／８) */
double  SinPiPer8;              /* sin(π／８) */

typedef struct BitmapGCData {
	GC gc;
	Pixmap bitmap;
	Pixmap mask;
} BitmapGCData;

BitmapGCData frames[FRAME_COUNT];

typedef struct {
	GC* TickGCPtr;
	Pixmap* TickMaskPtr;
} Animation;

int AnimationPattern[][2] = {
	{0, 0}, // NEKO_STOP
	{1, 0}, // NEKO_JARE
	{2, 3}, // NEKO_KAKI
	{4, 4}, // NEKO_AKUBI
	{5, 6}, // NEKO_SLEEP
	{7, 7}, // NEKO_AWAKE
	// move
	{8, 9}, {10, 11}, {12, 13}, {14, 15},
	// diagonal
	{16, 17}, {18, 19}, {20, 21}, {22, 23},
	// edge
	{24, 25}, {26, 27}, {28, 29}, {30, 31},
};

static void NullFunction();

/*
* ビットマップデータ・GC 初期化
 */

void InitBitmapAndGCs() {
	XGCValues theGCValues = {
		.function = GXcopy,
		.foreground = theForegroundColor.pixel,
		.background = theBackgroundColor.pixel,
		.fill_style = FillTiled,
		.ts_x_origin = 0,
		.ts_y_origin = 0,
	};
	
	for (int i=0; i<FRAME_COUNT; i++) {
		BitmapGCData* b = &frames[i];
		
		b->bitmap = XCreatePixmapFromBitmapData(
			D, theRoot,
			(char*)&animal->bitmaps[i], BITMAP_WIDTH, BITMAP_HEIGHT,
			theForegroundColor.pixel,
			theBackgroundColor.pixel,
			DefaultDepth(D, theScreen));
		b->mask = XCreateBitmapFromData(
			D, theRoot,
			(char*)&animal->masks[i], BITMAP_WIDTH, BITMAP_HEIGHT);
		theGCValues.tile = b->bitmap;
		b->gc = XCreateGC(
			D, theWindow,
			GCFunction | GCForeground | GCBackground | GCTile |
			GCTileStipXOrigin | GCTileStipYOrigin | GCFillStyle,
			&theGCValues);
	}
}

/*
 *      リソース・データベースから必要なリソースを取り出す
 */

char* NekoGetDefault(char* resource) {
	char* value;

	if (value = XGetDefault(D, ProgramName, resource)) {
		return value;
	}
	if (value = XGetDefault(D, ClassName, resource)) {
		return value;
	}
	return NULL;
}

/*
 *      リソース・データベースからオプションを設定
 */

void GetResources() {
	char* resource;
	int           num;
	int loop;
	if (Foreground == NULL) {
		if ((resource = NekoGetDefault("foreground")) != NULL) {
			Foreground = resource;
		}
	}

	if (Background == NULL) {
		if ((resource = NekoGetDefault("background")) != NULL) {
			Background = resource;
		}
	}

	if (IntervalTime == 0) {
		if ((resource = NekoGetDefault("time")) != NULL) {
			if (num = atoi(resource)) {
				IntervalTime = num;
			}
		}
	}

	if (NekoSpeed == (double)0) {
		if ((resource = NekoGetDefault("speed")) != NULL) {
			if (num = atoi(resource)) {
				NekoSpeed = (double)num;
			}
		}
	}

	if (IdleSpace == 0) {
		if ((resource = NekoGetDefault("idle")) != NULL) {
			if (num = atoi(resource)) {
				IdleSpace = num;
			}
		}
	}

	if (!animal) {
		for (loop=0;loop<animal_count;loop++)
			if ((resource = NekoGetDefault(animals[loop]->name)) != NULL) {
				if (IsTrue(resource))
					animal = animals[loop];
			}
	}

	if (NoShape == NOTDEFINED) {
		if ((resource = NekoGetDefault("noshape")) != NULL) {
			NoShape = IsTrue(resource);
		}
	}

	if (ReverseVideo == NOTDEFINED) {
		if ((resource = NekoGetDefault("reverse")) != NULL) {
			ReverseVideo = IsTrue(resource);
		}
	}

	if (Foreground == NULL) {
		Foreground = DEFAULT_FOREGROUND;
	}
	if (Background == NULL) {
		Background = DEFAULT_BACKGROUND;
	}
	if (!animal) {
		animal = animals[0];
	}
	if (IntervalTime == 0) {
		IntervalTime = animal->time;
	}
	if (NekoSpeed == 0) {
		NekoSpeed = animal->speed;
	}
	if (IdleSpace == 0) {
		IdleSpace = animal->idle;
	}
	XOffset = XOffset + animal->off_x;
	YOffset = YOffset + animal->off_y;
	if (NoShape == NOTDEFINED) {
		NoShape = False;
	}
	if (ReverseVideo == NOTDEFINED) {
		ReverseVideo = False;
	}
	if (ToWindow == NOTDEFINED) {
		ToWindow = False;
	}
	if (ToFocus == NOTDEFINED) {
		ToFocus = False;
	}
}

/*
 *      色を初期設定する
 */

void SetupColors() {
	XColor theExactColor;
	Colormap theColormap;

	theColormap = DefaultColormap(D, theScreen);

	if (theDepth == 1) {
		Foreground = "black";
		Background = "white";
	}

	if (ReverseVideo == True) {
		char* tmp = Foreground;
		Foreground = Background;
		Background = tmp;
	}

	if (!XAllocNamedColor(D, theColormap,
	                      Foreground, &theForegroundColor, &theExactColor)) {
		fprintf(stderr, "%s: Can't XAllocNamedColor(\"%s\").\n",
		        ProgramName, Foreground);
		exit(1);
	}

	if (!XAllocNamedColor(D, theColormap,
	                      Background, &theBackgroundColor, &theExactColor)) {
		fprintf(stderr, "%s: Can't XAllocNamedColor(\"%s\").\n",
		        ProgramName, Background);
		exit(1);
	}
}

/*
 * Routine to let user select a window using the mouse
 *
 * This routine originate in dsimple.c
 */

Window SelectWindow(Display* dpy) {
	int status;
	XEvent event;
	Window target_win = None, root = theRoot;
	int buttons = 0;

	/* Grab the pointer using target cursor, letting it room all over */
	/*status = XGrabPointer(dpy, root, False,
	  ButtonPressMask|ButtonReleaseMask, GrabModeSync,
	  GrabModeAsync, root, cursor, CurrentTime);
	  if (status != GrabSuccess) {
	  fprintf(stderr, "%s: Can't grab the mouse.\n", ProgramName);
	  exit(1);
	  }*/

	/* Let the user select a window... */
	while ((target_win == None) || (buttons != 0)) {
		/* allow one more event */
		XAllowEvents(dpy, SyncPointer, CurrentTime);
		XWindowEvent(dpy, root, ButtonPressMask|ButtonReleaseMask, &event);
		switch (event.type) {
		case ButtonPress:
			if (target_win == None) {
				target_win = event.xbutton.subwindow; /* window selected */
				if (target_win == None) target_win = root;
			}
			buttons++;
			break;    case ButtonRelease:
			if (buttons > 0) /* there may have been some down before we started */
				buttons--;
			break;
		}
	}

	XUngrabPointer(dpy, CurrentTime);      /* Done with pointer */

	return(target_win);
}

/*
 * WindowWithName: routine to locate a window with a given name on a display.
 *                 If no window with the given name is found, 0 is returned.
 *                 If more than one window has the given name, the first
 *                 one found will be returned.  Only top and its subwindows
 *                 are looked at.  Normally, top should be the RootWindow.
 *
 * This routine originate in dsimple.c
 */
Window WindowWithName(Display* dpy, Window top, char* name) {
	Window* children, dummy;
	unsigned int nchildren;
	int i;
	Window w=0;
	char* window_name;

	if (XFetchName(dpy, top, &window_name) && !strcmp(window_name, name))
		return(top);

	if (!XQueryTree(dpy, top, &dummy, &dummy, &children, &nchildren))
		return(0);

	for (i=0; i<nchildren; i++) {
		w = WindowWithName(dpy, children[i], name);
		if (w)
			break;
	}
	if (children) XFree ((char* )children);
	return(w);
}

/*
 *      スクリーン環境初期化
 */

void InitScreen(char* DisplayName) {
	XSetWindowAttributes  theWindowAttributes;
	unsigned long         theWindowMask;
	Window                        theTempRoot;
	int                           WindowPointX;
	int                           WindowPointY;
	unsigned int          BorderWidth;
	int                           event_base, error_base;

	if ((D = XOpenDisplay(DisplayName)) == NULL) {
		fprintf(stderr, "%s: Can't open display", ProgramName);
		if (DisplayName != NULL) {
			fprintf(stderr, " %s.\n", DisplayName);
		} else {
			fprintf(stderr, ".\n");
		}
		exit(1);
	}

	GetResources();

	if (Synchronous == True) {
		fprintf(stderr,"Synchronizing.\n");
		XSynchronize(D,True);
	}

	if (!NoShape && XShapeQueryExtension(D,
	                                     &event_base, &error_base) == False) {
		fprintf(stderr, "Display not suported shape extension.\n");
		NoShape = True;
	}

	theScreen = DefaultScreen(D);
	theDepth = DefaultDepth(D, theScreen);

	theRoot = RootWindow(D, theScreen);

	XGetGeometry(D, theRoot, &theTempRoot,
	             &WindowPointX, &WindowPointY,
	             &WindowWidth, &WindowHeight,
	             &BorderWidth, &theDepth);

	SetupColors();

	if (ToWindow && theTarget == None) {
		if (TargetName != NULL) {
			int i;

			for (i=0; i<5; i++) {
				theTarget = WindowWithName(D, theRoot, TargetName);
				if (theTarget != None) break;
			}
			if (theTarget == None) {
				fprintf(stderr, "%s: No window with name '%s' exists.\n",
				        ProgramName, TargetName);
				exit(1);
			}
		} else {
			theTarget = SelectWindow(D);
			if (theTarget == theRoot) {
				theTarget = None;
				ToWindow = False;
			}
		}
		if (theTarget != None) {
			Window            QueryRoot, QueryParent, *QueryChildren;
			unsigned int      nchild;

			for (;;) {
				if (XQueryTree(D, theTarget, &QueryRoot,
				               &QueryParent, &QueryChildren, &nchild)) {
					XFree(QueryChildren);
					if (QueryParent == QueryRoot) break;
					theTarget = QueryParent;
				}
				else {
					fprintf(stderr, "%s: Target Lost.\n",ProgramName);
					exit(1);
				}
			}
		}
	}

	theWindowAttributes.background_pixel = theBackgroundColor.pixel;
	theWindowAttributes.override_redirect = True;

	if (!ToWindow) XChangeWindowAttributes(D, theRoot, 0, &theWindowAttributes);
	
	// from what i've read,
	// `override_redirect`=true tells the window manager to "leave the window alone"
	// that way it remains on top of other windows
	// and won't show up in taskbars etc.
	theWindowMask = CWBackPixel | CWOverrideRedirect;

	theWindow = XCreateWindow(D, theRoot, 0, 0,
	                          BITMAP_WIDTH, BITMAP_HEIGHT,
	                          0, theDepth, InputOutput, CopyFromParent,
	                          theWindowMask, &theWindowAttributes);

	XserverRegion region = XFixesCreateRegion(D, &(XRectangle){0,0,0,0}, 1);
	XFixesSetWindowShapeRegion(D, theWindow, ShapeInput, 0, 0, region);
	XFixesDestroyRegion(D, region);
  
	if (WindowName == NULL) WindowName = ProgramName;
	XStoreName(D, theWindow, WindowName);

	InitBitmapAndGCs();

	XSelectInput(D, theWindow,
	             ExposureMask|VisibilityChangeMask|KeyPressMask);

	XFlush(D);
}


/*
 *      SIGINT シグナル処理
 */

void RestoreCursor() {
	for (int i=0; i<FRAME_COUNT; i++) {
		BitmapGCData* b = &frames[i];
		XFreePixmap(D, b->bitmap);
		XFreePixmap(D, b->mask);
		XFreeGC(D, b->gc);
	}
	XCloseDisplay(D);
	exit(0);
}


/*
 *      インターバル
 *
 *      　この関数を呼ぶと、ある一定の時間返ってこなくなる。猫
 *      の動作タイミング調整に利用すること。
 */

void Interval() {
	pause();
	if (RaiseWindowDelay>0)
		RaiseWindowDelay--;
}


/*
 *      ティックカウント処理
 */

void TickCount() {
	if (++NekoTickCount >= MAX_TICK) {
		NekoTickCount = 0;
	}

	if (NekoTickCount % 2 == 0) {
		if (NekoStateCount < MAX_TICK) {
			NekoStateCount++;
		}
	}
}


/*
 *      猫状態設定
 */

void SetNekoState(int SetValue) {
	NekoTickCount = 0;
	NekoStateCount = 0;

	NekoState = SetValue;
}


/*
 *      猫描画処理
 */

void DrawNeko(int x, int y, int DrawAnime) {
	GC DrawGC = frames[DrawAnime].gc;
	Pixmap DrawMask = frames[DrawAnime].mask;
	
	if ((x != NekoLastX) || (y != NekoLastY)
	    || (DrawGC != NekoLastGC)) {
		XWindowChanges theChanges;
		
		theChanges.x = x;
		theChanges.y = y;
		XConfigureWindow(D, theWindow, CWX | CWY, &theChanges);
		if (NoShape == False) {
			XShapeCombineMask(D, theWindow, ShapeBounding,
			                  0, 0, DrawMask, ShapeSet);

		}
		if (DontMapped) {
			XMapWindow(D, theWindow);
			DontMapped = 0;
		}
		XFillRectangle(D, theWindow, DrawGC,
		               0, 0, BITMAP_WIDTH, BITMAP_HEIGHT);
	}

	XFlush(D);
	
	NekoLastX = x;
	NekoLastY = y;

	NekoLastGC = DrawGC;
}


/*
 *      猫再描画処理
 */

void RedrawNeko() {
	XFillRectangle(D, theWindow, NekoLastGC,
	               0, 0, BITMAP_WIDTH, BITMAP_HEIGHT);

	XFlush(D);
}


/*
 *      猫移動方法決定
 *
 *      This sets the direction that the neko is moving.
 *
 */

void NekoDirection() {
	int                 NewState;
	double              LargeX, LargeY;
	double              Length;
	double              SinTheta;

	if (NekoMoveDx == 0 && NekoMoveDy == 0) {
		NewState = NEKO_STOP;
	} else {
		LargeX = (double)NekoMoveDx;
		LargeY = (double)(-NekoMoveDy);
		Length = sqrt(LargeX * LargeX + LargeY * LargeY);
		SinTheta = LargeY / Length;

		if (NekoMoveDx > 0) {
			if (SinTheta > SinPiPer8Times3) {
				NewState = NEKO_U_MOVE;
			} else if ((SinTheta <= SinPiPer8Times3)
			           && (SinTheta > SinPiPer8)) {
				NewState = NEKO_UR_MOVE;
			} else if ((SinTheta <= SinPiPer8)
			           && (SinTheta > -(SinPiPer8))) {
				NewState = NEKO_R_MOVE;
			} else if ((SinTheta <= -(SinPiPer8))
			           && (SinTheta > -(SinPiPer8Times3))) {
				NewState = NEKO_DR_MOVE;
			} else {
				NewState = NEKO_D_MOVE;
			}
		} else {
			if (SinTheta > SinPiPer8Times3) {
				NewState = NEKO_U_MOVE;
			} else if ((SinTheta <= SinPiPer8Times3)
			           && (SinTheta > SinPiPer8)) {
				NewState = NEKO_UL_MOVE;
			} else if ((SinTheta <= SinPiPer8)
			           && (SinTheta > -(SinPiPer8))) {
				NewState = NEKO_L_MOVE;
			} else if ((SinTheta <= -(SinPiPer8))
			           && (SinTheta > -(SinPiPer8Times3))) {
				NewState = NEKO_DL_MOVE;
			} else {
				NewState = NEKO_D_MOVE;
			}
		}
	}

	if (NekoState != NewState) {
		SetNekoState(NewState);
	}
}


/*
 *      猫壁ぶつかり判定
 */

Bool IsWindowOver() {
	Bool        ReturnValue = False;

	if (NekoY <= 0) {
		NekoY = 0;
		ReturnValue = True;
	} else if (NekoY >= WindowHeight - BITMAP_HEIGHT) {
		NekoY = WindowHeight - BITMAP_HEIGHT;
		ReturnValue = True;
	}
	if (NekoX <= 0) {
		NekoX = 0;
		ReturnValue = True;
	} else if (NekoX >= WindowWidth - BITMAP_WIDTH) {
		NekoX = WindowWidth - BITMAP_WIDTH;
		ReturnValue = True;
	}

	return(ReturnValue);
}


/*
 *      猫移動状況判定
 */

Bool IsNekoDontMove() {
	if (NekoX == NekoLastX && NekoY == NekoLastY) {
		return(True);
	} else {
		return(False);
	}
}


/*
 *      猫移動開始判定
 */

Bool IsNekoMoveStart() {
	if ((PrevMouseX >= MouseX - IdleSpace
	     && PrevMouseX <= MouseX + IdleSpace) &&
	    (PrevMouseY >= MouseY - IdleSpace
	     && PrevMouseY <= MouseY + IdleSpace) &&
	    (PrevTarget == theTarget)) {
		return(False);
	} else {
		return(True);
	}
}


/*
 *      猫移動 dx, dy 計算
 */

void CalcDxDy() {
	Window              QueryRoot, QueryChild;
	int                 AbsoluteX, AbsoluteY;
	int                 RelativeX, RelativeY;
	unsigned int        ModKeyMask;
	double              LargeX, LargeY;
	double              DoubleLength, Length;

	XQueryPointer(D, theWindow,
	              &QueryRoot, &QueryChild,
	              &AbsoluteX, &AbsoluteY,
	              &RelativeX, &RelativeY,
	              &ModKeyMask);

	PrevMouseX = MouseX;
	PrevMouseY = MouseY;
	PrevTarget = theTarget;

	MouseX = AbsoluteX+XOffset;
	MouseY = AbsoluteY+YOffset;

	if (ToFocus) {
		int               revert;

		XGetInputFocus(D, &theTarget, &revert);

		if (theTarget != theRoot
		    && theTarget != PointerRoot && theTarget != None) {
			Window          QueryParent, *QueryChildren;
			unsigned int    nchild;

			for (;;) {
				if (XQueryTree(D, theTarget, &QueryRoot,
				               &QueryParent, &QueryChildren, &nchild)) {
					XFree(QueryChildren);
					if (QueryParent == QueryRoot) break;
					theTarget = QueryParent;
				}
				else {
					theTarget = None;
					break;
				}
			}
		}
		else {
			theTarget = None;
		}
	}

	if ((ToWindow || ToFocus) && theTarget != None) {
		int                       status;
		XWindowAttributes         theTargetAttributes;

		status =
			XGetWindowAttributes(D, theTarget, &theTargetAttributes);

		if (ToWindow && status == 0) {
			fprintf(stderr, "%s: '%s', Target Lost.\n",ProgramName, WindowName);
			RestoreCursor();
		}

		if (theTargetAttributes.x+theTargetAttributes.width > 0
		    && theTargetAttributes.x < (int)WindowWidth
		    && theTargetAttributes.y+theTargetAttributes.height > 0
		    && theTargetAttributes.y < (int)WindowHeight
		    && theTargetAttributes.map_state == IsViewable) {
			if (ToFocus) {
				if (MouseX < theTargetAttributes.x+BITMAP_WIDTH/2)
					LargeX = (double)(theTargetAttributes.x + XOffset - NekoX);
				else if (MouseX > theTargetAttributes.x+theTargetAttributes.width
				         -BITMAP_WIDTH/2)
					LargeX = (double)(theTargetAttributes.x + theTargetAttributes.width
					                  + XOffset - NekoX - BITMAP_WIDTH);
				else
					LargeX = (double)(MouseX - NekoX - BITMAP_WIDTH / 2);

				LargeY = (double)(theTargetAttributes.y
				                  + YOffset - NekoY - BITMAP_HEIGHT);
			}
			else {
				MouseX = theTargetAttributes.x
					+ theTargetAttributes.width / 2 + XOffset;
				MouseY = theTargetAttributes.y + YOffset;
				LargeX = (double)(MouseX - NekoX - BITMAP_WIDTH / 2);
				LargeY = (double)(MouseY - NekoY - BITMAP_HEIGHT);
			}
		}
		else {
			LargeX = (double)(MouseX - NekoX - BITMAP_WIDTH / 2);
			LargeY = (double)(MouseY - NekoY - BITMAP_HEIGHT);
		}
	}
	else {
		LargeX = (double)(MouseX - NekoX - BITMAP_WIDTH / 2);
		LargeY = (double)(MouseY - NekoY - BITMAP_HEIGHT);
	}

	DoubleLength = LargeX * LargeX + LargeY * LargeY;

	if (DoubleLength != (double)0) {
		Length = sqrt(DoubleLength);
		if (Length <= NekoSpeed) {
			NekoMoveDx = (int)LargeX;
			NekoMoveDy = (int)LargeY;
		} else {
			NekoMoveDx = (int)((NekoSpeed * LargeX) / Length);
			NekoMoveDy = (int)((NekoSpeed * LargeY) / Length);
		}
	} else {
		NekoMoveDx = NekoMoveDy = 0;
	}
}


/*
 *      動作解析猫描画処理
 */

void NekoThinkDraw() {
	CalcDxDy();

	
	if (NekoState != NEKO_SLEEP) {
		DrawNeko(NekoX, NekoY,
		         AnimationPattern[NekoState][NekoTickCount & 0x1]);
	} else {
		DrawNeko(NekoX, NekoY,
		         AnimationPattern[NekoState][(NekoTickCount >> 2) & 0x1]);
	}

	TickCount();

	switch (NekoState) {
	case NEKO_STOP:
		if (IsNekoMoveStart()) {
			SetNekoState(NEKO_AWAKE);
			break;
		}
		if (NekoStateCount < NEKO_STOP_TIME) {
			break;
		}
		if (NekoMoveDx < 0 && NekoX <= 0) {
			SetNekoState(NEKO_L_TOGI);
		} else if (NekoMoveDx > 0 && NekoX >= WindowWidth - BITMAP_WIDTH) {
			SetNekoState(NEKO_R_TOGI);
		} else if ((NekoMoveDy < 0 && NekoY <= 0)
		           || (ToFocus && theTarget != None && NekoY > MouseY)){
			SetNekoState(NEKO_U_TOGI);
		} else if ((NekoMoveDy > 0 && NekoY >= WindowHeight - BITMAP_HEIGHT)
		           || (ToFocus && theTarget != None
		               &&  NekoY < MouseY - BITMAP_HEIGHT)){
			SetNekoState(NEKO_D_TOGI);
		} else {
			SetNekoState(NEKO_JARE);
		}
		break;
	case NEKO_JARE:
		if (IsNekoMoveStart()) {
			SetNekoState(NEKO_AWAKE);
			break;
		}
		if (NekoStateCount < NEKO_JARE_TIME) {
			break;
		}
		SetNekoState(NEKO_KAKI);
		break;
	case NEKO_KAKI:
		if (IsNekoMoveStart()) {
			SetNekoState(NEKO_AWAKE);
			break;
		}
		if (NekoStateCount < NEKO_KAKI_TIME) {
			break;
		}
		SetNekoState(NEKO_AKUBI);
		break;
	case NEKO_AKUBI:
		if (IsNekoMoveStart()) {
			SetNekoState(NEKO_AWAKE);
			break;
		}
		if (NekoStateCount < NEKO_AKUBI_TIME) {
			break;
		}
		SetNekoState(NEKO_SLEEP);
		break;
	case NEKO_SLEEP:
		if (IsNekoMoveStart()) {
			SetNekoState(NEKO_AWAKE);
			break;
		}
		break;
	case NEKO_AWAKE:
		if (NekoStateCount < NEKO_AWAKE_TIME) {
			break;
		}
		NekoDirection();        /* 猫が動く向きを求める */
		break;
	case NEKO_U_MOVE:
	case NEKO_D_MOVE:
	case NEKO_L_MOVE:
	case NEKO_R_MOVE:
	case NEKO_UL_MOVE:
	case NEKO_UR_MOVE:
	case NEKO_DL_MOVE:
	case NEKO_DR_MOVE:
		NekoX += NekoMoveDx;
		NekoY += NekoMoveDy;
		NekoDirection();
		if (IsWindowOver()) {
			if (IsNekoDontMove()) {
				SetNekoState(NEKO_STOP);
			}
		}
		break;
	case NEKO_U_TOGI:
	case NEKO_D_TOGI:
	case NEKO_L_TOGI:
	case NEKO_R_TOGI:
		if (IsNekoMoveStart()) {
			SetNekoState(NEKO_AWAKE);
			break;
		}
		if (NekoStateCount < NEKO_TOGI_TIME) {
			break;
		}
		SetNekoState(NEKO_KAKI);
		break;
	default:
		/* Internal Error */
		SetNekoState(NEKO_STOP);
		break;
	}

	Interval();
}


/*
 *      キーイベント処理
 */

Bool ProcessKeyPress(XKeyEvent *theKeyEvent) {
	int                   Length;
	int                   theKeyBufferMaxLen = AVAIL_KEYBUF;
	char          theKeyBuffer[AVAIL_KEYBUF + 1];
	KeySym                theKeySym;
	XComposeStatus        theComposeStatus;
	Bool          ReturnState;

	ReturnState = True;

	Length = XLookupString(theKeyEvent,
	                       theKeyBuffer, theKeyBufferMaxLen,
	                       &theKeySym, &theComposeStatus);

	if (Length > 0) {
		switch (theKeyBuffer[0]) {
		case 'q':
		case 'Q':
			if (theKeyEvent->state & Mod1Mask) { /* META (Alt) キー */
				ReturnState = False;
			}
			break;
		default:
			break;
		}
	}

	return(ReturnState);
}


/*
 *      イベント処理
 */

Bool ProcessEvent() {
	XEvent      theEvent;
	Bool        ContinueState = True;

	while (XPending(D)) {
		XNextEvent(D,&theEvent);
		switch (theEvent.type) {
		case Expose:
			if (theEvent.xexpose.count == 0) {
				RedrawNeko();
			}
			break;
		case KeyPress:
			ContinueState = ProcessKeyPress(&theEvent.xkey);
			if (!ContinueState) {
				return(ContinueState);
			}
			break;
		case VisibilityNotify:
			if (RaiseWindowDelay==0) {
				XRaiseWindow(D,theWindow);
				RaiseWindowDelay=DEFAULT_RAISE_WAIT;
			}
		default:
			/* Unknown Event */
			break;
		}
	}

	return(ContinueState);
}


/*
 *      猫処理
 */

void ProcessNeko() {
	struct itimerval      Value;

	/* 猫の初期化 */

	NekoX = (WindowWidth - BITMAP_WIDTH / 2) / 2;
	NekoY = (WindowHeight - BITMAP_HEIGHT / 2) / 2;

	NekoLastX = NekoX;
	NekoLastY = NekoY;

	SetNekoState(NEKO_STOP);

	/* タイマー設定 */

	timerclear(&Value.it_interval);
	timerclear(&Value.it_value);

	Value.it_interval.tv_usec = IntervalTime;
	Value.it_value.tv_usec = IntervalTime;

	setitimer(ITIMER_REAL, &Value, 0);

	/* メイン処理 */

	do {
		NekoThinkDraw();
	} while (ProcessEvent());
}


/*
 *      SIGALRM シグナル処理
 */

static void NullFunction() {
	/* No Operation */
#if defined(SYSV) || defined(SVR4)
	signal(SIGALRM, NullFunction);
#endif /* SYSV || SVR4 */
}

/*
 *      エラー処理
 */

int NekoErrorHandler(Display *dpy, XErrorEvent *err) {
	if (err->error_code==BadWindow && (ToWindow || ToFocus)) {
		return 0;
	} else {
		char msg[80];
		XGetErrorText(dpy, err->error_code, msg, 80);
		fprintf(stderr, "%s: Error and exit.\n%s\n", ProgramName, msg);
		exit(1);
	}
}


/*
 *      Usage
 */

char    *message[] = {
	"",
	"Options are:",
	"-display <display>     : Neko appears on specified display.",
	"-fg <color>            : Foreground color",
	"-bg <color>            : Background color",
	"-speed <dots>",
	"-time <microseconds>",
	"-idle <dots>",
	"-name <name>           : set window name of neko.",
	"-towindow              : Neko chases selected window.",
	"-toname <name>         : Neko chases specified window.",
	"-tofocus               : Neko runs on top of focus window",
	"-rv                    : Reverse video. (effects monochrome display only)",
	"-position <geometry>   : adjust position relative to mouse pointer.",
	"-debug                 : puts you in synchronous mode.",
	"-patchlevel            : print out your current patchlevel.",
	NULL };

void Usage() {
	char  **mptr;
	int loop;

	mptr = message;
	fprintf(stderr, "Usage: %s [<options>]\n", ProgramName);
	while (*mptr) {
		fprintf(stderr,"%s\n", *mptr);
		mptr++;
	}
	for (loop=0;loop<animal_count;loop++)
		fprintf(stderr,"-%s Use %s bitmaps\n",animals[loop]->name, animals[loop]->name);
}


/*
 *      オプションの理解
 */

void GetArguments(int argc, char *argv[], char *theDisplayName) {
	int           ArgCounter;
	extern int XOffset,YOffset;
	int loop,found=0;

	theDisplayName[0] = '\0';

	for (ArgCounter = 0; ArgCounter < argc; ArgCounter++) {

		if (strncmp(argv[ArgCounter], "-h", 2) == 0) {
			Usage();
			exit(0);
		}
		if (strcmp(argv[ArgCounter], "-display") == 0) {
			ArgCounter++;
			if (ArgCounter < argc) {
				strcpy(theDisplayName, argv[ArgCounter]);
			} else {
				fprintf(stderr, "%s: -display option error.\n", ProgramName);
				exit(1);
			}
		}
		else if (strcmp(argv[ArgCounter], "-speed") == 0) {
			ArgCounter++;
			if (ArgCounter < argc) {
				NekoSpeed = atof(argv[ArgCounter]);
			} else {
				fprintf(stderr, "%s: -speed option error.\n", ProgramName);
				exit(1);
			}
		}
		else if (strcmp(argv[ArgCounter], "-time") == 0) {
			ArgCounter++;
			if (ArgCounter < argc) {
				IntervalTime = atol(argv[ArgCounter]);
			} else {
				fprintf(stderr, "%s: -time option error.\n", ProgramName);
				exit(1);
			}
		}
		else if (strcmp(argv[ArgCounter], "-idle") == 0) {
			ArgCounter++;
			if (ArgCounter < argc) {
				IdleSpace = atol(argv[ArgCounter]);
			} else {
				fprintf(stderr, "%s: -idle option error.\n", ProgramName);
				exit(1);
			}
		}
		else if (strcmp(argv[ArgCounter], "-name") == 0) {
			ArgCounter++;
			if (ArgCounter < argc) {
				WindowName = argv[ArgCounter];
			} else {
				fprintf(stderr, "%s: -name option error.\n", ProgramName);
				exit(1);
			}
		}
		else if (strcmp(argv[ArgCounter], "-towindow") == 0) {
			ToWindow = True;
			ToFocus = False;
		}
		else if (strcmp(argv[ArgCounter], "-toname") == 0) {
			ArgCounter++;
			if (ArgCounter < argc) {
				TargetName = argv[ArgCounter];
				ToWindow = True;
				ToFocus = False;
			} else {
				fprintf(stderr, "%s: -toname option error.\n", ProgramName);
				exit(1);
			}
		}
		else if (strcmp(argv[ArgCounter], "-tofocus") == 0) {
			ToFocus = True;
			ToWindow = False;
		}
		else if ((strcmp(argv[ArgCounter], "-fg") == 0) ||
		         (strcmp(argv[ArgCounter], "-foreground") == 0)) {
			ArgCounter++;
			Foreground = argv[ArgCounter];
		}
		else if ((strcmp(argv[ArgCounter], "-bg") == 0) ||
		         (strcmp(argv[ArgCounter], "-background") == 0)) {
			ArgCounter++;
			Background = argv[ArgCounter];
		}
		else if (strcmp(argv[ArgCounter], "-rv") == 0) {
			ReverseVideo = True;
		}
		else if (strcmp(argv[ArgCounter], "-noshape") == 0) {
			NoShape = True;
		}
		else if (strcmp(argv[ArgCounter], "-position") == 0) {
			ArgCounter++;
			unsigned int width, height;
			XParseGeometry(argv[ArgCounter],&XOffset,&YOffset,&width,&height);
			// FIXME: unused
		}
		else if (strcmp(argv[ArgCounter], "-debug") ==0) {
			Synchronous = True;
		}
		else if (strcmp(argv[ArgCounter], "-patchlevel") == 0) {
			fprintf(stderr,"Patchlevel :%s\n",PATCHLEVEL);
		}
		else {
			char *av = argv[ArgCounter] + 1;
			if (strcmp(av, "bsd") == 0)
				av = "bsd_daemon";
			for (loop=0;loop<animal_count;loop++) {
				if (strcmp(av,animals[loop]->name)==0) {
					animal = animals[loop];
					found = 1;
				}
			}
			if (!found) {
				fprintf(stderr,
				        "%s: Unknown option \"%s\".\n", ProgramName,
				        argv[ArgCounter]);
				Usage();
				exit(1);
			}
		}
	}

	if (strlen(theDisplayName) < 1) {
		theDisplayName = NULL;
	}
}


/*
 *      メイン関数
 */

int main(int argc, char** argv) {
	char theDisplayName[MAXDISPLAYNAME];
	
	ProgramName = argv[0];
	
	argc--;
	argv++;
	
	GetArguments(argc, argv, theDisplayName);
	
	//XSetErrorHandler(NekoErrorHandler);
	
	InitScreen(theDisplayName);
	
	signal(SIGALRM, NullFunction);
	signal(SIGINT, RestoreCursor);
	signal(SIGTERM, RestoreCursor);
	signal(SIGQUIT, RestoreCursor);
	
	SinPiPer8Times3 = sin(PI_PER8 * (double)3);
	SinPiPer8 = sin(PI_PER8);
	
	ProcessNeko();
	
	RestoreCursor();
}

void define_animal(Animal* x) {
	/*Frame f0 = x->frame_list[0];
	for (int i=1; i<32; i++) {
		x->frame_list[i] = (Frame){
			&f0.bits[32*32/8*i],
			&f0.mask[32*32/8*i],
		};
		}*/
	animals[animal_count] = x;
	animal_count++;
}
