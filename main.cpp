#include <iostream>
#include <utility>
#include <list>
#include <QString>
#include <QCoreApplication>
#include <QDateTime>
#include <QTextStream>
#include <QTimeZone>
#include <QDebug>

static std::list< std::pair< uint64_t, QString > > table =
{
	{1628538170,"2021/08/09 12:42:50"}
	,{1628538131,"2021/08/09 12:42:11" }
	,{1628537141, "2021/08/09 12:25:41" }
	,{1628537078, "2021/08/09 12:24:38" }
	,{1628536296,"2021/08/09 12:11:36"}
	,{1628536220,"2021/08/09 12:10:20" }
	,{1628535867, "2021/08/09 12:04:27" }
	,{1628535841, "2021/08/09 12:04:01" }
	,{1628534459,"2021/08/09 11:40:59"}
	,{1628533060,"2021/08/09 11:17:40" }
	,{1628533051, "2021/08/09 11:17:31" }
	,{1628018683, "2021/08/03 12:24:43" }
	,{1627511526, "2021/07/28 15:32:06" }
	,{1627511487,"2021/07/28 15:31:27"}
	,{1627371789,"2021/07/27 00:43:09" }
	,{1627371752, "2021/07/27 00:42:32" }
	,{1627244388, "2021/07/25 13:19:48" }
	,{1627243558,"2021/07/25 13:05:58"}
	,{1627241300,"2021/07/25 12:28:20"}
	,{1627241228,"2021/07/25 12:27:08"}

	,{1626682331,"2021/07/19 01:12:11"}
	,{1626682241,"2021/07/19 01:10:41"}
	,{1626681417,"2021/07/19 00:56:57"}
	,{1626680659,"2021/07/19 00:44:19"}

	,{1626679590,"2021/07/19 00:26:30"}
	,{1626679292,"2021/07/19 00:21:32"}
	,{1626382453,"2021/07/15 13:54:13"}
};

QTextStream& qStdOut()
{
	static QTextStream ts(stdout);
	return ts;
}

int main( int argc, char ** argv )
{
	for (auto&& ii : table)
	{
		auto intvalue = ii.first;
		auto strvalue = ii.second;
		auto ts = QDateTime::fromString(strvalue, "yyyy/MM/dd hh:mm:ss" );
		if (!ts.isValid())
			qStdOut() << "Could not read in value " << strvalue << Qt::endl;
		else if (ts.toString("yyyy/MM/dd hh:mm:ss") != strvalue)
			qStdOut() << "different value calculated for: Should be '" << strvalue << "' but is '" << ts.toString("yyyy/MM/dd hh:mm:ss" ) << "'" << Qt::endl;
		else
		{
			auto otherTime = ts.addSecs(-1 * intvalue);
			qStdOut() << "Othertime= " << otherTime.toString("yyyy/MM/dd hh:mm:ss t") << Qt::endl;

			auto utc = otherTime.toUTC();
			utc.setTimeSpec(Qt::OffsetFromUTC);
			qStdOut() << "utc= " << utc.toString("yyyy/MM/dd hh:mm:ss t") << Qt::endl;
		}

	}
}
