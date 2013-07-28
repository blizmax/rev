//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Creted by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// on 2013/May/13
//----------------------------------------------------------------------------------------------------------------------
#ifndef _REV_CORE_EVENT_EVENT_H_
#define _REV_CORE_EVENT_EVENT_H_

#include <functional>
#include <vector>

namespace rev
{
	
	template<typename Ret_, typename ... Arg_>
	class Event
	{
		typedef Function std::function<Ret_(Arg_)>;
	public:
		class Delegate {
		public:
			Delegate(std::function<Ret_(Arg_)> _f) : mF(_f) {}
			Ret_	operator() (Arg_ _args) { return mF(_args); }
		private:
			std::function<Ret_(Arg_)>	mF;
		};

		~Event()
		{
			for(auto d : mDelegates)
				delete d;
		}

		void operator(Arg_ _args)
		{
			for(auto deleg : mDelegates)
				deleg(_args);
		}

		Delegate* operator+= (std::function<Ret_(Arg_)> _f) {
			Delegate* del = new Delegate(_f);
			mDelegates.push_back(del);
			return del;
		}

		void operator-= (Delegate* _del)
		{
			for(auto i = mDelegates.begin(); i != mDelegates.end(); ++i)
				if(*i == _del) {
					delete _del;
					return;
				}
		}

	private:
		std::vector<Delegate>	mDelegates;
	};

	//----------------------------------------------------------
	// Inline implementation
	//----------------------------------------------------------

}	// namespace rev

#endif // _REV_CORE_EVENT_EVENT_H_
