#pragma once
#include <chrono>

class Counter
{
public:
    Counter()
    {
        start();
    }

    void start()
    {
        start_time = std::chrono::steady_clock::now();
    }

    /**
     * @return elapsed time in seconds.
     */
    float elapsed_time()
    {
        return std::chrono::duration<float>(std::chrono::steady_clock::now() - start_time).count();
    }

private:
    std::chrono::steady_clock::time_point start_time;
};

/**
  This is a simple ramp component. It is able to handle both positive and negative values.
  */
class Ramp
{
private:
    float start;
    float end;
    float ramp_up_time;
    float ramp_down_time;
    Counter counter;

    enum RampState
    {
        init_state,
        ramp_up_state,
        ramp_down_state,
        ramp_stop_state,
        end_state
    };
    RampState ramp_state;

    bool is_finished(float start, float end, float out)
    {
        if (end > start) // forward direction
        {
            if (out >= end)
                return true;
        }
        else if (end < start) // reverse direction
        {
            if (out <= end)
                return true;
        }
        else if (end == start)
        {
            return true;
        }
        return false;
    }

public:
    void reset(float in_start,
               float in_end,
               float in_ramp_up_time,
               float in_ramp_down_time)
    {
        counter.start();
        start = in_start;
        end = in_end;
        ramp_up_time = in_ramp_up_time;
        ramp_down_time = in_ramp_down_time;
        ramp_state = init_state;
    }

    float tick()
    {
        switch (ramp_state)
        {
        case init_state:
        {
            bool zero_cross = start * end < 0;
            if (zero_cross)
            {
                ramp_state = ramp_stop_state;
                return start;
            }

            if (!zero_cross && end < 0.0f || start < 0.0f)
            {
                // reverse direction
                ramp_state = end < start ? ramp_up_state : ramp_down_state;
                return start;
            }
            else
            {
                // forward direction
                ramp_state = end > start ? ramp_up_state : ramp_down_state;
                return start;
            }
        }
        break;
        case ramp_up_state:
        {
            float slope = (end - start) / ramp_up_time;
            float out = start + slope * counter.elapsed_time();

            if (is_finished(start, end, out))
            {
                ramp_state = end_state;
                return end;
            }
            return out;
        }
        break;
        case ramp_down_state:
        {
            float slope = (end - start) / ramp_down_time;
            float out = start + slope * counter.elapsed_time();

            if (is_finished(start, end, out))
            {
                ramp_state = end_state;
                return end;
            }
            return out;
        }
        break;
        case ramp_stop_state:
        {
            float slope = (0.0f - start) / ramp_down_time;
            float out = start + slope * counter.elapsed_time();

            if (is_finished(start, 0.0f, out))
            {
                start = 0.0f;
                counter.start();
                ramp_state = ramp_up_state;
                return 0.0f;
            }
            return out;
        }
        break;
        case end_state:
        {
            return end;
        }
        break;
        }
        assert(false);
    }
};
