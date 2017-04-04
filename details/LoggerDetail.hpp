#include <sstream>
#include <function>
#include "common.hpp"

namespace Carbon
{
	namespace detail
	{
		template<typename Level>
		class Message:Uncopyable
		{
		private:
			std::stringstream m_buffer;
			Level m_level;
			const std::function<void(Level,const std::stringstream&)>& m_flush;
			bool m_flag;
			Message(Level level,const std::function<void(Level,const std::stringstream&)>& flush,std::stringstream&& buffer)
			:m_level(level),m_flush(flush),m_buffer(buffer),m_flag(true){}
		public:
			Message(Level level,const std::function<void(Level,const std::stringstream&)>& flush)
			:m_level(level),m_flush(flush),m_flag(true){}
			template<typename T>
			Message operator<<(const T& x)
			{
				m_buffer<<x;
				m_flag=true;
				return Message(x);
			}
			~Message()
			{
				if(m_flag)m_flush(m_level,m_buffer);
			}
		}
	}
}
