#include "EngineTools/Time.hpp"

namespace rge
{
    Stopwatch rge::Time::DeltaTimeStopwatch = Stopwatch();
    Stopwatch rge::Time::GlobalTimeStopwatch = Stopwatch();
    double rge::Time::DeltaTime = 0.0;
}
