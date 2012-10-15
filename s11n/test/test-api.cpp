#include "umundo/s11n.h"
#include "umundo/s11n/protobuf/PBSerializer.h"
#include "umundo/core.h"
#include "Test1.pb.h"

using namespace umundo;

class TestTypedReceiver : public TypedReceiver {
	void receive(void* obj, Message* msg) {
    std::cout << msg->getMeta("um.s11n.type") << std::endl;
    if (msg->getMeta("um.s11n.type").compare("Test1Msg") == 0) {
      // we got an explicit type from tSub->registerType
			Test1Msg* tMsg = (Test1Msg*)obj;
      std::cout << tMsg->doubletype() << ": " << tMsg->stringtype() << std::endl;
		} else if (msg->getMeta("um.s11n.type").compare("google::protobuf::Message") == 0) {
      // we only have the descriptor from PBSerializer::addProto
      google::protobuf::Message* tMsg = (google::protobuf::Message*)obj;
      tMsg->PrintDebugString();
    }
	}
};

int main(int argc, char** argv) {
	Node* mainNode = new Node();
	Node* otherNode = new Node();
	TestTypedReceiver* tts = new TestTypedReceiver();
	TypedPublisher* tPub = new TypedPublisher("fooChannel");
	TypedSubscriber* tSub = new TypedSubscriber("fooChannel", tts);

  PBSerializer::addProto("/Users/sradomski/Documents/TK/Code/umundo/build/xcode/protobuf/generated");
	
  tSub->registerType("Test1Msg", new Test1Msg());
	tPub->registerType("Test1Msg", new Test1Msg());

	mainNode->addPublisher(tPub);
	otherNode->addSubscriber(tSub);

	// try a typed message for atomic types
	Test1Msg* tMsg1 = new Test1Msg();
	Test2Msg* tMsg2 = new Test2Msg();
  tPub->waitForSubscribers(1);
  
  int iterations = 10;
	while(iterations--) {
		Thread::sleepMs(10);
    tMsg1->set_doubletype(iterations);
    tMsg1->set_stringtype("foo");
		tPub->sendObj("Test1Msg", tMsg1);
    tMsg2->set_doubletype(iterations);
    tMsg2->set_stringtype("foo");
		tPub->sendObj("Test2Msg", tMsg2);
	}
  
//  mainNode->removePublisher(tPub);
//  otherNode->removeSubscriber(tSub);
//  
//  delete tPub;
//  delete tSub;
//  delete mainNode;
//  delete otherNode;
}