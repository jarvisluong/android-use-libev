#ifndef _TRANS_HANDLER_H
#define _TRANS_HANDLER_H


class TransInst;


class TransHandler
{
public:
    TransHandler(TransInst *pTransInst);
    virtual ~TransHandler();

    virtual DWORD Init(sem_t *pEndEvent);
    virtual void Fini();

    virtual TransHandler *Clone(TransInst *pTransInst);

    virtual void OnStarted() = 0;
    virtual void OnStopped() = 0;
    virtual void OnRecvReady() = 0;

    bool IsSessionEnd();

protected:
    void EndSession();

protected:
    TransInst                 * _pTransInst;

private:
    bool                        _bSessionEnd;
    sem_t                     * _pEndEvent;
};


#endif
