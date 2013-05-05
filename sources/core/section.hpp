#pragma once

#include <string>

#include <boost/foreach.hpp>

#include "dkernel.h"

namespace core{

class section
	{
	public:
						section(unsigned int ParametersCount, unsigned int StringsCount);
		virtual			~section(){};
		virtual void	on_load(){};
		virtual void	on_init(){};
		virtual void	on_render(float RunTime){};
		virtual void	on_end(){};
		std::string		get_string(unsigned int n) const {return mySection->strings[n];}
		float			get_float(unsigned int n) const {return mySection->param[n];}

	private:
		class	delete_me
			{
			private:
				~delete_me()
					{
//					BOOST_FOREACH(::sl::Patterns::CAutoList<section>* pSection, mylist)
//						delete dynamic_cast<section*>(pSection);
					}
			};
		
		static	delete_me m_DeleteMe;
	};
}