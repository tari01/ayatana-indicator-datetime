/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Charles Kerr <charles.kerr@canonical.com>
 */

#include <datetime/clock-watcher.h>

namespace unity {
namespace indicator {
namespace datetime {

/***
****
***/

ClockWatcherImpl::ClockWatcherImpl(const std::shared_ptr<const State>& state):
    m_state(state)
{
    m_state->planner->upcoming.changed().connect([this](const std::vector<Appointment>&){
        g_debug("ClockWatcher pulse because upcoming appointments changed");
        pulse();
    });
    m_state->clock->minute_changed.connect([this](){
        g_debug("ClockWatcher pulse because clock minute_changed");
        pulse();
    });
    pulse();
}

core::Signal<const Appointment&>& ClockWatcherImpl::alarm_reached()
{
    return m_alarm_reached;
}

void ClockWatcherImpl::pulse()
{
    const auto now = m_state->clock->localtime();

    for(const auto& appointment : m_state->planner->upcoming.get())
    {
        if (m_triggered.count(appointment.uid))
            continue;
        if (!DateTime::is_same_minute(now, appointment.begin))
            continue;

        m_triggered.insert(appointment.uid);
        m_alarm_reached(appointment);
    }
}

/***
****
***/

} // namespace datetime
} // namespace indicator
} // namespace unity