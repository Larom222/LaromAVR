/* 
* DisplayWindow.h
*
* Created: 2020-02-22 16:33:22
* Author: Gabriel
*/


#ifndef __DISPLAYWINDOW_H__
#define __DISPLAYWINDOW_H__

class DisplayWindow
{
public:

	virtual ~DisplayWindow();
	
	//return true if processing finished
	virtual void Finished();

	//process function
	virtual void Process();
};

#endif //__DISPLAYWINDOW_H__
