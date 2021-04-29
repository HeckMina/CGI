#pragma once
namespace Alice {
	template<typename ElementType>
	class List {
	private:
		struct ListNode {
			ElementType* mElementPtr;
			ListNode* mNext;
			ListNode(ElementType* element):mElementPtr(element),mNext(nullptr){}
		};
		int mElementCount;
		ListNode* mHead, * mTail;
	public:
		List():mElementCount(0),mHead(nullptr),mTail(nullptr){}
		void PushBack(ElementType* element) {
			ListNode* node = new ListNode(element);
			if (mHead==nullptr){
				mHead = node;
				mTail = node;
			}
			else {
				mTail->mNext = node;
				mTail = node;
			}
		}
		ElementType* PopFront() {
			ElementType*
			if (mHead!=nullptr){

			}
		}
		ElementType* Front() {
			return mHead == nullptr ? nullptr : mHead->mElementPtr;
		}
	};
}