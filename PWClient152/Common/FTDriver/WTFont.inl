		iforspace = 0;
		totals=totald=xtotald=ytotald=xtotals=ytotals=0;	
		ZeroMemory(m_pMem,-m_iPitch*m_iHeight);
		GetCharExtent(*pch,&offsize,&size,&bmpsize);
		if((size.cx==(2*m_iOutlineWidth))&&
			(offsize.cy+3*m_iOutlineWidth)<=bmpsize.cy)
		{	// 某些泰文的音标比较大超过了顶部 
			iforspace = bmpsize.cy-2*m_iOutlineWidth;
			if(0>(offsize.cy-bmpsize.cy+3*m_iOutlineWidth))
				iforspace = offsize.cy+bmpsize.cy-2*m_iOutlineWidth;
		}
#ifdef	WTFONT_DRAW_TEXT
		
		if(size.cy<=ystart||xoff>=nWidth)
			return TRUE;
		xtotald = xoff + offsize.cx;
		if(xtotald<0)
		{
			xtotals = -xtotald;
			xtotald = 0;
		}
		ytotald = ystart+offsize.cy;
		if(ytotald<0)
		{
			ytotals = -ytotald;
			ytotald = 0;
		}
		totald = xtotald+ytotald*nWidth;
		totals = xtotals+ytotals*m_iPitch;
#endif

#ifdef	WTFONT_DRAW_OUTLINE
		BeginPath(m_hMemDC);
		TextOutW(m_hMemDC,-offsize.cx,-offsize.cy-iforspace, pch,1);
		EndPath(m_hMemDC);
		if(!StrokePath(m_hMemDC))
			return FALSE;
#ifdef	WTFONT_DRAW_TEXT
		pdit = pOutline+totald;
		psrc = m_pBitmap+totals;
#else
		pdit = pOutline;
		psrc = m_pBitmap;
#endif
#else
		TextOutW(m_hMemDC,-offsize.cx,-offsize.cy-iforspace, pch,1);	
#ifdef	WTFONT_DRAW_TEXT
		pdit = pBitmap+totald;
		psrc = m_pBitmap+totals;
#else
		pdit = pBitmap;
		psrc = m_pBitmap;
#endif
#endif
				
		h = min(bmpsize.cy-ytotals,nHeight);
		ipitch = min(bmpsize.cx-xtotals, nWidth);
#ifdef	WTFONT_DRAW_TEXT
		if(h>(nHeight-ytotald)&&ytotald)
			h = (nHeight-ytotald)>=0?(nHeight-ytotald):0;	
		if(ipitch>(nWidth-xtotald)&&xtotald)
			ipitch = (nWidth-xtotald)>=0?(nWidth-xtotald):0;
#endif
		while(h--)
		{
			w = ipitch;
			ptmdt = pdit;
			ptmsc = psrc;
			while(w--)
			{
#ifdef	WTFONT_DRAW_TEXT
				*ptmdt = *ptmsc?*ptmsc:*ptmdt; //	考虑到斜体可能会互相叠压所以效率低些
				ptmdt++;			
				ptmsc++;
#else
				*(ptmdt++) = *(ptmsc++);
#endif				
			}
			pdit += nWidth;
			psrc += m_iPitch;
		}		
