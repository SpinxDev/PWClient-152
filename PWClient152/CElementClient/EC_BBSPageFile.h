/*
 * FILE: EC_BBSPageFile.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: XiaoZhou, 2005/6/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "AString.h"
#include <windows.h>

#ifndef EC_BBSPAGEFILE_H
#define EC_BBSPAGEFILE_H

const char fnLog[]="hfile.log";

enum ImageState
{
	IMG_DOWNLOADING,
	IMG_DOWNLOADED,
	IMG_RELEASED
};

struct ImageInfo
{
	AString url;
	AString fn;
	ImageState state;
};

class ImageList
{
public:
	int	m_nMax;
	int	m_nTotal;
	ImageInfo **m_Images;
public:
	ImageList();
	~ImageList();
	void Init();
	void Release();
	int AddImage(AString url);
	int GetImage(AString url);
	ImageInfo* GetImageInfo(int index);
};

struct FileImageNode
{
	FileImageNode *next;
	ImageInfo *node;
};

class FileImageList
{
public:
	FileImageNode *head;
public:
	FileImageList();
	~FileImageList();
	void AddImageNode(ImageInfo *node);
	bool IsFinishLoadingImg();
	void DeleteAllNode();
	void Release();
};

struct Attribute
{
	AString cName;
	AString cValue;
};

struct AttributeNode
{
	AttributeNode	*next;
	Attribute		*data;
};

class HTMLLabelAttribute
{
public:
	AttributeNode *Head;
public:
	HTMLLabelAttribute();
	~HTMLLabelAttribute();
	void Release();
	void AddAttribute(Attribute *attrib);
	void AddAttribute(AString cName,AString cValue);
	AString* GetAttribute(AString cName);
	bool SetAttribute(AString cName,AString cValue);
};

struct HTMLLabel
{
	HTMLLabelAttribute	*Attrib;
	HTMLLabel			*FirstChildLabel;
	HTMLLabel			*NextLabel;
	HTMLLabel			*ParentLabel;
	AString				LabelType;
	int					pos;
};

class CECBBSPageFile
{
public:
	HTMLLabel *TempLabel;
	HTMLLabel *StyleLabel;
	int iLine;
	int iLinePos;
	HTMLLabel *hEndLabel;
	AString aUrl;
	FileImageList ImgNodeList;
	char *fbuf;
	char *ch;
	char *fend;
	bool bStop;
public:
	CECBBSPageFile();
	~CECBBSPageFile();
	HTMLLabel* LoadFile(AString lFileName,AString url);
	bool LoadLabel(HTMLLabel *ParentLabel);
	void GetStyle();
	char* FindEndLabel(char *labelname);
	bool GetLabel(HTMLLabel *hLabel);
	bool GetChar(char &resChar);
	int ReadString(AString &AString);
	void AppendLog(const char *log,bool b=true);
};

void ReleaseLabel(HTMLLabel *hLabel);


extern ImageList ImgList;
extern int ImageNum;
extern bool bLoadNewImage;
extern unsigned int	EndThreadId;
extern CRITICAL_SECTION ImageSection;

#endif
