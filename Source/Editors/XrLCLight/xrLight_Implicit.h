#ifndef	_XRLIGHT_IMPLICIED_H_
#define	_XRLIGHT_IMPLICIED_H_

class ImplicitExecute
{
	// Data for this thread
	u32					y_start,y_end;
public:
	ImplicitExecute( u32 _y_start, u32 _y_end ): y_start(_y_start), y_end( _y_end )
	{
	
	}
	ImplicitExecute(): y_start( u32(-1) ),y_end( u32(-1) )
	{

	}
	void		Execute			();
};


#endif