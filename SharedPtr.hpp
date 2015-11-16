#ifndef XXX_SHARED_PTR_XXX
#define XXX_SHARED_PTR_XXX
#include <utility>
#include <atomic>

namespace cs540{
	template<typename T>
	class SharedPtr{
		public:
			class RefCount{
				//Find out if this satisfies the concurrency issues, may need to implement a Mutex
				private:
					std::atomic<int> ref_count;
				public:
					RefCount() : ref_count(0){};
					void addReference(){
						ref_count++;
					};
					int freeReference(){
						ref_count--;
						return ref_count;
					};
			};
		private:
			T* data;
			RefCount* reference_counter;
		public:
			//constructors
			SharedPtr() : data(nullptr){
				reference_counter = new RefCount();
				reference_counter->addReference();
			};
			template<typename U>
				explicit SharedPtr(U* u){
					//This is used to convert a pointer of type T to P? Not sure exactly what to do here		
				};
			SharedPtr(const SharedPtr &p) : data(*p), reference_counter(p->reference_counter){
				reference_counter->addReference();	
			};
			template<typename U> 
				SharedPtr(const SharedPtr<U> &p){
					
				};
			SharedPtr(SharedPtr &&p){
				//Ask about this, pretty confused here
			};
			template<typename U> 
				SharedPtr(SharedPtr<U> &&p){
					//ask here
				};
			SharedPtr& operator=(const SharedPtr &p){
				if(*p == *data){
					return *this;
				}
				if(reference_counter->freeReference() == 0){
					//Do we need to free it in this case?
					delete data;
					delete reference_counter;
				}
				//Is this correct?
				data = p.get();
				reference_counter = p.reference_counter;
				reference_counter->addReference();
				return *this;
			};
			template<typename U> 
				SharedPtr<T>& operator=(const SharedPtr<U> &p){
					if(*p == *data){
						return *this;
					}
					if(reference_counter->freeReference() == 0){
						//Do we need to free it in this case?
						delete data;
						delete reference_counter;
					}
					//Is this correct?
					data = p.get();
					reference_counter = p.reference_counter;
					reference_counter->addReference();
					return *this;	
				};
			SharedPtr& operator=(SharedPtr &&p){
				//What makes these different from the two above? What is &&p? Reference to a reference?
			};
			template<typename U> 
				SharedPtr& operator=(SharedPtr<U> &&p){
					//Same questions as above
				};
			//destructor
			~SharedPtr(){
				if(reference_counter->freeReference() == 0){
					delete data;
					delete reference_counter;
				}
			};
			//modifiers
			void reset(){
				if(reference_counter->freeReference() == 0){
					delete data;
					delete reference_counter;
				}
				data = nullptr;
			};
			template<typename U> 
				void reset(U *p){
					//what is different about working with the member templates? confused here
				};
			//observers
			T *get() const{
				return data;
			};
			T& operator*() const{
				return *data;
			};
			T* operator->() const{
				return data;
			};
			explicit operator bool() const{
				return data != nullptr;
			};
	};
			
	//free functions
	template<typename T1, typename T2>
		bool operator==(const SharedPtr<T1>& lhs, const SharedPtr<T2>& rhs){
			if(*lhs == nullptr && *rhs == nullptr){
				return true;
			}	
			return *lhs == *rhs;
		}
	template<typename T>
		bool operator==(const SharedPtr<T>& lhs, std::nullptr_t){
			return *lhs == nullptr;
		}
	template<typename T>
		bool operator==(std::nullptr_t, const SharedPtr<T>& rhs){
			return *rhs == nullptr;	
		}
	template<typename T1, typename T2>
		bool operator!=(const SharedPtr<T1>& lhs, const SharedPtr<T2>& rhs){
			return !(lhs == rhs);
		}
	template<typename T>
		bool operator!=(const SharedPtr<T>& lhs, std::nullptr_t){
			return *lhs != nullptr;
		}
	template<typename T>
		bool operator!=(std::nullptr_t, const SharedPtr<T>& rhs){
			return *rhs != nullptr;
		}

	//Ask about these, what is the difference here?
	template<typename T, typename U>
		SharedPtr<T> static_pointer_cast(const SharedPtr<U> &sp);
	template<typename T, typename U>
		SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &sp);								

}
#endif
