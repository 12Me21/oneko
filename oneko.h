/*
 *      oneko  -  X11 猫
 */

/*
 *      インクルードファイル
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <signal.h>
#include <math.h>
#include <sys/time.h>

/*
 *      カーソルビットマップファイルの読み込み
 */

#include "cursors/cursor.include"

/*
 *      普通の猫用ビットマップファイルの読み込み
 *      These include files in the directories noted.
 *      These are the bitmaps.
 */

#include "bitmaps/neko/neko.include"
#include "bitmaps/tora/tora.include"
#include "bitmaps/dog/dog.include"
#include "bitmaps/bsd/bsd.include"
#include "bitmaps/sakura/sakura.include"
#include "bitmaps/tomoyo/tomoyo.include"

/* These are the bitmasks that allow the use of the shape extension. */

#include "bitmasks/neko/neko.mask.include"
#include "bitmasks/dog/dog.mask.include"
#include "bitmasks/bsd/bsd.mask.include"
#include "bitmasks/sakura/sakura.mask.include"
#include "bitmasks/tomoyo/tomoyo.mask.include"

/*
 *      定数定義
 */

/* １キャラクタの幅 (ピクセル) */
#define BITMAP_WIDTH            32
/* １キャラクタの高さ (ピクセル) */
#define BITMAP_HEIGHT           32

#define AVAIL_KEYBUF            255
/* Odd Only! */
#define MAX_TICK                9999            

/* フォアグラウンドカラー */
#define DEFAULT_FOREGROUND      "black"
/* バックグラウンドカラー */
#define DEFAULT_BACKGROUND      "white"

/*
 *      猫の状態定数
 */

enum {
	NEKO_STOP = 0, /* stopped */
	NEKO_JARE,		/* washing face */
	NEKO_KAKI,		/* scratching head */
	NEKO_AKUBI,		/* yawning */
	NEKO_SLEEP,		/* 寝てしまった */
	NEKO_AWAKE,		/* 目が覚めた */
	NEKO_U_MOVE,	/* 上に移動中 */
	NEKO_D_MOVE,	/* 下に移動中 */
	NEKO_L_MOVE,	/* 左に移動中 */
	NEKO_R_MOVE,	/* 右に移動中 */
	NEKO_UL_MOVE,	/* 左上に移動中 */
	NEKO_UR_MOVE,	/* 右上に移動中 */
	NEKO_DL_MOVE,	/* 左下に移動中 */
	NEKO_DR_MOVE,	/* 右下に移動中 */
	NEKO_U_TOGI,	/* scratching upper wall */
	NEKO_D_TOGI,	/* 下の壁を引っ掻いている */
	NEKO_L_TOGI,	/* 左の壁を引っ掻いている */
	NEKO_R_TOGI,	/* 右の壁を引っ掻いている */
};

/*
 *      猫のアニメーション繰り返し回数
 */

#define NEKO_STOP_TIME          4
#define NEKO_JARE_TIME          10
#define NEKO_KAKI_TIME          4
#define NEKO_AKUBI_TIME         6       /* もともと３だったけど長い方がいい */
#define NEKO_AWAKE_TIME         3
#define NEKO_TOGI_TIME          10

#define PI_PER8                 ((double)3.1415926535/(double)8)

#define MAXDISPLAYNAME          (64 + 5)        /* ディスプレイ名の長さ */

#define NOTDEFINED              (-1)

/*
 *      マクロ定義
 */

#define IsTrue(str)     (strcmp((str), "true") == 0)
