#pragma once


template<class Type>
void Swap(Type &val1, Type &val2)
{
	Type tmp = val1;
	val1 = val2;
	val2 = tmp;
}

//////////////////////////////////////////////////////////////////////////
// sort item in range [iter1, iter2)
// for containers supoort iterator, like abase::vector
//////////////////////////////////////////////////////////////////////////

// compare using "a < b" 
template<class Iterator>
void BubbleSort(Iterator iter1, Iterator iter2)
{
	while (iter1 != iter2)
	{
		Iterator iterCur = iter1;
		Iterator iterNext = iter1; iterNext++;
		
		while (iterNext != iter2)
		{
			if (*iterNext < *iterCur) Swap(*iterCur, *iterNext);
			iterCur = iterNext;
			iterNext++;
		}
		
		iter2 = iterCur;
	}
}

// compare using pred(a, b)
template<class Iterator, class Predictor>
void BubbleSort(Iterator iter1, Iterator iter2, Predictor pred)
{
	while (iter1 != iter2)
	{
		Iterator iterCur = iter1;
		Iterator iterNext = iter1; iterNext++;
		
		while (iterNext != iter2)
		{
			if (pred(*iterNext, *iterCur)) Swap(*iterCur, *iterNext);
			iterCur = iterNext;
			iterNext++;
		}
		
		iter2 = iterCur;
	}
}


//////////////////////////////////////////////////////////////////////////
// sort item in range [pos1, pos2]
// for containers of MFC style, like AList
//////////////////////////////////////////////////////////////////////////

// compare using "a < b" 
template<class Container, class POSITION>
void BubbleSort(Container& container, POSITION pos1, POSITION pos2)
{
	while (pos1 != pos2)
	{
		POSITION posCur = pos1;
		POSITION posNext = pos1; container.GetNext(posNext);

		while (posNext != pos2)
		{
			if (container.GetAt(posNext) < container.GetAt(posCur))
				Swap(container.GetAt(posNext), container.GetAt(posCur));
			posCur = posNext;
			container.GetNext(posNext);
		}

		pos2 = posCur;
	}
}


// compare using pred(a, b)
template<class Container, class POSITION, class Predictor>
void BubbleSort(Container& container, POSITION pos1, POSITION pos2, Predictor pred)
{
	while (pos1 != pos2)
	{
		POSITION posCur = pos1;
		POSITION posNext = pos1; container.GetNext(posNext);
		
		while (posNext != pos2)
		{
			if (pred(container.GetAt(posNext), container.GetAt(posCur)))
				Swap(container.GetAt(posNext), container.GetAt(posCur));
			posCur = posNext;
			container.GetNext(posNext);
		}
		
		pos2 = posCur;
	}
}

//////////////////////////////////////////////////////////////////////////
// apply functor in range [iter1, iter2)
// for containers supoort iterator, like abase::vector
//////////////////////////////////////////////////////////////////////////

template<class Iterator, class Predictor>
void ForEach(Iterator iter1, Iterator iter2, Predictor pred)
{
	while (iter1 != iter2)
	{
		pred(*iter1);
		++iter1;
	}
}

template<class Iterator, class Predictor, typename ElemType>
void FindIf(Iterator iter1, Iterator iter2, const ElemType &valueToCompare, Predictor pred)
{
	while (iter1 != iter2)
	{
		if (*iter1 == valueToCompare)
			pred(*iter1);
		++iter1;
	}
}