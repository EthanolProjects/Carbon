#include <../details/LoggerDetail.hpp>

namespace Carbon
{
	template<typename Level>
	class Logger:Uncopyable
	{
	private:
		const std::function<void(Level,const std::stringstream&)> m_flush;
	public:
		Logger(const std::function<void(Level,std::stringstream&)> flush):m_flush(flush){}
		auto operator()(Level level)
		{
			return detail::Message<Level>(level);
		}
	}
}
