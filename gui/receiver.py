from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor
from twisted.internet.task import LoopingCall
import sys, time

class HeartbeatReciever(DatagramProtocol):
    def __init__(self, name):
        self.name = name

    def startProtocol(self):
        "Called when transport is connected"
        self.transport.joinGroup('224.0.0.1')
        pass

    def stopProtocol(self):
        "Called after all transport is teared down"
        pass

    def datagramReceived(self, data, hostport):
        host, port = hostport
        now = time.localtime(time.time())
        timeStr = str(time.strftime("%y/%m/%d %H:%M:%S",now))
        print("%s received %r from %s:%d at %s" % (self.name, data, host, port, timeStr))



# heartBeatSenderObj = HeartbeatSender("sender", "224.0.0.1", 8005)

reactor.listenMulticast(8888, HeartbeatReciever("listner1"), listenMultiple=True)
# reactor.listenMulticast(8005, heartBeatSenderObj, listenMultiple=True)
reactor.run()
