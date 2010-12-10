#include "SignalGuard.h"

SignalGuard::SignalGuard(QObject* target, const char* method) :
   mShouldEmit(true)
{
   connect(this, SIGNAL(triggerSignal()), target, method);
}

SignalGuard::~SignalGuard()
{
   if( mShouldEmit )
      emit triggerSignal();
}
