namespace timing
{
inline int64_t& _Var_start_seconds()
{
    static int64_t start_seconds;
    return start_seconds;
}

inline int64_t& _Var_stop_seconds()
{
    static int64_t stop_seconds;
    return stop_seconds;
}

inline int64_t& _Var_start_micro_seconds()
{
    static int64_t start_micro_seconds;
    return start_micro_seconds;
}

inline int64_t& _Var_stop_micro_seconds()
{
    static int64_t stop_micro_seconds;
    return stop_micro_seconds;
}

inline void startTimer()
{
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    _Var_start_seconds() = current_time.tv_sec;
    _Var_start_micro_seconds() = current_time.tv_usec;
}

inline void stopTimer()
{
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    _Var_stop_seconds() = current_time.tv_sec;
    _Var_stop_micro_seconds() = current_time.tv_usec;
}

}  // namespace timing

#define DUOPLOT_TIC() duoplot::timing::startTimer()

#define DUOPLOT_TOC_MS(msg)                                                                                                \
    {                                                                                                                  \
        duoplot::timing::stopTimer();                                                                                      \
        int64_t delta_seconds = duoplot::timing::_Var_stop_seconds() - duoplot::timing::_Var_start_seconds();                  \
        int64_t delta_microseconds = duoplot::timing::_Var_stop_micro_seconds() - duoplot::timing::_Var_start_micro_seconds(); \
        int64_t delta_time = delta_seconds * 1000000 + delta_microseconds;                                             \
        DUOPLOT_LOG_DEBUG() << msg << static_cast<float>(delta_time) / 1000.0f << " ms";                                   \
    }

#define DUOPLOT_TOC_US(msg)                                                                                                \
    {                                                                                                                  \
        duoplot::timing::stopTimer();                                                                                      \
        int64_t delta_seconds = duoplot::timing::_Var_stop_seconds() - duoplot::timing::_Var_start_seconds();                  \
        int64_t delta_microseconds = duoplot::timing::_Var_stop_micro_seconds() - duoplot::timing::_Var_start_micro_seconds(); \
        int64_t delta_time = delta_seconds * 1000000 + delta_microseconds;                                             \
        DUOPLOT_LOG_DEBUG() << msg << delta_time << " us";                                                                 \
    }
