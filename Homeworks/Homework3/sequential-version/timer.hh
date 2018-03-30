#include <chrono>
#include <functional>

class Timer {
private:
  //////////////////////
  // Type definitions //
  //////////////////////
  typedef std::chrono::high_resolution_clock Clock;
  typedef std::chrono::time_point<Clock> Time;
#ifdef TIMER_USES_MICROSECONDS
  typedef std::chrono::microseconds TimeUnit;
#else
  typedef std::chrono::milliseconds TimeUnit;
#endif
public:
  typedef std::function<void(long long duration)> CallBack;

private:
  /**
   * @brief A function object that is called with the lifetime on destruction.
   *
   * The function takes as argument a value of type long long with the lifetime.
   * The units of that time depends on the chosen resolution of the timer.
   *
   * If no callback is provided, a default callback that does nothing is used by
   * default.
   */
  CallBack onDestructionCall_;
  /// Construction time of the object
  Time construction_;

public:
  /// Constructor
  Timer(CallBack onDestructionCall = [](long long duration) -> void {})
      : onDestructionCall_(onDestructionCall), construction_(Clock::now()) {
    /*
     Note: Do not put anything here as it will count on the time the object
     is measuring. If you really need to do that then move the
     initialization of the construction_ attribute to be the last thing the
     constructor does.
     */
  }
  // copy constructor
  Timer(const Timer &st)
      : onDestructionCall_(st.onDestructionCall_),
        construction_(st.construction_) {}
  /// Returns the elapsed time since the construction of the object
  long long elapsed(void) const {
    auto now = Clock::now();
    return std::chrono::duration_cast<TimeUnit>(now - construction_).count();
  }
  /// Destructor
  ~Timer(void) { onDestructionCall_(elapsed()); }
};

inline std::ostream &operator<<(std::ostream &os, const Timer &timer) {
  os << timer.elapsed();
#ifdef TIMER_USES_MICROSECONDS
  os << " us.";
#else
  os << " ms.";
#endif
  return os;
}

#define COUT_TIMER(name, str)                                                  \
  Timer name([](long long d) { std::cout << str << (d / 1000.0) << std::endl; })
