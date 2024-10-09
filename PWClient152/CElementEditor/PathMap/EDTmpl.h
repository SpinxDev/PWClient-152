//////////////////////////////////////////////////////////////////////
// EDTmpl.h: Define a set of templates for Edge Detection
// Created by He Wenfeng, 2005-01-06
//////////////////////////////////////////////////////////////////////

#ifndef _EDTMPL_H
#define _EDTMPL_H

////////////////////////////////////////////////////
//	Sobel ED(Edge Detection) template 3*3
////////////////////////////////////////////////////
static char Sobel_tmpl_Horizon_Edge[]={-1,-2,-1,0,0,0,1,2,1};		// Extract Horizon Edge
static char Sobel_tmpl_Vertical_Edge[]={-1,0,1,-2,0,2,-1,0,1};	// Extract Vertical Edge

static char Sobel_tmpl_Slash_Edge[]={-2,-1,0,-1,0,1,0,1,2};		// Extract Slash Edge		/
static char Sobel_tmpl_Backslash_Edge[]={0,-1,-2,1,0,-1,2,1,0};	// Extract BackSlash Edge	\

//////////////////////////
/*
static char tmpl_Horizon_Edge[]={-1,-1,-1,0,0,0,1,1,1};		// Extract Horizon Edge
static char tmpl_Vertical_Edge[]={-1,0,1,-1,0,1,-1,0,1};	// Extract Vertical Edge

static char tmpl_Slash_Edge[]={-1,-1,0,-1,0,1,0,1,1};		// Extract Slash Edge		/
static char tmpl_Backslash_Edge[]={0,-1,-1,1,0,-1,1,1,0};	// Extract BackSlash Edge	\
*/

#endif