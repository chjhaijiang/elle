#ifndef CURLY_SCHED_HH
# define CURLY_SCHED_HH

# include <reactor/operation.hh>

# include <curly/asio_request.hh>

namespace curly
{
  class sched_request: public reactor::Operation
  {
  private:
    curly::asio_request _request;

  public:
    sched_request(reactor::Scheduler& sched,
                  curly::request_configuration const& conf);

  protected:
    virtual void _start() override;
    virtual void _abort() override;
  };
} /* curly */


#endif /* end of include guard: CURLY_SCHED_HH */
