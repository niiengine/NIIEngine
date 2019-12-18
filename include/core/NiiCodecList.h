/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-3-15

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/
///@warnning 不要手动修改此文件
///@warnning Do not modify this file manually
#ifndef _NII_CODEC_LIST_
#define _NII_CODEC_LIST_

/** 统一编码标识
@note 属于 CodecID 范畴
@version NIIEngine 3.0.0
*/
#define N_CODEC_UNKNOW          0
#define N_IMAGE_NONE            1
#define N_IMAGE_BMP             2
#define N_IMAGE_RAW             3
#define N_IMAGE_SGI             4
#define N_IMAGE_TIFF            5
#define N_IMAGE_PCX             6
#define N_IMAGE_PNM             7
#define N_IMAGE_PIC             8
#define N_IMAGE_ICO             9
#define N_IMAGE_XPM             10
#define N_IMAGE_TGA             11
#define N_IMAGE_HDR             12
#define N_IMAGE_JPEG            13
#define N_IMAGE_JASC_PAL        14
#define N_IMAGE_PNG             15
#define N_IMAGE_PIX             16
#define N_IMAGE_PXR             17
#define N_IMAGE_DDS             18
#define N_IMAGE_GIF             19
#define N_IMAGE_LIF             20
#define N_IMAGE_MDL             21
#define N_IMAGE_MNG             22
#define N_IMAGE_CUT             23
#define N_IMAGE_JNG             24
#define N_IMAGE_PCD             25
#define N_IMAGE_PSD             26
#define N_IMAGE_JASPER          27
#define N_IMAGE_OPENEXR         28
#define N_IMAGE_WAL             30
#define N_IMAGE_OPENJPEG        31
#define N_IMAGE_PSP             32
#define N_IMAGE_PVRT            33
#define N_IMAGE_INNER1			34
#define N_IMAGE_INNER2			35
#define N_IMAGE_INNER3			36
#define N_IMAGE_INNER4			37
#define N_IMAGE_INNER5			38
#define N_IMAGE_INNER6			39
#define N_IMAGE_INNER7			40
#define N_IMAGE_INNER8			41
#define N_IMAGE_INNER9			42
#define N_IMAGE_EXT20           43
#define N_IMAGE_EXT19           44
#define N_IMAGE_EXT18           45
#define N_IMAGE_EXT17           46
#define N_IMAGE_EXT16           47
#define N_IMAGE_EXT15           48
#define N_IMAGE_EXT14           49
#define N_IMAGE_EXT13           50
#define N_IMAGE_EXT12           51
#define N_IMAGE_EXT11           52
#define N_IMAGE_EXT10           53
#define N_IMAGE_EXT9            54
#define N_IMAGE_EXT8            55
#define N_IMAGE_EXT7            56
#define N_IMAGE_EXT6            57
#define N_IMAGE_EXT5            58
#define N_IMAGE_EXT4            59
#define N_IMAGE_EXT3            60
#define N_IMAGE_EXT2            61
#define N_IMAGE_EXT1            62

#define N_AUDIO_NONE            100
#define N_AUDIO_MP2             101
#define N_AUDIO_MP3             102
#define N_AUDIO_AAC             103
#define N_AUDIO_AC3             104
#define N_AUDIO_DTS             105
#define N_AUDIO_VORBIS          106
#define N_AUDIO_DVAUDIO         107
#define N_AUDIO_WMAV1           108
#define N_AUDIO_WMAV2           109
#define N_AUDIO_MACE3           110
#define N_AUDIO_MACE6           111
#define N_AUDIO_VMDAUDIO        112
#define N_AUDIO_FLAC            113
#define N_AUDIO_MP3ADU          114
#define N_AUDIO_MP3ON4          115
#define N_AUDIO_SHORTEN         116
#define N_AUDIO_ALAC            117
#define N_AUDIO_WESTWOOD_SND1   118
#define N_AUDIO_GSM             119    ///< as in Berlin toast sf
#define N_AUDIO_QDM2            120
#define N_AUDIO_COOK            130
#define N_AUDIO_TRUESPEECH      131
#define N_AUDIO_TTA             132
#define N_AUDIO_SMACKAUDIO      133
#define N_AUDIO_QCELP           134
#define N_AUDIO_WAVPACK         135
#define N_AUDIO_DSICINAUDIO     136
#define N_AUDIO_IMC             137
#define N_AUDIO_MUSEPACK7       138
#define N_AUDIO_MLP             139
#define N_AUDIO_GSM_MS          140  /* as found in WAV */
#define N_AUDIO_ATRAC3          141
#define N_AUDIO_VOXWARE         142
#define N_AUDIO_APE             143
#define N_AUDIO_NELLYMOSER      144
#define N_AUDIO_MUSEPACK8       145
#define N_AUDIO_SPEEX           146
#define N_AUDIO_WMAVOICE        147
#define N_AUDIO_WMAPRO          148
#define N_AUDIO_WMALOSSLESS     149
#define N_AUDIO_ATRAC3P         150
#define N_AUDIO_EAC3            151
#define N_AUDIO_SIPR            152
#define N_AUDIO_MP1             153
#define N_AUDIO_TWINVQ          154
#define N_AUDIO_TRUEHD          155
#define N_AUDIO_MP4ALS          156
#define N_AUDIO_ATRAC1          157
#define N_AUDIO_BINKAUDIO_RDFT  158
#define N_AUDIO_BINKAUDIO_DCT   159
#define N_AUDIO_AAC_LATM        160
#define N_AUDIO_QDMC            161
#define N_AUDIO_CELT            162
#define N_AUDIO_G723_1          163
#define N_AUDIO_G729            164
#define N_AUDIO_8SVX_EXP        165
#define N_AUDIO_8SVX_FIB        166
#define N_AUDIO_BMV_AUDIO       167
#define N_AUDIO_RALF            168
#define N_AUDIO_FFWAVESYNTH     169
#define N_AUDIO_8SVX_RAW        170
#define N_AUDIO_SONIC           171
#define N_AUDIO_SONIC_LS        172
#define N_AUDIO_EXT20           173
#define N_AUDIO_EXT19           174
#define N_AUDIO_EXT18           175
#define N_AUDIO_EXT17           176
#define N_AUDIO_EXT16           177
#define N_AUDIO_EXT15           178
#define N_AUDIO_EXT14           179
#define N_AUDIO_EXT13           180
#define N_AUDIO_EXT12           181
#define N_AUDIO_EXT11           182
#define N_AUDIO_EXT10           183
#define N_AUDIO_EXT9            184
#define N_AUDIO_EXT8            185
#define N_AUDIO_EXT7            186

#define N_MODEL_MD2             300
#define N_MODEL_MD3             301
#define N_MODEL_MD5             302
#define N_MODEL_FBO             303
#define N_MODEL_EXT20           304
#define N_MODEL_EXT19           305
#define N_MODEL_EXT18           306
#define N_MODEL_EXT17           307
#define N_MODEL_EXT16           308
#define N_MODEL_EXT15           309
#define N_MODEL_EXT14           310
#define N_MODEL_EXT13           311
#define N_MODEL_EXT12           312
#define N_MODEL_EXT11           313
#define N_MODEL_EXT10           314
#define N_MODEL_EXT9            315
#define N_MODEL_EXT8            316
#define N_MODEL_EXT7            317
#define N_MODEL_EXT6            318
#define N_MODEL_EXT5            319
#define N_MODEL_EXT4            320
#define N_MODEL_EXT3            321
#define N_MODEL_EXT2            322
#define N_MODEL_EXT1            323

#define N_VIDEO_EXT20           400
#define N_VIDEO_EXT19           401
#define N_VIDEO_EXT18           402
#define N_VIDEO_EXT17           403
#define N_VIDEO_EXT16           404
#define N_VIDEO_EXT15           405
#define N_VOICE_EXT14           406
#define N_VOICE_EXT13           407
#define N_VOICE_EXT12           408
#define N_VOICE_EXT11           409

#endif