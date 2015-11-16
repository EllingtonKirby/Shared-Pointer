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
			class DeleterBase{
				virtual void operator()() = 0;
				virtual ~DeleterBase();
			};
			template<typename U>
			class Deleter : public DeleterBase{
				public:
					Deleter(U *p) : ptr(p){};
					void operator()(){
						delete ptr;
					};
					U* ptr;
			};
		private:
			T* data;
			RefCount* reference_counter;
			Deleter<T>* del;
		public:
			//constructors
			SharedPtr() : data(nullptr){
				reference_counter = new RefCount();
				reference_counter->addReference();
				del = new Deleter<T>(data);
			};
			template<typename U>
				explicit SharedPtr(U* u) : data(u), del(u){
					data = u;
					reference_counter = new RefCount();
					reference_counter->addReference();
					del = new Deleter<U>(data);
				};
			SharedPtr(const SharedPtr &p) : data(*p), reference_counter(p->reference_counter), del(*p){
				reference_counter->addReference();	
			};
			template<typename U> 
				SharedPtr(const SharedPtr<U> &p) : data(*p){
					if(reference_counter->freeReference() == 0){
						(*del)();
						delete reference_counter;
						delete del;
					}
					reference_counter = p.reference_counter;
					reference_counter->addReference();
					del = p.del;
				};
			SharedPtr(SharedPtr &&p) : data(std::move(p.get())), reference_counter(std::move(p.reference_counter)), del(std::move(p.del)) {
				p = nullptr;
			};
			template<typename U> 
				SharedPtr(SharedPtr<U> &&p) : data(std::move(p.get())), reference_counter(std::move(p.reference_counter)), del(std::move(p.del)){
					p = nullptr;
				};
			SharedPtr& operator=(const SharedPtr &p){
				if(*p == *data){
					return *this;
				}
				if(reference_counter->freeReference() == 0){
					(*del)();
					delete reference_counter;
					delete del;
				}
				//Is this correct?
				data = p.get();
				reference_counter = p.reference_counter;
				if(data != nullptr){
					reference_counter->addReference();
				}
				del = p.del;
				return *this;
			};
			template<typename U> 
				SharedPtr<T>& operator=(const SharedPtr<U> &p){
					if(*p == *data){
						return *this;
					}
					if(reference_counter->freeReference() == 0){
						//Do we need to free it in this case?
						(*del)();
						delete reference_counter;
						delete del;
					}
					//Is this correct?
					data = p.get();
					reference_counter = p.reference_counter;
					if(data!= nullptr){
						reference_counter->addReference();
					}
					del = p.del;
					return *this;	
				};
			SharedPtr& operator=(SharedPtr &&p){
				//Move assignment operator, use info from move constructor
				data = std::move(p.get());
				reference_counter = std::move(p.reference_counter);
				del = std::move(p.del);
				p = nullptr;
				return *this;
			};
			template<typename U> 
				SharedPtr& operator=(SharedPtr<U> &&p){
					//Same questions as above
					data = std::move(p.get());
					reference_counter = std::move(p.reference_counter);
					del = std::move(p.del);
					p = nullptr;
					return *this;
				};
			//destructor
			~SharedPtr(){
				if(reference_counter->freeReference() == 0){
					(*del)();
					delete reference_counter;
					delete del;
				}
			};
			//modifiers
			void reset(){
				if(reference_counter->freeReference() == 0){
					 (*del)();
					delete reference_counter;
					delete del;
				}
				data = nullptr;
			};
			template<typename U> 
				void reset(U *p){
					//what is different about working with the member templates? confused here
					reset();
					data = p;
					reference_counter = new RefCount();
					reference_counter->addReference();
					del = new Deleter<U>(data);
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
		SharedPtr<T> static_pointer_cast(const SharedPtr<U> &sp){
			SharedPtr<T> retval(static_cast<T*>(sp.get()));
			return retval;
		}
	template<typename T, typename U>
		SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &sp){
			SharedPtr<T> retval(dynamic_cast<T*>(sp.get()));
			return retval;
		}			

}
#endif
