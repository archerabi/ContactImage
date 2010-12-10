#ifndef SIGNALGUARD_H
#define SIGNALGUARD_H

#include <QObject>

//! This is a concept similar to Alexandrescu & Marginean's ScopeGuard.
//! The class ensures through RAII that a signal will be emitted in the case
//! of early returns.
class SignalGuard : public QObject
{
   Q_OBJECT
public:
   SignalGuard(QObject* target, const char* method);
   ~SignalGuard();

   void dismiss()
   {
      mShouldEmit = false;
   }

signals:
   void triggerSignal();

private:
   bool mShouldEmit;
};

#endif // SIGNALGUARD_H
