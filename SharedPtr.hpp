#ifndef XXX_SHARED_PTR_XXX
#define XXX_SHARED_PTR_XXX
#include <utility>
#include <atomic>
#include <typeinfo>
#include <iostream>
#include <cxxabi.h>
#include <cassert>
  auto demangle = [](const std::type_info &ti) {
	      int ec;
	          return abi::__cxa_demangle(ti.name(), 0, 0, &ec);
		      assert(ec == 0);
		        };
namespace cs540{
	class RefCount{
		public:
			std::atomic<int> ref_count;
			RefCount() : ref_count(0){};
	};
	class DeleterBase{
		public:
			virtual void operator()() = 0;
			virtual ~DeleterBase(){};
	};
	template<typename U>
	class Deleter : public DeleterBase{
		public:
			U *ptr;
			Deleter(U* p) : ptr(p){
			};
			void operator()(){
				delete ptr;
			};
	};
	class SharedPtrBase{
		public:	
			DeleterBase* del;
			RefCount* reference_counter;
			SharedPtrBase(RefCount *rc, DeleterBase* db) : del(db), reference_counter(rc){}; 	
	};
	template<typename T>
	class SharedPtr : public SharedPtrBase{	
		private:
			T* data;	
		public:
			//constructors
			SharedPtr() : SharedPtrBase(nullptr, nullptr), data(nullptr){};

			template<typename U>
				explicit SharedPtr(U* u) : SharedPtrBase(new RefCount(), new Deleter<U>(u)), data(u){
					reference_counter->ref_count++;
				};

			SharedPtr(const SharedPtr &p) : SharedPtrBase(p.reference_counter, p.del), data(p.get()){
				if(reference_counter != nullptr && data != nullptr){
					reference_counter->ref_count++;	
				}
			};

			template<typename U> 
				SharedPtr(const SharedPtr<U> &p) : SharedPtrBase(p.reference_counter, p.del), data(p.get()){
					if(reference_counter != nullptr && data != nullptr){
							reference_counter->ref_count++;
					}
				};

			SharedPtr(SharedPtr &&p) :  SharedPtrBase(std::move(p.reference_counter), std::move(p.del)), data(std::move(p.get())){
			};
			
			template<typename U> 
			SharedPtr(SharedPtr<U> &&p) : SharedPtrBase(std::move(p.reference_counter), std::move(p.del)), data(std::move(p.get())){
			};

			template<typename U, typename P>
			SharedPtr(const SharedPtr<U> &p, P *ptr) : SharedPtrBase(p.reference_counter, p.del), data(ptr){ 
				reference_counter->ref_count++;
			};

			SharedPtr& operator=(const SharedPtr &p){
				if(p.get() == data){
					return *this;
				}
				if(reference_counter){
					reference_counter->ref_count--;
					if(reference_counter->ref_count.load() == 0){
						(*del)();
						delete reference_counter;
						delete del;
					}
				}
				data = p.get();
				reference_counter = p.reference_counter;
				if(data != nullptr){
					reference_counter->ref_count++;
				}
				del = p.del;
				return *this;
			};
			template<typename U> 
				SharedPtr<T>& operator=(const SharedPtr<U> &p){
					if(reference_counter){
						reference_counter->ref_count--;
						if(reference_counter->ref_count.load() == 0){
							(*del)();
							delete reference_counter;
							delete del;
						}
					}
					del = p.del;
					if(p.del!= nullptr){
						p.reference_counter->ref_count++;
					}
					reference_counter = p.reference_counter;
					//data = static_cast<Deleter<U>*>(del)->ptr;
					data = p.get();
					return *this;	
				};
			SharedPtr& operator=(SharedPtr &&p){
				//Move assignment operator, use info from move constructor
				data = std::move(p.get());
				reference_counter = std::move(p.reference_counter);
				p.reference_counter = nullptr;
				del = std::move(p.del);
				p.del = nullptr;
				return *this;
			};
			template<typename U> 
				SharedPtr& operator=(SharedPtr<U> &&p){
					data = std::move(p.get());
					reference_counter = std::move(p.reference_counter);
					p.reference_counter = nullptr;
					del = std::move(p.del);
					p.del = nullptr;
					return *this;
				};
			//destructor
			~SharedPtr(){
				if(reference_counter){
					reference_counter->ref_count--;
					if(reference_counter->ref_count.load() == 0){
						(*del)();
						delete reference_counter;
						delete del;
					}
				}
				reference_counter = nullptr;
				del = nullptr;
				data = nullptr;
			};
			//modifiers
			void reset(){
				if(reference_counter){
					reference_counter->ref_count--;
					if(reference_counter->ref_count.load() == 0){
						(*del)();
						delete reference_counter;
						delete del;
					}
				}	
				data = nullptr;
				reference_counter = nullptr;
				del = nullptr;
			};
			template<typename U> 
				void reset(U *p){
					//what is different about working with the member templates? confused here
					reset();
					data = p;
					reference_counter = new RefCount();
					reference_counter->ref_count++;
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
			if(!lhs && !rhs){
				return true;
			}	
			return lhs.get() == rhs.get();
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

	template<typename T, typename U>
		SharedPtr<T> static_pointer_cast(const SharedPtr<U> &sp){
			auto p = static_cast<T*>(sp.get());
			return SharedPtr<T>(sp, p);
		}
	template<typename T, typename U>
		SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &sp){
			if (auto p = dynamic_cast<T*>(sp.get())) {
				return SharedPtr<T>(sp, p);
			} else {
				return SharedPtr<T>();
			}
		}			

}
#endif
