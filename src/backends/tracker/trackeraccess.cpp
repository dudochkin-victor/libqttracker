#include "trackeraccess.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{

			TrackerQueryResult::TrackerQueryResult(bool isFinished
												 , bool isError
												 , QString const &errorMessage) :
				isFinished_(isFinished), isError_(isError), errorMessage_(errorMessage)
			{
			}

			bool TrackerQueryResult::isFinished() const
			{
				return isFinished_;
			}

			bool TrackerQueryResult::isError() const
			{
				return isError_;
			}

			QString TrackerQueryResult::errorMessage() const
			{
				return errorMessage_;
			}

			void TrackerQueryResult::setSelectResult(QVector<QStringList> const &select_result)
			{
				select_result_ = select_result;
			}

			void TrackerQueryResult::setUpdateResult(UpdateResultOperations const &update_result)
			{
				update_result_ = update_result;
			}

			QVector<QStringList> TrackerQueryResult::selectResult() const
			{
				return select_result_;
			}

			UpdateResultOperations TrackerQueryResult::updateResult() const
			{
				return update_result_;
			}
		}
	}
}
